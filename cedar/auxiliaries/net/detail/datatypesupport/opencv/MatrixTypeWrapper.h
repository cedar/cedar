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

    Description: openCV specialization for MatrixTypeWrapper.
                 This tells us how to acces an openCV-type

    Credits:

=============================================================================*/

#ifndef CEDAR_COLLATED_TYPE_OPENCV_H
#define CEDAR_COLLATED_TYPE_OPENCV_H

// LOCAL INCLUDES
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
//!@cond SKIPPED_DOCUMENTATION



/////// cv::Mat


template <> 
inline void* MatrixTypeWrapper<cv::Mat>::contentAt(int index, int elemSize) const
                // specialization for class template
{
//  return (void*) &( mData.ptr( index % mHeader.mRows )[ (index
//                                                     - ( index
//                                                        % mHeader.mRows) )
//                                                     / mHeader.mRows
//                                                     * elemSize] );
  //!@todo Does this work?
  return (void*) &(mData.data[index * elemSize]);
}

template <>
inline cv::Mat MatrixTypeWrapper<cv::Mat>::lateInitDataFromHeader()
{
//  return cv::Mat( mHeader.mRows,
//                    mHeader.mColumns,
//                    mHeader.mCVMatType, 0.0);
  if (mHeader.mDims < 2)
  {
    return cv::Mat( mHeader.mSizes[0],
                    mHeader.mSizes[1],
                    mHeader.mCVMatType, 0.0);
  }
  else
  {
    return cv::Mat( mHeader.mDims,
                    &mHeader.mSizes.front(),
                    mHeader.mCVMatType, 0.0);
  }
}

template <>
inline unsigned int MatrixTypeWrapper<cv::Mat>::getElementCount() const
{
  unsigned int size = 1;
  for (size_t i = 0; i < mHeader.mSizes.size(); ++i)
  {
    size *= static_cast<unsigned int>(mHeader.mSizes.at(i));
  }
  return size;
}

template <>
inline unsigned int MatrixTypeWrapper<cv::Mat>::getDataSize() const
{
  return mHeader.mElementSize * this->getElementCount();
}

template <>
inline void MatrixTypeWrapper<cv::Mat>::writeToMemory(char *p_vals)
{
  defaultWriteToMemory(p_vals);
}

template <>
inline void MatrixTypeWrapper<cv::Mat>::readFromMemory(const char *p_vals)
{
  defaultReadFromMemory(p_vals);
}


/////// cv::Mat_ (note the underscore!)


template <>
inline void* MatrixTypeWrapper< cv::Mat_<float> >::contentAt(int index, int) const
                // specialization for class template
{
//  return (void*)  &mData( ( index % mHeader.mRows ),
//                         ( index
//                             - ( index
//                                 % mHeader.mRows) )
//                         / mHeader.mRows
//                         );
  return (void*) &mData.data[index * mHeader.mElementSize];
}

template <>
inline cv::Mat_<float> MatrixTypeWrapper< cv::Mat_<float> >::lateInitDataFromHeader()
{
//  return cv::Mat_<float>( mHeader.mRows,
//                          mHeader.mColumns );
  if (mHeader.mDims < 2)
  {
    return cv::Mat_<float>(mHeader.mSizes[0],
                    mHeader.mSizes[1],
                    0.0);
  }
  else
  {
    return cv::Mat_<float>(mHeader.mDims,
                    &mHeader.mSizes.front(),
                    0.0);
  }
}


template <>
inline unsigned int MatrixTypeWrapper< cv::Mat_<float> >::getElementCount() const
{
  unsigned int size = 1;
  for (size_t i = 0; i < mHeader.mSizes.size(); ++i)
  {
    size *= static_cast<unsigned int>(mHeader.mSizes.at(i));
  }
  return size;
}

template <>
inline unsigned int MatrixTypeWrapper< cv::Mat_<float> >::getDataSize() const
{
  return mHeader.mElementSize * this->getElementCount();
}



//!@endcond
} } } } // end namespaces

#endif
