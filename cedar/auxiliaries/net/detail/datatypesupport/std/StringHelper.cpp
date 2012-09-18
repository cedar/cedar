/*=============================================================================

    Copyright 2011, 2012 Institut fuer Neuroinforsik, Ruhr-Universitaet Bochum, Germany
 
    This file is part of cedar.

    cedar is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    cedar is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with cedar. If not, see <http://www.gnu.org/licenses/>.

===============================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinforsik

    File:        StringHelper.cpp

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Fri 04 May 2012

    Description: Helper class for std::string

    Credits:

=============================================================================*/

#include "cedar/configuration.h"
#ifdef CEDAR_USE_YARP

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/datatypesupport/std/StringHelper.h"

#include "cedar/auxiliaries/net/detail/datatypesupport/interfaces/InterfaceCollatedData.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/opencv/cvMatNetHeader.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// TODO: make constant
#define CONST_MAGIC_NUMBER (0xabc)

namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {
//!@cond SKIPPED_DOCUMENTATION

// save local copy of the header inforsion. (will be compared later on with
// user input)
template <typename CVT> void StringHelper<CVT>::initLocalHeader(
                                                 const HeaderType &header)
{
  // initially fill the local header with inforsion 
  mLocalHeader.mMagicNumber= CONST_MAGIC_NUMBER;
                                // note: this is set here, not copied from
                                //       the external header
  mLocalHeader.mDataSize= header.mDataSize;
}

// init the *external* header from the srix, that will be sent over the line
template <typename CVT> void StringHelper<CVT>::initExternalHeader(
                                                   const CVT &s,
                                                   HeaderType &extheader)
{
  extheader.mDataSize= s.size() + 1; // dont forget the trailig \0!
  extheader.mMagicNumber= CONST_MAGIC_NUMBER;
}

// check the data before we write it over the network.
// mainly we have to compare the header-info - and maybe initialize it
// param: extheader may be changed (initialized)!
template <typename CVT> bool StringHelper<CVT>::checkCollatedDataForWrite(
                                               const CVT &s,
                                               HeaderType &extheader)
{
  // TODO: safer: have a boolean in header to mark initialization status

  // first run: Initialize
  if (!mLocalHeader.mDataSize)
  {
    // init the header data (from s) that will be sent over the line
    initExternalHeader(s, extheader); // get info from mat!

    // init the local copy of the header that we will save to compare
    // with later user inputs
    initLocalHeader(extheader);
    return true;
  }
  // all later runs: Compare (with locally saved localheader to check against)
  else
  {
    {
      // init the header data (from s) that will be sent over the line

      // we need this for different s-variables that are passed
      // to the same writer
      initExternalHeader(s, extheader);

      return true;
    }
  }
}

template <typename CVT> bool StringHelper<CVT>::checkCollatedDataForRead(
                                                 const HeaderType &extheader)
{
  if (!mLocalHeader.mDataSize)
  {
    // first run: initialize!
    initLocalHeader(extheader); // get info from header!

    // consistency check for the first run
    if (mLocalHeader.mMagicNumber != extheader.mMagicNumber)
    {                // ^--- was just set in initLocalHeader()
                     //      (and not copied from the extheader)
      return false;
    }
 
    return true;
  }
  else
  {
    // all later runs: just check for consistency
    if (mLocalHeader.mMagicNumber != extheader.mMagicNumber 
      )
    {
      // exceptions will not be thrown here
      return false;
    }

    return true;
  }
}

template <typename CVT> StringHelper<CVT>::StringHelper() 
  : mLocalHeader()
{
#ifdef DEBUG_NETT
//  cout << "  StringHelper [CONSTRUCTOR]" << endl;
#endif
    mLocalHeader.mDataSize= 0;
}

template <typename CVT> StringHelper<CVT>::~StringHelper()
{
#ifdef DEBUG_NETT
//  cout << "  ~StringHelper [DESTRUCTOR]" << endl;
#endif
}


// explicit instantiation DO NOT FORGET THIS!
template struct cedar::aux::net::detail::StringHelper<std::string>;


//!@endcond
} } } } // end namespaces
#endif

