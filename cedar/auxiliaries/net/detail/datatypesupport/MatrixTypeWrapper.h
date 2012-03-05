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

    File:        MatrixTypeWrapper.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011 04:32:21 PM CEST

    Description: MatrixTypeWrapper provides template specializations to access
                 the transferred type (which will be a 'collated type'
                 == matrix header + matrix body)

    Credits:

=============================================================================*/

#ifndef CEDAR_COLLATEDTYPE_H
#define CEDAR_COLLATEDTYPE_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/namespace.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/CollatedTraits.h"

// PROJECT INCLUDES
#include <boost/static_assert.hpp>

// SYSTEM INCLUDES




namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {


/*!@brief templated wrapper of the to be transported data type. SPECIALIZE!
 *
 * This template needs to be specialized!
 *
 * note that for once the member can be accessed directly (publicly)
 */
template <typename T>
struct MatrixTypeWrapper
{
  //---------------------------------------------------------------------------
  // members and typedefs
  //---------------------------------------------------------------------------
public:
  typedef CollatedTraits<T>                 TraitsType;
  typedef typename TraitsType::DataType     DataType;
  typedef typename TraitsType::HeaderType   HeaderType;

  HeaderType mHeader; // yes, it is public. this type is about being accessed
  DataType   mData;   // yes, it is public.                    ... externally

  //---------------------------------------------------------------------------
  // constructors and destructor
  //---------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  MatrixTypeWrapper() 
    : mHeader(), mData()
  {
    // important: the size of the matrix will only be obtained later on
    //            - at runtime (think of cv::Mat matrices)
  }

  //!@brief Destructor
  ~MatrixTypeWrapper()
  {
  }

  // inline as it will be called in inner loop
  inline void* contentAt(int index, int iElemSize)
  {
    BOOST_STATIC_ASSERT(sizeof(T) == 0); 
    // this will not compile, need to specialize
    return NULL;
  }

  inline T lateInitDataFromHeader()
  {
    BOOST_STATIC_ASSERT(sizeof(T) == 0); 
    // this will not compile, need to specialize
    return 0;
  }

};


} } } } // end namespaces


// Spezialisierungen: ...
#include "cedar/auxiliaries/net/detail/datatypesupport/opencv/MatrixTypeWrapper.h"

#endif
