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

    File:        MatrixNetHeader.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011

    Description: Header (part of the transported information that contains
                 the structure information of the data proper) for
                 matrices. ie rows, columns

    Credits:

=============================================================================*/

#ifndef CEDAR_MATRIX_NET_HEADER_H
#define CEDAR_MATRIX_NET_HEADER_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/transport/collated/header/MatrixNetHeader.fwd.h"
#include "cedar/auxiliaries/net/detail/transport/collated/header/BasicNetHeader.h"
#include "cedar/auxiliaries/assert.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <vector>

//!@cond SKIPPED_DOCUMENTATION
/*!@brief extend the header struct to matrix data (cols, rows)
 *
 */
struct cedar::aux::net::detail::MatrixNetHeader : BasicNetHeader
{
public:
  int mDims; // number of dimensions
  std::vector<int> mSizes; // for 2d matrices, this will be {rows, cols}
  unsigned int mElementSize;
  bool mInitialized;

  //! Returns the necessary size of a memarray that is used to completely serialize this header
  virtual unsigned int getFixedSerializationSize() const
  {
    return sizeof(int) // mDims
         + sizeof(unsigned int) // mElementSize
         + this->BasicNetHeader::getFixedSerializationSize();
  }

  virtual unsigned int getVariableSerializationSize() const
  {
    return mDims * sizeof(int) // mSizes
           + this->BasicNetHeader::getVariableSerializationSize();
  }

  virtual void serializeFixed(char* memarray)
  {
    char* p = memarray;
    *reinterpret_cast<int*>(p) = this->mDims;
    p += sizeof(int);
    *reinterpret_cast<unsigned int*>(p) = this->mElementSize;
    p += sizeof(unsigned int);
    this->BasicNetHeader::serializeFixed(p);
  }

  virtual void serializeVariable(char* memarray)
  {
    char* p = memarray;
    CEDAR_DEBUG_ASSERT(this->mSizes.size() == static_cast<size_t>(this->mDims));
    for (size_t i = 0; i < this->mSizes.size(); ++i)
    {
      *reinterpret_cast<int*>(p) = this->mSizes.at(i);
      p += sizeof(int);
    }
    this->BasicNetHeader::serializeVariable(p);
  }

  virtual void deserializeFixed(const char* memarray)
  {
    const char* p = memarray;
    this->mDims = *reinterpret_cast<const int*>(p);
    p += sizeof(int);
    this->mElementSize = *reinterpret_cast<const unsigned int*>(p);
    p += sizeof(unsigned int);

    this->BasicNetHeader::deserializeFixed(p);
  }

  virtual void deserializeVariable(const char* memarray)
  {
    const char* p = memarray;
    this->mSizes.resize(static_cast<size_t>(this->mDims));
    for (size_t i = 0; i < this->mSizes.size(); ++i)
    {
      this->mSizes.at(i) = *reinterpret_cast<const int*>(p);
      p += sizeof(int);
    }
    this->BasicNetHeader::deserializeVariable(p);
  }
};
//!@endcond

#endif

