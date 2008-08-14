/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file zypp/Product.cc
 *
*/
#include <iostream>
#include "zypp/base/LogTools.h"

#include "zypp/Product.h"
#include "zypp/Url.h"

#include "zypp/sat/LookupAttr.h"
#include "zypp/sat/WhatProvides.h"

using std::endl;

///////////////////////////////////////////////////////////////////
namespace zypp
{ /////////////////////////////////////////////////////////////////

  IMPL_PTR_TYPE(Product);

  namespace
  {
    void fillList( std::list<Url> & ret_r, sat::Solvable solv_r, sat::SolvAttr attr_r )
    {
      sat::LookupAttr query( attr_r, solv_r );
      for_( it, query.begin(), query.end() )
      {
        try // ignore malformed urls
        {
          ret_r.push_back( Url( it.asString() ) );
        }
        catch( const url::UrlException & )
        {}
      }
    }

    void fillList( std::list<std::string> & ret_r, sat::Solvable solv_r, sat::SolvAttr attr_r )
    {
      sat::LookupAttr query( attr_r, solv_r );
      for_( it, query.begin(), query.end() )
      {
        ret_r.push_back( it.asString() );
      }
    }
  }

  ///////////////////////////////////////////////////////////////////
  //
  //	METHOD NAME : Product::Product
  //	METHOD TYPE : Ctor
  //
  Product::Product( const sat::Solvable & solvable_r )
  : ResObject( solvable_r )
  {}

  ///////////////////////////////////////////////////////////////////
  //
  //	METHOD NAME : Product::~Product
  //	METHOD TYPE : Dtor
  //
  Product::~Product()
  {}

  ///////////////////////////////////////////////////////////////////
  //
  //	Package interface forwarded to implementation
  //
  ///////////////////////////////////////////////////////////////////

  sat::Solvable Product::referencePackage() const
  {
    Capability identCap( lookupStrAttribute( sat::SolvAttr::productReferences ) );
    if ( ! identCap )
    {
      // No 'references': fallback to provider of 'product(name) = version'
      // Without this solver testcase won't work, as it does not remember
      // 'references'.
      identCap = Capability( str::form( "product(%s) = %s", name().c_str(), edition().c_str() )  );
    }
    if ( ! identCap )
    {
      return sat::Solvable::noSolvable;
    }


    // if there is productReferences defined, we expect
    // a matching package within the same repo.
    sat::WhatProvides providers( identCap );
    for_( it, providers.begin(), providers.end() )
    {
      if ( it->repository() == repository() )
        return *it;
    }

    WAR << *this << ": no reference package found: " << identCap << endl;
    return sat::Solvable::noSolvable;
  }

  std::string Product::shortName() const
  { return lookupStrAttribute( sat::SolvAttr::productShortlabel ); }

  std::string Product::flavor() const
  { return lookupStrAttribute( sat::SolvAttr::productFlavor ); }

  std::string Product::type() const
  { return lookupStrAttribute( sat::SolvAttr::productType ); }

  Url Product::releaseNotesUrl() const
  {
    std::list<Url> ret;
    fillList( ret, satSolvable(), sat::SolvAttr::productRelnotesurl );
    if ( ! ret.empty() )
      return  ret.front();
    return Url();
  }

  std::list<Url> Product::updateUrls() const
  {
    std::list<Url> ret;
    fillList( ret, satSolvable(), sat::SolvAttr::productUpdateurls );
    return ret;
  }

  std::list<Url> Product::extraUrls() const
  {
    std::list<Url> ret;
    fillList( ret, satSolvable(), sat::SolvAttr::productExtraurls );
    return ret;
  }

  std::list<Url> Product::optionalUrls() const
  {
    std::list<Url> ret;
    fillList( ret, satSolvable(), sat::SolvAttr::productOptionalurls );
    return ret;
  }

  std::list<std::string> Product::flags() const
  {
    std::list<std::string> ret;
    fillList( ret, satSolvable(), sat::SolvAttr::productFlags );
    return ret;
  }

  std::string Product::distributionName() const
  { return lookupStrAttribute( sat::SolvAttr::productDistproduct ); }

  Edition Product::distributionEdition() const
  { return Edition( lookupStrAttribute( sat::SolvAttr::productDistversion ) ); }


  /////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////
