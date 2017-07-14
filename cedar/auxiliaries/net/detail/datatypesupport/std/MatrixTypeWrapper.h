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

    File:        MatrixTypeWrapper.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011

    Description: std-namespace specialization for MatrixTypeWrapper.

    Credits:

=============================================================================*/

#ifndef CEDAR_COLLATED_TYPE_STD_H
#define CEDAR_COLLATED_TYPE_STD_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/datatypesupport/CollatedTraits.h"

// SYSTEM INCLUDES
#include <string>



/////////////////// SPECIALIZATIONS of MatrixTypeWrapper

namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {

//!@cond SKIPPED_DOCUMENTATION


/////// std:std::string


template <>
inline std::string MatrixTypeWrapper<std::string>::lateInitDataFromHeader()
{
  return std::string();
}

template <>
inline unsigned int MatrixTypeWrapper<std::string>::getDataSize() const
{
  return mHeader.mDataSize;
}


template <>
inline void MatrixTypeWrapper<std::string>::writeToMemory(char *p_vals)
{
  fixedWriteToMemory(p_vals);
}

template <>
inline void MatrixTypeWrapper<std::string>::readFromMemory(const char *p_vals)
{
  mData = std::string(p_vals);
}

template <>
inline void* MatrixTypeWrapper<std::string>::contentAll() const
{
  return (void*) mData.c_str();
}


//!@endcond

      } } } } // end namespaces

#endif
