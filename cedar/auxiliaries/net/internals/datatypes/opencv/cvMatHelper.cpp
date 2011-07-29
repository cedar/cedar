/*=============================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
    Date:        Wed 20 Jul 2011 05:15:29 PM CEST

    Description:

    Credits:

=============================================================================*/

// LOCAL INCLUDES
#include "cvMatHelper.h"

#include "../interfaces/InterfaceCollatedData.h"
#include "cvMatNetHeader.h"
#include "../../transport/collated/header/MatrixNetHeaderAccessor.h"

// PROJECT INCLUDES
#include <opencv/cv.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/NetFloat64.h>

// SYSTEM INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <iostream>


#define MAGIC_NUMBER 0xabc

namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {

// save local copy of the header information. (will be compared later on with
// user input)
template <typename CVT> void cvMatHelper<CVT>::init_checkheader(
                                                 const HeaderType &header)
{
#define CONST_MAGIC_NUMBER yarp::os::NetInt32(MAGIC_NUMBER)

  // einmalig Header-Infos fuellen 
  mCheckHeader.magicNumber= CONST_MAGIC_NUMBER;
                                // note: this is set here, not copied from
                                //       the external header
  mCheckHeader.rows= header.rows;
  mCheckHeader.cols= header.cols;
  mCheckHeader.elemSize= header.elemSize;
  mCheckHeader.cvMatType= header.cvMatType;
}

// init the *external* header from the matrix, that will be sent over the line
template <typename CVT> void cvMatHelper<CVT>::cvMatHelper::init_externalheader(
                                                   const CVT &mat,
                                                   HeaderType &header)
{
  header.cols= mat.cols;
  header.rows= mat.rows;
  header.elemSize= mat.elemSize();
  header.cvMatType= mat.type();
  header.magicNumber= CONST_MAGIC_NUMBER;
}

// check the data before we write it over the network.
// mainly we have to compare the header-info - and maybe initialize it
// param: extheader may be changed (initialized)!
template <typename CVT> bool cvMatHelper<CVT>::check_collateddata_for_write(
                                               const CVT &mat,
                                               HeaderType &extheader)
{
  // first run: Initialize
  if (!mCheckHeader.cols || !mCheckHeader.rows)
  {
    // init the header data (from mat) that will be sent over the line
    init_externalheader(mat, extheader); // get info from mat!

    // init the local copy of the header that we will save to compare
    // with later user inputs
    init_checkheader(extheader);
    return true;
  }
  // all later runs: Compare (with locally save checkheader)
  else
  {
    if (mCheckHeader.cols != mat.cols
        || mCheckHeader.rows != mat.rows
        || mCheckHeader.elemSize != mat.elemSize() 
        || mCheckHeader.cvMatType != mat.type() )
    {

      return false; // exceptions will not be thrown here
    }
    else
    {
      // init the header data (from mat) that will be sent over the line

      // we need this for different mat-variables that are passed
      // to the same writer
      init_externalheader(mat, extheader);

      return true;
    }
  }
}

template <typename CVT> bool cvMatHelper<CVT>::check_collateddata_for_read(
                                                 const HeaderType &extheader)
{
  if (!mCheckHeader.cols || !mCheckHeader.rows)
  {
//std::cout << "first run"  << std::endl;    
    // first run: initialize!
    init_checkheader(extheader); // get info from header!

    // consistency check for the first run
    if (mCheckHeader.magicNumber != extheader.magicNumber)
    {                // ^--- was just set in init_checkheader()
                     //      (and not copied from the extheader)
//std::cout << "first run magic number failed "  
//          <<  mCheckHeader.magicNumber << " and "  
//          << extheader.magicNumber <<  std::endl;    
      return false;
    }
 
//std::cout << "ALL OK"  << std::endl;    
    return true;
  }
  else
  {
//std::cout << "other run: "  << std::endl;    
    // all later runs: just check for consitency
    if (mCheckHeader.cols != extheader.cols
        || mCheckHeader.rows != extheader.rows
        || mCheckHeader.elemSize != extheader.elemSize
        || mCheckHeader.cvMatType != extheader.cvMatType 
        || mCheckHeader.magicNumber != extheader.magicNumber 
      )
    {
//std::cout << "other run: some check failed "  << std::endl;    
      // exceptions will not be thrown here
      return false;
    }

//std::cout << "ALL OK"  << std::endl;    
    return true;
  }
}

template <typename CVT> cvMatHelper<CVT>::cvMatHelper() : mCheckHeader()
{
#ifdef DEBUG_NETT
//  cout << "  cvMatHelper [CONSTRUCTOR]" << endl;
#endif
    mCheckHeader.rows= 0;
    mCheckHeader.cols= 0;
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


} } } } // end namespaces

