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

    File:        cvMatHelper.cpp

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011

    Description: Helper class for openCV matrices. Implements some checks 

    Credits:

=============================================================================*/

#include "cedar/configuration.h"
#ifdef CEDAR_USE_YARP

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/datatypesupport/opencv/cvMatHelper.h"

#include "cedar/auxiliaries/net/detail/datatypesupport/opencv/cvMatNetHeader.h"
#include "cedar/auxiliaries/net/exceptions.h"
#include "cedar/auxiliaries/assert.h"

// PROJECT INCLUDES
#include <opencv2/opencv.hpp>

// SYSTEM INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// TODO: make constant
#define CONST_MAGIC_NUMBER (0xdef)

namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {
//!@cond SKIPPED_DOCUMENTATION

// save local copy of the header information. (will be compared later on with
// user input)
template <typename CVT> void cvMatHelper<CVT>::initLocalHeader(
                                                 const HeaderType &header)
{
  // initially fill the local header with information 
  mLocalHeader.mMagicNumber = CONST_MAGIC_NUMBER;
                                // note: this is set here, not copied from
                                //       the external header
  mLocalHeader.mDims = header.mDims;
  mLocalHeader.mSizes = header.mSizes;
  mLocalHeader.mElementSize= header.mElementSize;
  mLocalHeader.mCVMatType= header.mCVMatType;
}

// init the *external* header from the matrix, that will be sent over the line
template <typename CVT> void cvMatHelper<CVT>::initExternalHeader(
                                                   const CVT &mat,
                                                   HeaderType &extheader)
{
  extheader.mDims= mat.dims;
  extheader.mSizes.clear();
  extheader.mSizes.reserve(static_cast<size_t>(mat.dims));
  if (mat.dims == 2)
  {
    extheader.mSizes.push_back(mat.rows);
    extheader.mSizes.push_back(mat.cols);
  }
  else
  {
    for (int i = 0; i < mat.dims; ++i)
    {
      extheader.mSizes.push_back(mat.size[i]);
    }
  }
  extheader.mElementSize= mat.elemSize();
  extheader.mCVMatType= mat.type();
  extheader.mMagicNumber= CONST_MAGIC_NUMBER;
}

// check the data before we write it over the network.
// mainly we have to compare the header-info - and maybe initialize it
// param: extheader may be changed (initialized)!
template <typename CVT> bool cvMatHelper<CVT>::checkCollatedDataForWrite(
                                               const CVT &mat,
                                               HeaderType &extheader)
{
  // first run: Initialize
  if (!mLocalHeader.mInitialized)
  {
    // init the header data (from mat) that will be sent over the line
    initExternalHeader(mat, extheader); // get info from mat!

    // init the local copy of the header that we will save to compare
    // with later user inputs
    initLocalHeader(extheader);
    return true;
  }
  // all later runs: Compare (with locally saved localheader to check against)
  else
  {
    if
    (
      mLocalHeader.mSizes.size() != static_cast<size_t>(mat.dims)
      || mLocalHeader.mElementSize != mat.elemSize()
      || mLocalHeader.mCVMatType != mat.type()
    )
    {
      return false;
    }
    for (size_t i = 0; i < mLocalHeader.mSizes.size(); ++i)
    {
      if (mLocalHeader.mSizes.at(i) != mat.size[i])
      {
        return false;
      }
    }

    // init the header data (from mat) that will be sent over the line

    // we need this for different mat-variables that are passed
    // to the same writer
    initExternalHeader(mat, extheader);

    return true;
  }
}

template <typename CVT> bool cvMatHelper<CVT>::checkCollatedDataForRead(
                                                 const HeaderType &extheader)
{
  if (!mLocalHeader.mInitialized)
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
    if (mLocalHeader.mSizes.size() != extheader.mSizes.size()
        || mLocalHeader.mDims != extheader.mDims
        || mLocalHeader.mElementSize != extheader.mElementSize
        || mLocalHeader.mCVMatType != extheader.mCVMatType 
        || mLocalHeader.mMagicNumber != extheader.mMagicNumber 
      )
    {
      // exceptions will not be thrown here
      return false;
    }

    for (size_t i = 0; i < this->mLocalHeader.mSizes.size(); ++i)
    {
      if (mLocalHeader.mSizes.at(i) != extheader.mSizes.at(i))
      {
        return false;
      }
    }

    return true;
  }
}

template <typename CVT> cvMatHelper<CVT>::cvMatHelper() 
  : mLocalHeader()
{
#ifdef DEBUG_NETT
//  cout << "  cvMatHelper [CONSTRUCTOR]" << endl;
#endif
  mLocalHeader.mInitialized = false;
}

template <typename CVT> cvMatHelper<CVT>::~cvMatHelper()
{
#ifdef DEBUG_NETT
//  cout << "  ~cvMatHelper [DESTRUCTOR]" << endl;
#endif
}


// explicit instantiation DO NOT FORGET THIS!
template struct cedar::aux::net::detail::cvMatHelper<cv::Mat>;
template struct cedar::aux::net::detail::cvMatHelper< cv::Mat_<float> >;

//!@endcond
} } } } // end namespaces
#endif

