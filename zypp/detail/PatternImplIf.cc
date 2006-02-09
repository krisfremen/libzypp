/*---------------------------------------------------------------------\
|                          ____ _   __ __ ___                          |
|                         |__  / \ / / . \ . \                         |
|                           / / \ V /|  _/  _/                         |
|                          / /__ | | | | | |                           |
|                         /_____||_| |_| |_|                           |
|                                                                      |
\---------------------------------------------------------------------*/
/** \file	zypp/detail/PatternImplIf.cc
 *
*/

#include "zypp/detail/PatternImplIf.h"

///////////////////////////////////////////////////////////////////
namespace zypp
{ /////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////
  namespace detail
  { /////////////////////////////////////////////////////////////////

    std::string PatternImplIf::patternId() const
    { return ""; }

    bool PatternImplIf::isDefault() const
    { return false; }

    bool PatternImplIf::userVisible() const
    { return true; }

    std::string PatternImplIf::category() const
    { return std::string(); }

    Pathname PatternImplIf::icon() const
    { return Pathname(); }

    Pathname PatternImplIf::script() const
    { return Pathname(); }

    /////////////////////////////////////////////////////////////////
  } // namespace detail
  ///////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
} // namespace zypp
///////////////////////////////////////////////////////////////////
