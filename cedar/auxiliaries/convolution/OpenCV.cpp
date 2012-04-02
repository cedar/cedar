/*======================================================================================================================

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

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        OpenCV.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 14

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/convolution/OpenCV.h"
#include "cedar/auxiliaries/convolution/BorderType.h"
#include "cedar/auxiliaries/convolution/Mode.h"
#include "cedar/auxiliaries/kernel/Separable.h"
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register type with the factory
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered
    = cedar::aux::conv::EngineManagerSingleton::getInstance()->registerType<cedar::aux::conv::OpenCVPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::conv::OpenCV::OpenCV()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

int cedar::aux::conv::OpenCV::translateBorderType(cedar::aux::conv::BorderType::Id borderType) const
{
  switch (borderType)
  {
    case cedar::aux::conv::BorderType::Cyclic:
      return cv::BORDER_WRAP;

    case cedar::aux::conv::BorderType::Reflect:
      return cv::BORDER_REFLECT;

    case cedar::aux::conv::BorderType::Replicate:
      return cv::BORDER_REPLICATE;

    case cedar::aux::conv::BorderType::Zero:
      //!@todo This actually makes the border -1 rather than 0!
      return cv::BORDER_CONSTANT;

    default:
      return cv::BORDER_DEFAULT;
  }
}

void cedar::aux::conv::OpenCV::translateAnchor
     (
       cv::Point& anchor,
       const std::vector<int>& anchor_vector,
       const cv::Mat::MSize& msize
     ) const
{
  anchor = cv::Point(-1, -1);

  if (anchor_vector.size() >= 1 && anchor_vector.at(0) > 0)
  {
    int size = msize[0];
    anchor.x = cedar::aux::math::saturate(size/2 + anchor_vector.at(0), 0, size - 1);
  }
  if (anchor_vector.size() >= 2 && anchor_vector.at(1) > 0)
  {
    int size = msize[1];
    anchor.y = cedar::aux::math::saturate(size/2 + anchor_vector.at(1), 0, size - 1);
  }
}

void cedar::aux::conv::OpenCV::translateAnchor
     (
       cv::Point& anchor,
       cedar::aux::kernel::ConstKernelPtr kernel
     ) const
{
  anchor = cv::Point(-1, -1);
  const std::vector<int> anchor_vector = kernel->getAnchor();

  kernel->lockForRead();
  if (anchor_vector.size() >= 1 && anchor_vector.at(0) > 0)
  {
    int size = static_cast<int>(kernel->getSize(0));
    anchor.x = cedar::aux::math::saturate(size/2 + anchor_vector.at(0), 0, size - 1);
  }
  if (anchor_vector.size() >= 2 && anchor_vector.at(1) > 0)
  {
    int size = static_cast<int>(kernel->getSize(1));
    anchor.y = cedar::aux::math::saturate(size/2 + anchor_vector.at(1), 0, size - 1);
  }
  kernel->unlock();
}


cv::Mat cedar::aux::conv::OpenCV::convolve
        (
          const cv::Mat& matrix,
          const cv::Mat& kernel,
          cedar::aux::conv::BorderType::Id borderType,
          cedar::aux::conv::Mode::Id mode,
          const std::vector<int>& anchorVector
        ) const
{
  CEDAR_DEBUG_ASSERT(this->getKernelList()->size() == this->mKernelTypes.size());

  //!@todo Implement full mode!
  CEDAR_ASSERT(mode == cedar::aux::conv::Mode::Same);

  cv::Point anchor = cv::Point(-1, -1);
  this->translateAnchor(anchor, anchorVector, kernel.size);
  int border_type = this->translateBorderType(borderType);
  return this->cvConvolve(matrix, kernel, border_type, anchor);
}

cv::Mat cedar::aux::conv::OpenCV::convolve
        (
          const cv::Mat& matrix,
          const cedar::aux::kernel::ConstKernelPtr kernel,
          cedar::aux::conv::BorderType::Id borderType,
          cedar::aux::conv::Mode::Id mode
        ) const
{
  //!@todo implement mode properly
  CEDAR_ASSERT(mode == cedar::aux::conv::Mode::Same);
  cv::Point anchor = cv::Point(-1, -1);
  this->translateAnchor(anchor, kernel);
  int border_type = this->translateBorderType(borderType);
  return this->cvConvolve(matrix, kernel, border_type, anchor);
}

cv::Mat cedar::aux::conv::OpenCV::convolve
        (
          const cv::Mat& matrix,
          const cedar::aux::conv::KernelList& kernelList,
          cedar::aux::conv::BorderType::Id borderType,
          cedar::aux::conv::Mode::Id mode
        ) const
{
  int border_type = this->translateBorderType(borderType);

  //!@todo Implement mode
  CEDAR_ASSERT(mode == cedar::aux::conv::Mode::Same);

  cv::Mat result = 0.0 * matrix;
  for (size_t i = 0; i < kernelList.size(); ++i)
  {
    cedar::aux::kernel::ConstKernelPtr kernel = kernelList.getKernel(i);
    cv::Point anchor = cv::Point(-1, -1);
    this->translateAnchor(anchor, kernel);
    result += this->cvConvolve(matrix, kernel, border_type, anchor);
  }

  return result;
}

cv::Mat cedar::aux::conv::OpenCV::convolve
        (
          const cv::Mat& matrix,
          const cedar::aux::kernel::ConstSeparablePtr kernel,
          cedar::aux::conv::BorderType::Id borderType,
          cedar::aux::conv::Mode::Id mode
        ) const
{
  cv::Point anchor = cv::Point(-1, -1);
  //!@todo Implement mode properly
  CEDAR_ASSERT(mode == cedar::aux::conv::Mode::Same);
  int border_type = this->translateBorderType(borderType);

  this->translateAnchor(anchor, kernel);
  return cvConvolve(matrix, kernel, border_type, anchor);
}

cv::Mat cedar::aux::conv::OpenCV::cvConvolve
(
  const cv::Mat& matrix,
  const cedar::aux::kernel::ConstKernelPtr kernel,
  int cvBorderType,
  const cv::Point& anchor
) const
{
  if
  (
    cedar::aux::kernel::ConstSeparablePtr separable_kernel
      = boost::dynamic_pointer_cast<const cedar::aux::kernel::Separable>(kernel)
  )
  {
    return this->cvConvolve(matrix, separable_kernel, cvBorderType, anchor);
  }
  else
  {
    kernel->lockForRead();
    cv::Mat result;
    const cv::Mat& kernel_mat = kernel->getKernel();
    result = this->cvConvolve(matrix, kernel_mat, cvBorderType, anchor);
    kernel->unlock();
    return result;
  }
}

cv::Mat cedar::aux::conv::OpenCV::cvConvolve
        (
          const cv::Mat& matrix,
          const cedar::aux::kernel::ConstSeparablePtr kernel,
          int cvBorderType,
          const cv::Point& anchor
        ) const
{
  cv::Mat convolved;

  kernel->lockForRead();

  switch (kernel->getDimensionality())
  {
    case 0:
    {
      const cv::Mat& kernel_mat = kernel->getKernelPart(0);
      // in the 0d-case, kernel mat is a 1x1 matrix, thus convolution is the same as multiplication
      convolved = matrix * cedar::aux::math::getMatrixEntry<double>(kernel_mat, 0, 0);
      break;
    }

    case 1:
    {
      CEDAR_DEBUG_ASSERT(kernel->kernelPartCount() == 1);
      const cv::Mat& kernel_mat = kernel->getKernelPart(0);
      convolved = this->cvConvolve(matrix, kernel_mat, cvBorderType, anchor);
      break;
    }

    case 2:
    {
      CEDAR_DEBUG_ASSERT(kernel->kernelPartCount() == 2);
      const cv::Mat& kernel_mat_x = kernel->getKernelPart(1);
      const cv::Mat& kernel_mat_y = kernel->getKernelPart(0);

      cv::Mat flipped_kernel_mat_x, flipped_kernel_mat_y;
      cv::flip(kernel_mat_x, flipped_kernel_mat_x, -1);
      cv::flip(kernel_mat_y, flipped_kernel_mat_y, -1);


      if (cvBorderType != cv::BORDER_WRAP)
      {
        cv::sepFilter2D
        (
          matrix,
          convolved,
          -1,
          flipped_kernel_mat_x,
          flipped_kernel_mat_y,
          anchor,
          0,
          cvBorderType
        );
      }
      else
      {
        cv::Mat modified;
        //@todo For even kernels, this may pad 1 too much
        int dh = cedar::aux::math::get1DMatrixSize(flipped_kernel_mat_x) / 2;
        int dw = cedar::aux::math::get1DMatrixSize(flipped_kernel_mat_y) / 2;

        cv::copyMakeBorder(matrix, modified, dh, dh, dw, dw, cv::BORDER_WRAP);
        cv::sepFilter2D
        (
          modified,
          convolved,
          -1,
          flipped_kernel_mat_x,
          flipped_kernel_mat_y,
          anchor,
          0,
          cv::BORDER_DEFAULT
        );
        convolved = convolved(cv::Range(dh, dh + matrix.rows), cv::Range(dw, dw + matrix.cols));
      }


      break;
    }

    default:
      kernel->unlock();
      CEDAR_THROW(cedar::aux::UnhandledValueException, "Cannot convolve matrices of the given dimensionality.");
  }

  kernel->unlock();

  return convolved;
}

cv::Mat cedar::aux::conv::OpenCV::convolve
        (
          const cv::Mat& matrix,
          cedar::aux::conv::BorderType::Id borderType,
          cedar::aux::conv::Mode::Id mode
        ) const
{
  CEDAR_DEBUG_ASSERT(this->getKernelList()->size() == this->mKernelTypes.size());

  //!@todo Mode handling
  CEDAR_ASSERT(mode == cedar::aux::conv::Mode::Same);

  int border_type = this->translateBorderType(borderType);

  cv::Mat result = 0.0 * matrix;
  for (size_t i = 0; i < this->getKernelList()->size(); ++i)
  {
    cv::Mat convolved;

    switch (this->mKernelTypes.at(i))
    {
      //--------------------------------------------------------------------------------------
      case KERNEL_TYPE_SEPARABLE:
      //--------------------------------------------------------------------------------------
      {
        cedar::aux::kernel::ConstSeparablePtr kernel
          = cedar::aux::asserted_pointer_cast<const cedar::aux::kernel::Separable>(this->getKernelList()->getKernel(i));

        cv::Point anchor = cv::Point(-1, -1);
        this->translateAnchor(anchor, kernel);

        convolved = this->cvConvolve(matrix, kernel, border_type, anchor);
        break;
      }

      //--------------------------------------------------------------------------------------
      case KERNEL_TYPE_FULL:
      //--------------------------------------------------------------------------------------
      {
        cedar::aux::kernel::ConstKernelPtr kernel = this->getKernelList()->getKernel(i);

        cv::Point anchor = cv::Point(-1, -1);
        this->translateAnchor(anchor, kernel);

        kernel->lockForRead();
        cv::Mat kernel_mat = kernel->getKernel();
        convolved = this->cvConvolve(matrix, kernel_mat, border_type, anchor);
        kernel->unlock();
        break;
      }

      //--------------------------------------------------------------------------------------
      case KERNEL_TYPE_UNKNOWN:
      //--------------------------------------------------------------------------------------
        CEDAR_THROW(cedar::aux::UnknownTypeException, "Unknown kernel type.");
        break;

      default:
        CEDAR_THROW(cedar::aux::UnhandledValueException, "Unhandled kernel-type enum value.");
    }

    result += convolved;
  }
  return result;
}

cv::Mat cedar::aux::conv::OpenCV::cvConvolve
(
  const cv::Mat& matrix,
  const cv::Mat& kernel,
  int cvBorderType,
  const cv::Point& anchor
) const
{
  if (cedar::aux::math::getDimensionalityOf(matrix) > 2)
  {
    CEDAR_THROW(cedar::aux::UnhandledValueException, "Cannot convolve matrices of the given dimensionality.");
  }

  //!@todo Cache the flipped matrices?
  cv::Mat flipped_kernel;
  cv::flip(kernel, flipped_kernel, -1);
  cv::Mat result;

  if (cvBorderType != cv::BORDER_WRAP)
  {
    cv::filter2D(matrix, result, -1, flipped_kernel, anchor, 0.0, cvBorderType);
  }
  else
  {
    cv::Mat modified;
    //@todo For even kernels, this may pad 1 too much
    int dh = flipped_kernel.rows / 2;
    int dw = flipped_kernel.cols / 2;

    cv::copyMakeBorder(matrix, modified, dh, dh, dw, dw, cv::BORDER_WRAP);
    cv::filter2D(modified, result, -1, flipped_kernel, anchor, 0.0, cv::BORDER_DEFAULT);
    result = result(cv::Range(dh, dh + matrix.rows), cv::Range(dw, dw + matrix.cols));
  }

  return result;
}

void cedar::aux::conv::OpenCV::kernelRemoved(size_t index)
{
  CEDAR_DEBUG_ASSERT(index < this->mKernelTypes.size());
  this->mKernelTypes.erase(this->mKernelTypes.begin() + index);
}

void cedar::aux::conv::OpenCV::updateKernelType(size_t index)
{
  if (this->mKernelTypes.size() <= index)
  {
    this->mKernelTypes.resize(index + 1, KERNEL_TYPE_UNKNOWN);
  }


  cedar::aux::kernel::ConstKernelPtr kernel = this->getKernelList()->getKernel(index);
  if (boost::dynamic_pointer_cast<const cedar::aux::kernel::Separable>(kernel))
  {
    this->mKernelTypes.at(index) = KERNEL_TYPE_SEPARABLE;
  }
  else
  {
    this->mKernelTypes.at(index) = KERNEL_TYPE_FULL;
  }
}

void cedar::aux::conv::OpenCV::setKernelList(cedar::aux::conv::KernelListPtr kernelList)
{
  this->Engine::setKernelList(kernelList);
  for (size_t i = 0; i < this->getKernelList()->size(); ++i)
  {
    this->updateKernelType(i);
  }
  this->getKernelList()->connectToKernelAddedSignal(boost::bind(&cedar::aux::conv::OpenCV::updateKernelType, this, _1));
  this->getKernelList()->connectToKernelChangedSignal(boost::bind(&cedar::aux::conv::OpenCV::updateKernelType, this, _1));
  this->getKernelList()->connectToKernelRemovedSignal(boost::bind(&cedar::aux::conv::OpenCV::kernelRemoved, this, _1));
}
