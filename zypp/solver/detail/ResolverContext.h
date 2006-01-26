/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* ResolverContext.h
 *
 *Keep a set of 'active' PoolItems, these are part of the current
 *transaction
 *
 *
 *Copyright (C) 2000-2002 Ximian, Inc.
 *Copyright (C) 2005 SUSE Linux Products GmbH
 *
 *This program is free software; you can redistribute it and/or
 *modify it under the terms of the GNU General Public License,
 *version 2, as published by the Free Software Foundation.
 *
 *This program is distributed in the hope that it will be useful, but
 *WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program; if not, write to the Free Software
 *Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *02111-1307, USA.
 */

#ifndef ZYPP_SOLVER_DETAIL_RESOLVERCONTEXT_H
#define ZYPP_SOLVER_DETAIL_RESOLVERCONTEXT_H

#include "zypp/ResPool.h"
#include "zypp/PoolItem.h"
#include "zypp/Capability.h"

#include "zypp/solver/detail/Types.h"
#include "zypp/solver/detail/ResolverInfo.h"

/////////////////////////////////////////////////////////////////////////
namespace zypp
{ ///////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  namespace solver
  { /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    namespace detail
    { ///////////////////////////////////////////////////////////////////

typedef void (*ResolverContextFn) (ResolverContext_Ptr ctx, void *data);
typedef void (*MarkedPoolItemFn) (PoolItem_Ref item, void *data);
typedef void (*MarkedPoolItemPairFn) (PoolItem_Ref item1, PoolItem_Ref item2, void *data);


///////////////////////////////////////////////////////////////////
//
//	CLASS NAME : ResolverContext
class ResolverContext : public base::ReferenceCounted, private base::NonCopyable {
    

  private:

    ResolverContext_Ptr _parent;		// branches share a common parent

    typedef std::set<PoolItem_Ref> Context;
    Context _context;				// the set of items touched in this transaction

    const ResPool *_pool;

    ResolverInfoList _log;			// report log

    unsigned long long _download_size;
    unsigned long long _install_size;
    int _total_priority;
    int _min_priority;
    int _max_priority;
    int _other_penalties;

    bool _verifying;				// running 'verifySystem'
    bool _invalid;				// lead to invalid solution

  public:
    ResolverContext (ResolverContext_Ptr parent = NULL);
    virtual ~ResolverContext();

    // ---------------------------------- I/O

    friend std::ostream& operator<<(std::ostream&, const ResolverContext & context);

    // ---------------------------------- accessors

    unsigned long long downloadSize(void) const { return _download_size; }
    unsigned long long installSize(void) const { return _install_size; }
    int totalPriority (void) const { return _total_priority; }
    int minPriority (void) const { return _min_priority; }
    int maxPriority (void) const { return _max_priority; }
    int otherPenalties (void) const { return _other_penalties; }

    bool isValid (void) const { return !_invalid; }
    bool isInvalid (void) const { return _invalid; }

    bool verifying (void) const { return _verifying; }
    void setVerifying (bool verifying) { _verifying = verifying; }

    inline const ResPool *pool() const { return _pool; }
    void setPool (const ResPool *pool) { _pool = pool; }

    // ---------------------------------- methods

    /** state change functions
    *  they do some checking before actually changing the state of the PoolItem. */

    /**
     *set the state of \c item to \c status
     *If \c status is not the current state of \c item, make \c item
     *part of the current transaction (the context) */
    void setStatus (PoolItem_Ref item, ResStatus status);

    /**
     *set \c item to \a to-be-installed */
    bool install (PoolItem_Ref item, bool is_soft, int other_penalty);

    /**
     *set \c item to \a satisfied */
    bool satisfy (PoolItem_Ref item, int other_penalty);

    /**
     *set \c item to \a unneeded */
    bool unneeded (PoolItem_Ref item, int other_penalty);

    /**
     *set \c item to \a incomplete */
    bool incomplete (PoolItem_Ref item, int other_penalty);

    /**
     *upgrade \c from to \c to
     *marks \c from as \a to-be-uninstalled and \c to as \a to-be-installed  */
    bool upgrade (PoolItem_Ref to, PoolItem_Ref from, bool is_soft, int other_penalty);

    /**
     *set \c item to \a to-be-uninstalled */
    bool uninstall (PoolItem_Ref item, bool part_of_upgrade, bool due_to_obsolete, bool due_to_unlink);

    // rough installed/uninstalled test for 'after transaction'

    /**
     *\return \c true if \c item is \a installed or \a to-be-installed */
    bool isPresent (PoolItem_Ref item) const;

    /**
     *\return \c true if \c item is \a uninstalled or \a to-be-uninstalled */
    bool isAbsent (PoolItem_Ref item) const;

    bool requirementIsMet (const Capability & cap, bool is_child = false) const;
    bool requirementIsPossible (const Capability & cap) const;
    bool itemIsPossible (const PoolItem_Ref item) const;
    bool isParallelInstall (const PoolItem_Ref item) const;

    /** iterate over various states */

    void foreachMarked (MarkedPoolItemFn fn, void *data) const;
    PoolItemList getMarked (int which) const;					// <0:uninstalls, 0:all; >0:installs

    int foreachInstall (MarkedPoolItemFn fn, void *data) const;
    PoolItemList getInstalls (void) const;
    int installCount (void) const;

    int foreachUninstall (MarkedPoolItemFn fn, void *data) const;
    PoolItemList getUninstalls (void) const;
    int uninstallCount (void) const;

    int foreachUpgrade (MarkedPoolItemPairFn fn, void *data) const;
    PoolItemList getUpgrades (void) const;
    int upgradeCount (void) const;

    int foreachSatisfy (MarkedPoolItemFn fn, void *data) const;
    PoolItemList getSatisfies (void) const;
    int satisfyCount (void) const;

    int foreachIncomplete (MarkedPoolItemFn fn, void *data) const;
    PoolItemList getIncompletes (void) const;
    int incompleteCount (void) const;

    // add to the report log
    void addInfo (ResolverInfo_Ptr info);					// normal progress info
    void addError (ResolverInfo_Ptr info);					// error progress info

    // iterate over report log
    void foreachInfo (PoolItem_Ref item, int priority, ResolverInfoFn fn, void *data) const;
    ResolverInfoList getInfo (void) const;

    // Context compare to identify equal branches

    int partialCompare (ResolverContext_Ptr context) const;
    int compare (ResolverContext_Ptr context) const;

    // debug
    void spew (void) const;
    void spewInfo (void) const;

};

///////////////////////////////////////////////////////////////////
    };// namespace detail
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
  };// namespace solver
  ///////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
};// namespace zypp
/////////////////////////////////////////////////////////////////////////
#endif // ZYPP_SOLVER_DETAIL_RESOLVERCONTEXT_H

