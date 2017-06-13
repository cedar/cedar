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

    File:        cvMatNetHeader.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011

    Description: extend the MatrixNetHeader to hold information abaout
                 the matrix type.
                 We need this, because cv::Mat is vastly configurable
                 at runtime

    Credits:

=============================================================================*/

#ifndef CEDAR_CVMAT_NET_HEADER_H
#define CEDAR_CVMAT_NET_HEADER_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/transport/collated/header/MatrixNetHeader.h"


// PROJECT INCLUDES

// SYSTEM INCLUDES


namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {
//!@cond SKIPPED_DOCUMENTATION

/*!@brief packed header with matrix info for network transfer
 *
 * This is the openCV::Mat specialization
 */
struct cvMatNetHeader : MatrixNetHeader
{
public:
  /*unsigned*/ int mCVMatType; // note: cv::Mat::cvMatType is signed

  virtual unsigned int getFixedSerializationSize() const
  {
    return sizeof(int) // mCVMatType
         + this->MatrixNetHeader::getFixedSerializationSize();
  }

  virtual void serializeFixed(char* memarray)
  {
    char* p = memarray;
    *reinterpret_cast<int*>(p) = this->mCVMatType;
    p += sizeof(int);
    this->MatrixNetHeader::serializeFixed(p);
  }

  virtual void deserializeFixed(const char* memarray)
  {
    const char* p = memarray;
    this->mCVMatType = *reinterpret_cast<const int*>(p);
    p += sizeof(int);
    this->MatrixNetHeader::deserializeFixed(p);
  }
};

//!@endcond
} } } } // end namespaces

#endif

