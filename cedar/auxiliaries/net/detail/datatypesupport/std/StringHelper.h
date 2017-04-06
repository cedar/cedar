/*=============================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
                 Institut fuer Neuroinformatik

    File:        StringHelper.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011

    Description: Helper class for std::string handling

    Credits:

=============================================================================*/

#ifndef CEDAR_STD_STRING_HELPER_H
#define CEDAR_STD_STRING_HELPER_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/datatypesupport/std/StringHelper.fwd.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/CollatedTraits.h"
#include "cedar/auxiliaries/net/detail/transport/collated/header/BasicNetHeader.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

//!@cond SKIPPED_DOCUMENTATION
// TODO: make constant
#define CONST_MAGIC_NUMBER (0xabc)


//!@brief a helper class which implements InterfacCollatedData for opencv types
template <typename CVT>
class cedar::aux::net::detail::StringHelper
{

  //---------------------------------------------------------------------------
  // members and typedefs
  //---------------------------------------------------------------------------
public:
  typedef BasicNetHeader HeaderType;

private:
  HeaderType mLocalHeader; // local info about matrix-header, to compare
                           // with future user inputs

  // save local copy of the header inforsion. (will be compared later on with
  // user input)
  //!@brief prepare (init) the local copy of the header for comparisons
  void initLocalHeader( const HeaderType &header)
  {
    // initially fill the local header with inforsion 
    mLocalHeader.mMagicNumber= CONST_MAGIC_NUMBER;
                                  // note: this is set here, not copied from
                                  //       the external header
    mLocalHeader.mDataSize= header.mDataSize;
  }

  // init the *external* header from the srix, that will be sent over the line
  //!@brief prepare (init) the external data, which will be sent
  void initExternalHeader( const CVT &s, HeaderType &extheader)
  {
    extheader.mDataSize= s.size() + 1; // dont forget the trailig \0!
    extheader.mMagicNumber= CONST_MAGIC_NUMBER;
  }


  //---------------------------------------------------------------------------
  // protected methods
  //---------------------------------------------------------------------------
protected:

  // check the data before we write it over the network.
  // mainly we have to compare the header-info - and maybe initialize it
  // param: extheader may be changed (initialized)!
  //!@brief check the data before writing it (opencv implementation)
  bool checkCollatedDataForWrite(
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

  //!@brief check the data before reading it (opencv implementation)
  bool checkCollatedDataForRead(const HeaderType &extheader)
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

  //---------------------------------------------------------------------------
  // constructors and destructor
  //---------------------------------------------------------------------------
public:

  //!@brief The standard constructor.
  StringHelper() 
    : mLocalHeader()
  {
#ifdef DEBUG_NETT
  //  cout << "  StringHelper [CONSTRUCTOR]" << endl;
#endif
      mLocalHeader.mDataSize= 0;
  }

  //!@brief Destructor
  virtual ~StringHelper()
  {
#ifdef DEBUG_NETT
  //  cout << "  ~StringHelper [DESTRUCTOR]" << endl;
#endif
  }

};

//!@endcond

#endif
