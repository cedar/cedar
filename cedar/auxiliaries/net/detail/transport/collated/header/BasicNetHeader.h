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

    File:        BasicNetHeader.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011

    Description: this doesnt do much. Even before we think about 
                 what a matrix header contains, we want to have some
                 primitive way to check for corruption.

    Credits:

=============================================================================*/

#ifndef CEDAR_COLLATED_NET_HEADER_H
#define CEDAR_COLLATED_NET_HEADER_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/transport/collated/header/BasicNetHeader.fwd.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

//!@cond SKIPPED_DOCUMENTATION
/*!@brief net portable struct that will hold the header (of a matrix)
 *
 * This will be inherited and extended for for example a header for
 * cv-matrizes.
 *
 * These structures are small by default, i.e. dont hold accessor
 * functions. For accessing the data, 
 * see the static class MatrixNetHeaderAccessor
 */
struct cedar::aux::net::detail::BasicNetHeader
{
  unsigned int mMagicNumber;
  size_t       mDataSize; // this is highly-architecture dependant!

  virtual unsigned int getFixedSerializationSize() const
  {
    return sizeof(unsigned int) // mMagicNumber
        + sizeof(size_t); // mDataSize
  }

  virtual unsigned int getVariableSerializationSize() const
  {
    return 0;
  }

  virtual void serializeFixed(char* memarray)
  {
    char* p = memarray;
    *reinterpret_cast<unsigned int*>(p) = this->mMagicNumber;
    p += sizeof(unsigned int);
    *reinterpret_cast<size_t*>(p) = this->mDataSize;
  }

  virtual void serializeVariable(char*)
  {
  }

  virtual void deserializeFixed(const char* memarray)
  {
    const char* p = memarray;
    this->mMagicNumber = *reinterpret_cast<const unsigned int*>(p);
    p += sizeof(unsigned int);
    this->mDataSize = *reinterpret_cast<const size_t*>(p);
  }

  virtual void deserializeVariable(const char*)
  {
  }
};
//!@endcond

#endif
