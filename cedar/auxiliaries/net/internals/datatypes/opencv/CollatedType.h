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
    Date:        Wed 20 Jul 2011 05:08:06 PM CEST

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_COLLATEDTYPE_OPENCV_H
#define CEDAR_COLLATEDTYPE_OPENCV_H

// LOCAL INCLUDES
#include "../../namespace.h"
#include "cvMatHelper.h"
#include "../CollatedTraits.h"

// PROJECT INCLUDES
#include <opencv/cv.h>

// SYSTEM INCLUDES




/////////////////// SPECIALIZATIONS of CollatedType


namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {



/////// cv::Mat


template <> 
inline void* CollatedType<cv::Mat>::contentAt(int index, int iElemSize) 
                // specialization for class template
{
  return (void*) &( data.ptr( index % header.rows )[ (index 
                                                     - ( index 
                                                        % header.rows) )
                                                     / header.rows 
                                                     * iElemSize] );

}

template <>
inline cv::Mat CollatedType<cv::Mat>::late_init_data_from_header()
{
  return cv::Mat( header.rows,
                    header.cols, 
                    header.cvMatType );
}



/////// cv::Mat_ (not the underscore!)


template <>
inline void* CollatedType< cv::Mat_<float> >::contentAt(int index, int iElemSize) 
                // specialization for class template
{
  return (void*)  &data( ( index % header.rows ),
                         ( index 
                             - ( index 
                                 % header.rows) )
                         / header.rows 
                         );
}

template <>
inline cv::Mat_<float> CollatedType< cv::Mat_<float> >::late_init_data_from_header()
{
  return cv::Mat_<float>( header.rows,
                          header.cols );
}


} } } } // end namespaces

#endif
