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

    Description: MatrixTypeWrapper provides template specializations to access
                 the transferred type (which will be a 'collated type'
                 == matrix header + matrix body).
                 
                 Note: This class will not be transferred.

    Credits:

=============================================================================*/

#ifndef CEDAR_COLLATED_TYPE_H
#define CEDAR_COLLATED_TYPE_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/datatypesupport/MatrixTypeWrapper.fwd.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/CollatedTraits.h"

// PROJECT INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/static_assert.hpp>
#endif

// SYSTEM INCLUDES




//!@cond SKIPPED_DOCUMENTATION

/*!@brief templated wrapper of the to be transported data type. SPECIALIZE!
 *
 * This template needs to be specialized!
 *
 * note that for once the member can be accessed directly (publicly)
 */
template <typename T>
struct cedar::aux::net::detail::MatrixTypeWrapper
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

  T lateInitDataFromHeader()
  {
    BOOST_STATIC_ASSERT(sizeof(T) == 0); 
    // this will not compile, need to specialize
    return 0;
  }

  unsigned int getDataSize() const
  {
    BOOST_STATIC_ASSERT(sizeof(T) == 0); 
    return 0;
  }

  unsigned int getElementCount() const
  {
    BOOST_STATIC_ASSERT(sizeof(T) == 0); 
    return 0;
  }

  unsigned int getElementSize()
  {
    return mHeader.mElementSize;
  }

  void* contentAt(int index, int iElemSize) const
  {
    BOOST_STATIC_ASSERT(sizeof(T) == 0); 
    // this will not compile, need to specialize
    return NULL;
  }

  void* contentAll() const
  {
    BOOST_STATIC_ASSERT(sizeof(T) == 0); 
    // this will not compile, need to specialize
    return NULL;
  }

  void writeToMemory(char *p_vals)
  {
    defaultWriteToMemory(p_vals);
  }


  void readFromMemory(const char *p_vals)
  {
    defaultReadFromMemory(p_vals);
  }

private:  
  void defaultReadFromMemory(const char *p_vals)
  {
    int num_elements, element_size, i;

    num_elements= getElementCount();
    element_size= getElementSize();

    for (i= 0; i< num_elements; i++)
    {
      // we use memcpy as we only know the sizes of the data at runtime
      memcpy( contentAt(i, element_size),
              p_vals + (i * element_size),
              element_size );
    }
  }

  void defaultWriteToMemory(char *p_vals)
  {
    int num_elements, element_size;
    int i;

    num_elements= getElementCount();
    element_size= getElementSize();

    for (i= 0; i < num_elements; i++)
    {
      // copy each element.
      // we access it with contentAt() which will be specialized for each
      // transportet data type
      memcpy( p_vals + (i * element_size),
              contentAt(i, element_size),
              element_size );
    } 
  }

  void fixedReadFromMemory(const char *p_vals)
  {
    size_t data_size;

    data_size = getDataSize();
    memcpy( contentAll(),
            p_vals,
            data_size );
  }

  void fixedWriteToMemory(char *p_vals)
  {
    size_t data_size;

    data_size = getDataSize();
    memcpy( p_vals,
            contentAll(),
            data_size );

  }
};

//!@endcond


// Spezialisierungen: ...
#include "cedar/auxiliaries/net/detail/datatypesupport/opencv/MatrixTypeWrapper.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/std/MatrixTypeWrapper.h"

#endif
