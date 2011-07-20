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

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {


template <typename CVT> void cvMatHelper<CVT>::init_checkheader(HeaderType &header)
{
  // einmalig Header-Infos fuellen 
  mCheckHeader.magicNumber= yarp::os::NetInt32(MAGIC_NUMBER);
  mCheckHeader.rows= header.rows;
  mCheckHeader.cols= header.cols;
  mCheckHeader.elemSize= header.elemSize;
  mCheckHeader.cvMatType= header.cvMatType;
}

template <typename CVT> void cvMatHelper<CVT>::cvMatHelper::init_externalheader(const CVT &mat,
                                                   HeaderType &header)
{
  header.cols= mat.cols;
  header.rows= mat.rows;
  header.elemSize= mat.elemSize();
  header.cvMatType= mat.type();
}

template <typename CVT> bool cvMatHelper<CVT>::check_collateddata_for_write(
                                               const CVT &mat,
                                               HeaderType &extheader)
{
    if (!mCheckHeader.cols || !mCheckHeader.rows)
    {
      init_externalheader(mat, extheader); // get info from mat!

      init_checkheader(extheader);
      return true;
    }
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
        init_externalheader(mat, extheader);
        return true;
      }
    }
}

template <typename CVT> bool cvMatHelper<CVT>::check_collateddata_for_read(HeaderType &extheader)
{
  if (!mCheckHeader.cols || !mCheckHeader.rows)
  {
    init_checkheader(extheader); // get info from header!

    if (mCheckHeader.magicNumber != extheader.magicNumber)
    {                // ^--- was just set in init_checkheader()
      return false;
    }
 
    return true;
  }
  else
  {
    if (mCheckHeader.cols != extheader.cols
        || mCheckHeader.rows != extheader.rows
        || mCheckHeader.elemSize != extheader.elemSize
        || mCheckHeader.cvMatType != extheader.cvMatType 
        || mCheckHeader.magicNumber != extheader.magicNumber 
      )
    {
      // exceptions will not be thrown here
      return false;
    }

    return true;
  }
}

template <typename CVT> cvMatHelper<CVT>::cvMatHelper() : mCheckHeader()
{
#ifdef DEBUG
//  cout << "  cvMatHelper [CONSTRUCTOR]" << endl;
#endif
    mCheckHeader.rows= 0;
    mCheckHeader.cols= 0;
}

template <typename CVT> cvMatHelper<CVT>::~cvMatHelper()
{
#ifdef DEBUG
//  cout << "  ~cvMatHelper [DESTRUCTOR]" << endl;
#endif
}


// explicit instantiation DO NOT FORGET THIS!
template struct _NM_FULL_::cvMatHelper<cv::Mat>;
template struct _NM_FULL_::cvMatHelper< cv::Mat_<float> >;


} } } } // end namespaces

