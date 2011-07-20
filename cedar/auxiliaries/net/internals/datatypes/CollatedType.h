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

    File:        CollatedType.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011 04:32:21 PM CEST

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_COLLATEDTYPE_H
#define CEDAR_COLLATEDTYPE_H

// LOCAL INCLUDES
#include "../namespace.h"
#include "CollatedTraits.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES




namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {


/*!@brief templated wrapper of the to be transported data type. SPECIALIZE!
 *
 * This template needs to be specialized!
 *
 * note that for once the member can be accessed directly (publicly)
 */
template <typename T>
struct CollatedType
{
  //---------------------------------------------------------------------------
  // members and typedefs
  //---------------------------------------------------------------------------
public:
  typedef collated_traits<T>                traits_type;
  typedef typename traits_type::data_type   data_type;
  typedef typename traits_type::header_type header_type;

  header_type header; // yes, it is public. this type is about being accessed
  data_type   data;   // yes, it is public.                    ... externally

  //---------------------------------------------------------------------------
  // constructors and destructor
  //---------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  CollatedType() : header(), data()
  {
    // important: the size of the matrix will only be obtained later on
    //            - at runtime (think of cv::Mat matrices)
  }

  //!@brief Destructor
  ~CollatedType()
  {
  }

  // inline as it will be called in inner loop
  inline void* contentAt(int index, int iElemSize)
  {
    // this should not compile
    return NULL;
  }

  inline T late_init_data_from_header()
  {
    return 0;
  }

};


} } } } // end namespaces


// Spezialisierungen: ...
#include "opencv/CollatedType.h"

#endif
