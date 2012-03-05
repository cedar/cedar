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
    Date:        Wed 20 Jul 2011 05:08:06 PM CEST

    Description: openCV specialization for MatrixTypeWrapper.
                 This tells us how to acces an openCV-type

    Credits:

=============================================================================*/

#ifndef CEDAR_COLLATEDTYPE_OPENCV_H
#define CEDAR_COLLATEDTYPE_OPENCV_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/namespace.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/opencv/cvMatHelper.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/CollatedTraits.h"

// PROJECT INCLUDES
#include <opencv2/opencv.hpp>

// SYSTEM INCLUDES




/////////////////// SPECIALIZATIONS of MatrixTypeWrapper


namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {



/////// cv::Mat


template <> 
inline void* MatrixTypeWrapper<cv::Mat>::contentAt(int index, int elemSize) 
                // specialization for class template
{
  return (void*) &( mData.ptr( index % mHeader.mRows )[ (index 
                                                     - ( index 
                                                        % mHeader.mRows) )
                                                     / mHeader.mRows 
                                                     * elemSize] );

}

template <>
inline cv::Mat MatrixTypeWrapper<cv::Mat>::lateInitDataFromHeader()
{
  return cv::Mat( mHeader.mRows,
                    mHeader.mColumns, 
                    mHeader.mCVMatType );
}



/////// cv::Mat_ (note the underscore!)


template <>
inline void* MatrixTypeWrapper< cv::Mat_<float> >::contentAt(int index, int) 
                // specialization for class template
{
  return (void*)  &mData( ( index % mHeader.mRows ),
                         ( index 
                             - ( index 
                                 % mHeader.mRows) )
                         / mHeader.mRows 
                         );
}

template <>
inline cv::Mat_<float> MatrixTypeWrapper< cv::Mat_<float> >::lateInitDataFromHeader()
{
  return cv::Mat_<float>( mHeader.mRows,
                          mHeader.mColumns );
}


} } } } // end namespaces

#endif
