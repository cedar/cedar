/*======================================================================================================================

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
#include "cedar/auxiliaries/convolution/EngineManager.h"
#include "cedar/auxiliaries/kernel/Separable.h"
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

cedar::aux::conv::OpenCV::~OpenCV()
{
  mKernelAddedConnection.disconnect();
  mKernelChangedConnection.disconnect();
  mKernelRemovedConnection.disconnect();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::aux::conv::OpenCV::checkCapability
     (
       size_t matrixDim,
       size_t kernelDim,
       cedar::aux::conv::BorderType::Id borderType,
       cedar::aux::conv::Mode::Id mode
     )
     const
{
  if (matrixDim > 2 || kernelDim > 2)
  {
    return false;
  }

  if (!this->checkBorderTypeCapability(borderType) || !this->checkModeCapability(mode))
  {
    return false;
  }

  return true;
}

bool cedar::aux::conv::OpenCV::checkCapability
     (
       cv::Mat matrix,
       cv::Mat kernel,
       cedar::aux::conv::BorderType::Id borderType,
       cedar::aux::conv::Mode::Id mode
     ) const
{
  if (!this->checkCapability(cedar::aux::math::getDimensionalityOf(matrix), cedar::aux::math::getDimensionalityOf(kernel), borderType, mode))
  {
    return false;
  }

  return true;
}

bool cedar::aux::conv::OpenCV::checkBorderTypeCapability
     (
       cedar::aux::conv::BorderType::Id borderType
     ) const
{
  switch (borderType)
  {
    case cedar::aux::conv::BorderType::Cyclic:
    case cedar::aux::conv::BorderType::Reflect:
    case cedar::aux::conv::BorderType::Replicate:
    case cedar::aux::conv::BorderType::Zero:
      return true;

    default:
      return false;
  }
}

bool cedar::aux::conv::OpenCV::checkModeCapability
     (
       cedar::aux::conv::Mode::Id mode
     ) const
{
  return mode == cedar::aux::conv::Mode::Same
      || mode == cedar::aux::conv::Mode::Full
      || mode == cedar::aux::conv::Mode::Valid;
}

void cedar::aux::conv::OpenCV::translateAnchor
     (
       cv::Point& anchor,
       std::vector<int> anchor_vector,
       const std::vector<int>& sizes,
       bool alternateEvenCenter
     ) const
{
  anchor = cv::Point(-1, -1);

  if (alternateEvenCenter && anchor_vector.size() < 1)
  {
    anchor_vector.push_back(0);
  }
  if (alternateEvenCenter && anchor_vector.size() < 2)
  {
    anchor_vector.push_back(0);
  }

  std::vector<int> point;
  point.assign(2, -1);

  for (size_t i = 0; i < 2; ++i)
  {
    if (anchor_vector.size() > i && sizes.size() > i)
    {
      int size = sizes[i];
      point[i] = cedar::aux::math::saturate(size/2 + anchor_vector.at(i), 0, size - 1);
      if (alternateEvenCenter && size % 2 == 0 && point[i] > 0)
      {
        point[i] -= 1;
      }
    }
  }

  anchor.x = point[1]; // [1] because x is columns in opencv
  anchor.y = point[0]; // [0] because y is rows in opencv
}

void cedar::aux::conv::OpenCV::translateAnchor
     (
       cv::Point& anchor,
       const std::vector<int>& anchor_vector,
#if CEDAR_OPENCV_MAJOR_VERSION >= 3
       const cv::MatSize& msize
#else
       const cv::Mat::MSize& msize
#endif
       ,
       bool alternateEvenCenter
     ) const
{
  std::vector<int> sizes(2);
  sizes[0] = msize[0];
  sizes[1] = msize[1];
  this->translateAnchor(anchor, anchor_vector, sizes, alternateEvenCenter);
}

void cedar::aux::conv::OpenCV::translateAnchor
     (
       cv::Point& anchor,
       cedar::aux::kernel::ConstKernelPtr kernel,
       const cv::Mat& /* matrix */,
       bool alternateEvenCenter
     ) const
{
  QReadLocker locker(kernel->getReadWriteLock());
  std::vector<int> sizes;
  const std::vector<int> anchor_vector = kernel->getAnchor();
  auto dim = kernel->getDimensionality();
  cv::Mat kernel_mat = kernel->getKernel().clone();
  for (size_t d = 0; d < dim; ++d)
  {
    sizes.push_back(static_cast<int>(kernel->getSize(d)));
  }
  locker.unlock();
  this->translateAnchor(anchor, anchor_vector, sizes, alternateEvenCenter);

  if (dim == 1 && kernel_mat.rows == 1)
  {
    std::swap(anchor.x, anchor.y);
  }
}

cv::Mat cedar::aux::conv::OpenCV::createFullMatrix
        (
          const cv::Mat& matrix,
          int kernelRows,
          int kernelCols,
          cedar::aux::conv::BorderType::Id borderType
        ) const
{
  // In the 0D and 1D case there is kernelRows and/or kernelCols 0 in some cases.
  // This means that nothing has to be done, but for computational reasons kernel_SIZE_ has to be set to 1 to do so.
  if (kernelRows == 0)
  {
    kernelRows = 1;
  }

  if (kernelCols == 0)
  {
    kernelCols = 1;
  }

  int top = kernelRows - 1;
  int bottom = kernelRows - 1;
  int left = kernelCols - 1;
  int right = kernelCols - 1;

  cv::Mat full_matrix;
  cv::copyMakeBorder(matrix, full_matrix, top, bottom, left, right, cedar::aux::conv::BorderType::toCvConstant(borderType));

  return full_matrix;
}

cv::Mat cedar::aux::conv::OpenCV::createFullMatrix
        (
          const cv::Mat& matrix,
          const cv::Mat& kernel,
          cedar::aux::conv::BorderType::Id borderType
        ) const
{
  return createFullMatrix(matrix, kernel.rows, kernel.cols, borderType);
}

cv::Mat cedar::aux::conv::OpenCV::createFullMatrix
        (
          const cv::Mat& matrix,
          const cedar::aux::kernel::ConstKernelPtr kernel,
          cedar::aux::conv::BorderType::Id borderType
        ) const
{
  const cv::Mat& kernel_mat = kernel->getKernel();
  return createFullMatrix
         (
           matrix,
           static_cast<unsigned int>(kernel_mat.rows),
           static_cast<unsigned int>(kernel_mat.cols),
           borderType
         );
}

cv::Mat cedar::aux::conv::OpenCV::createFullMatrix
        (
          const cv::Mat& matrix,
          cedar::aux::conv::ConstKernelListPtr kernelList,
          cedar::aux::conv::BorderType::Id borderType
        ) const
{

  if (!kernelList->checkForSameKernelSize())
  {
    CEDAR_THROW(cedar::aux::UnhandledValueException, "Full Convolution mode only supported if all kernels have same size");
  }

  cedar::aux::kernel::ConstKernelPtr kernel = kernelList->getKernel(0);

  const cv::Mat& kernel_mat = kernel->getKernel();
  return createFullMatrix
         (
           matrix,
           static_cast<unsigned int>(kernel_mat.rows),
           static_cast<unsigned int>(kernel_mat.cols),
           borderType
         );
}

cv::Mat cedar::aux::conv::OpenCV::createFullMatrix
        (
          const cv::Mat& matrix,
          const cedar::aux::kernel::ConstSeparablePtr kernel,
          cedar::aux::conv::BorderType::Id borderType
        ) const
{
  const cv::Mat& kernel_mat = kernel->getKernel();
  return createFullMatrix
         (
           matrix,
           static_cast<unsigned int>(kernel_mat.rows),
           static_cast<unsigned int>(kernel_mat.cols),
           borderType
         );
}

cv::Mat cedar::aux::conv::OpenCV::createFullMatrix
        (
          const cv::Mat& matrix,
          cedar::aux::conv::BorderType::Id borderType
        ) const
{
  cedar::aux::kernel::ConstKernelPtr kernel = this->getKernelList()->getKernel(0);

  const cv::Mat& kernel_mat = kernel->getKernel();
  return createFullMatrix
         (
           matrix,
           static_cast<unsigned int>(kernel_mat.rows),
           static_cast<unsigned int>(kernel_mat.cols),
           borderType
         );
}

cv::Mat cedar::aux::conv::OpenCV::cutOutResult(
                                              const cv::Mat& result,
                                              unsigned int matrixRows,
                                              unsigned int matrixCols,
                                              unsigned int kernelRows,
                                              unsigned int kernelCols
                                              ) const
{
  // In the 0D and 1D case there is kernelRows and/or kernelCols 0 in some cases.
  // This means that nothing has to be done, but for computational reasons kernel_SIZE_ has to be set to 1 to do so.
  if (kernelRows == 0)
  {
    kernelRows = 1;
  }

  if (kernelCols == 0)
  {
    kernelCols = 1;
  }

  unsigned int cut_rows = matrixRows + kernelRows - 1;
  unsigned int cut_cols = matrixCols + kernelCols - 1;
  unsigned int offset_row = (result.rows - cut_rows) / 2;
  unsigned int offset_col = (result.cols - cut_cols) / 2;

  cv::Mat cut_out = result(cv::Range(offset_row, offset_row + cut_rows), cv::Range(offset_col, offset_col + cut_cols));

  return cut_out;
}

cv::Mat cedar::aux::conv::OpenCV::cutOutResult(
                                              const cv::Mat& result,
                                              const cv::Mat& matrix,
                                              const cv::Mat& kernel
                                              ) const
{
  return cutOutResult(result, matrix.rows, matrix.cols, kernel.rows, kernel.cols);
}

cv::Mat cedar::aux::conv::OpenCV::cutOutResult(
                                              const cv::Mat& result,
                                              const cv::Mat& matrix,
                                              const cedar::aux::kernel::ConstKernelPtr kernel
                                              ) const
{
  // check for 0D-Kernels:
  if (kernel->getDimensionality() == 0)
  {
    // no border handling necessary
    return result;
  }
  else
  {
    const cv::Mat& kernel_mat = kernel->getKernel();
    return cutOutResult
           (
             result,
             matrix.rows,
             matrix.cols,
             static_cast<unsigned int>(kernel_mat.rows),
             static_cast<unsigned int>(kernel_mat.cols)
           );
  }
}

cv::Mat cedar::aux::conv::OpenCV::cutOutResult(
                                              const cv::Mat& result,
                                              unsigned int kernelRows,
                                              unsigned int kernelCols
                                              ) const
{
  // In the 0D and 1D case there is kernelRows and/or kernelCols 0 in some cases.
  // This means that nothing has to be done, but for computational reasons kernel_SIZE_ has to be set to 1 to do so.
  if (kernelRows == 0)
  {
    kernelRows = 1;
  }

  if (kernelCols == 0)
  {
    kernelCols = 1;
  }

  unsigned int cut_rows = result.rows - kernelRows + 1;
  unsigned int cut_cols = result.cols - kernelCols + 1;
  unsigned int offset_row = (result.rows - cut_rows) / 2;
  unsigned int offset_col = (result.cols - cut_cols) / 2;

  cv::Mat cut_out = result(cv::Range(offset_row, offset_row + cut_rows), cv::Range(offset_col, offset_col + cut_cols));

  return cut_out;
}

cv::Mat cedar::aux::conv::OpenCV::cutOutResult(
                                              const cv::Mat& result,
                                              const cv::Mat& kernel
                                              ) const
{
  return cutOutResult(result, kernel.rows, kernel.cols);
}

cv::Mat cedar::aux::conv::OpenCV::cutOutResult(
                                              const cv::Mat& result,
                                              const cedar::aux::kernel::ConstKernelPtr kernel
                                              ) const
{
  // check for 0D-Kernels:
  if (kernel->getDimensionality() == 0)
  {
    // no border handling necessary
    return result;
  }
  else
  {
    const cv::Mat& kernel_mat = kernel->getKernel();
    return cutOutResult
           (
             result,
             static_cast<unsigned int>(kernel_mat.rows),
             static_cast<unsigned int>(kernel_mat.cols)
           );
  }
}

cv::Mat cedar::aux::conv::OpenCV::convolve
        (
          const cv::Mat& matrix,
          const cv::Mat& kernel,
          cedar::aux::conv::BorderType::Id borderType,
          cedar::aux::conv::Mode::Id mode,
          const std::vector<int>& anchorVector,
          bool alternateEvenCenter
        ) const
{
  CEDAR_ASSERT
  (
    mode == cedar::aux::conv::Mode::Same
    || mode == cedar::aux::conv::Mode::Full
    || mode == cedar::aux::conv::Mode::Valid
  );

  cv::Mat result;
  switch(mode)
  {
    case cedar::aux::conv::Mode::Same:
      {
        cv::Point anchor = cv::Point(-1, -1);
        this->translateAnchor(anchor, anchorVector, kernel.size, alternateEvenCenter);
        int border_type = cedar::aux::conv::BorderType::toCvConstant(borderType);
        result = this->cvConvolve(matrix, kernel, border_type, anchor);
      }
      break;

    case cedar::aux::conv::Mode::Full:
      {
        cv::Mat matrix_full = createFullMatrix(matrix, kernel, borderType);
        cv::Point anchor = cv::Point(-1, -1);
        this->translateAnchor(anchor, anchorVector, kernel.size, alternateEvenCenter);
        // border type dose not matter, because cut off
        cv::Mat result_full = this->cvConvolve(matrix_full, kernel, cv::BORDER_CONSTANT, anchor);
        result = cutOutResult(result_full, matrix, kernel);
      }
      break;
    case cedar::aux::conv::Mode::Valid:
      if (matrix.rows < kernel.rows || matrix.cols < kernel.cols)
      {
        result = cv::Mat::zeros(1,1,matrix.type());
      }
      else
      {
        cv::Point anchor = cv::Point(-1, -1);
        this->translateAnchor(anchor, anchorVector, kernel.size, alternateEvenCenter);
        // border type dose not matter, because cut off
        cv::Mat result_full = this->cvConvolve(matrix, kernel, cv::BORDER_CONSTANT, anchor);
        result = cutOutResult(result_full, kernel);
      }
      break;
    default:
      CEDAR_THROW(cedar::aux::BadConnectionException,"\"CEDAR_ASSERT\" failed before this exception.");
      break;
  }
  return result;
}

cv::Mat cedar::aux::conv::OpenCV::convolve
        (
          const cv::Mat& matrix,
          const cedar::aux::kernel::ConstKernelPtr kernel,
          cedar::aux::conv::BorderType::Id borderType,
          cedar::aux::conv::Mode::Id mode,
          bool alternateEvenCenter
        ) const
{
  CEDAR_ASSERT
  (
    mode == cedar::aux::conv::Mode::Same
    || mode == cedar::aux::conv::Mode::Full
    || mode == cedar::aux::conv::Mode::Valid
  );

  cv::Mat result;
  switch(mode)
  {
    case cedar::aux::conv::Mode::Same:
      {
        cv::Point anchor = cv::Point(-1, -1);
        this->translateAnchor(anchor, kernel, matrix, alternateEvenCenter);
        int border_type = cedar::aux::conv::BorderType::toCvConstant(borderType);
        result = this->cvConvolve(matrix, kernel, border_type, anchor);
      }
      break;
    case cedar::aux::conv::Mode::Full:
      {
        cv::Mat matrix_full = createFullMatrix(matrix, kernel, borderType);
        cv::Point anchor = cv::Point(-1, -1);
        this->translateAnchor(anchor, kernel, matrix, alternateEvenCenter);

        // border type dose not matter, because cut off
        cv::Mat result_full = this->cvConvolve(matrix_full, kernel, cv::BORDER_CONSTANT, anchor);
        result = cutOutResult(result_full, matrix, kernel);
      }
      break;
    case cedar::aux::conv::Mode::Valid:
      {
        const cv::Mat& kernel_mat = kernel->getKernel();
        if (matrix.rows < kernel_mat.rows || matrix.cols < kernel_mat.cols)
        {
          result = cv::Mat::zeros(1,1,matrix.type());
        }
        else
        {
          cv::Point anchor = cv::Point(-1, -1);
          this->translateAnchor(anchor, kernel, matrix, alternateEvenCenter);
          // border type dose not matter, because cut off
          cv::Mat result_full = this->cvConvolve(matrix, kernel, cv::BORDER_CONSTANT, anchor);
          result = cutOutResult(result_full, kernel);
        }
      }
      break;
    default:
      CEDAR_THROW(cedar::aux::BadConnectionException,"\"CEDAR_ASSERT\" failed before this exception.");
      break;
  }
  return result;
}

cv::Mat cedar::aux::conv::OpenCV::convolve
        (
          const cv::Mat& matrix,
          cedar::aux::conv::ConstKernelListPtr kernelList,
          cedar::aux::conv::BorderType::Id borderType,
          cedar::aux::conv::Mode::Id mode,
          bool alternateEvenCenter
        ) const
{
  int border_type = cedar::aux::conv::BorderType::toCvConstant(borderType);

  CEDAR_ASSERT
  (
    mode == cedar::aux::conv::Mode::Same
    || mode == cedar::aux::conv::Mode::Full
    || mode == cedar::aux::conv::Mode::Valid
  );

  switch(mode)
  {
  case cedar::aux::conv::Mode::Same:
    {
      cv::Mat result = 0.0 * matrix;
      for (size_t i = 0; i < kernelList->size(); ++i)
      {
        cedar::aux::kernel::ConstKernelPtr kernel = kernelList->getKernel(i);
        cv::Point anchor = cv::Point(-1, -1);
        this->translateAnchor(anchor, kernel, matrix, alternateEvenCenter);
        result += this->cvConvolve(matrix, kernel, border_type, anchor);
      }

      return result;
    }
    break;
  case cedar::aux::conv::Mode::Full:
    {
      if (kernelList->checkForSameKernelSize())
      {
        cv::Mat matrix_full = createFullMatrix(matrix, kernelList, borderType);

        cv::Mat result = 0.0 * matrix_full;
        for (size_t i = 0; i < kernelList->size(); ++i)
        {
          cedar::aux::kernel::ConstKernelPtr kernel = kernelList->getKernel(i);
          cv::Point anchor = cv::Point(-1, -1);
          this->translateAnchor(anchor, kernel, matrix, alternateEvenCenter);
          result += this->cvConvolve(matrix_full, kernel, border_type, anchor);
        }

        return cutOutResult(result,matrix,kernelList->getKernel(0));
      }
      else
      {
        CEDAR_THROW
        (
          cedar::aux::UnhandledValueException,
          "Full Convolution mode only supported if all kernels have same size"
        );
      }

    }
    break;
  case cedar::aux::conv::Mode::Valid:
    {
      if (kernelList->checkForSameKernelSize())
      {
        const cv::Mat& kernel_mat = kernelList->getKernel(0)->getKernel();
        if (matrix.rows < kernel_mat.rows || matrix.cols < kernel_mat.cols)
        {
          return cv::Mat::zeros(1,1,matrix.type());
        }
        else
        {
          cv::Mat result = 0.0 * matrix.clone();
          for (size_t i = 0; i < kernelList->size(); ++i)
          {
            cedar::aux::kernel::ConstKernelPtr kernel = kernelList->getKernel(i);
            cv::Point anchor = cv::Point(-1, -1);
            this->translateAnchor(anchor, kernel, matrix, alternateEvenCenter);
            result += this->cvConvolve(matrix, kernel, border_type, anchor);
          }

          return cutOutResult(result,kernelList->getKernel(0));
        }
      }
      else
      {
        CEDAR_THROW
        (
          cedar::aux::UnhandledValueException,
          "Full Convolution mode only supported if all kernels have same size"
        );
      }

    }
    break;
  default:
    CEDAR_THROW(cedar::aux::BadConnectionException,"\"CEDAR_ASSERT\" failed before this exception.");
    break;
  }
}

cv::Mat cedar::aux::conv::OpenCV::convolveSeparable
        (
          const cv::Mat& matrix,
          const cedar::aux::kernel::ConstSeparablePtr kernel,
          cedar::aux::conv::BorderType::Id borderType,
          cedar::aux::conv::Mode::Id mode,
          bool alternateEvenCenter
        ) const
{
  cv::Point anchor = cv::Point(-1, -1);

  CEDAR_ASSERT
  (
    mode == cedar::aux::conv::Mode::Same
    || mode == cedar::aux::conv::Mode::Full
    || mode == cedar::aux::conv::Mode::Valid
  );

  switch(mode)
  {
    case cedar::aux::conv::Mode::Same:
      {
        int border_type = cedar::aux::conv::BorderType::toCvConstant(borderType);

        this->translateAnchor(anchor, kernel, matrix, alternateEvenCenter);
        return cvConvolve(matrix, kernel, border_type, anchor);
      }
      break;
    case cedar::aux::conv::Mode::Full:
      {
        cv::Mat matrix_full = createFullMatrix(matrix, kernel, borderType);

        int border_type = cedar::aux::conv::BorderType::toCvConstant(borderType);

        this->translateAnchor(anchor, kernel, matrix, alternateEvenCenter);
        cv::Mat result_full = cvConvolve(matrix_full, kernel, border_type, anchor);
        return cutOutResult(result_full,matrix,kernel);
      }
      break;
    case cedar::aux::conv::Mode::Valid:
      {
        const cv::Mat& kernel_mat = kernel->getKernel();
        if (matrix.rows < kernel_mat.rows || matrix.cols < kernel_mat.cols)
        {
          return cv::Mat::zeros(1,1,matrix.type());
        }
        else
        {
          int border_type = cedar::aux::conv::BorderType::toCvConstant(borderType);

          this->translateAnchor(anchor, kernel, matrix, alternateEvenCenter);
          cv::Mat result_full = cvConvolve(matrix, kernel, border_type, anchor);
          return cutOutResult(result_full,kernel);
        }
      }
      break;
    default:
      CEDAR_THROW(cedar::aux::BadConnectionException,"\"CEDAR_ASSERT\" failed before this exception.");
      break;
  }
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
    QReadLocker locker(kernel->getReadWriteLock());
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
          cv::Point anchor
        ) const
{
  cv::Mat convolved;

  QReadLocker locker(kernel->getReadWriteLock());

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
      cv::Mat kernel_mat = kernel->getKernelPart(0);
      if ((matrix.rows == 1 && kernel_mat.rows != 1) || (matrix.cols == 1 && kernel_mat.cols != 1))
      {
        kernel_mat = kernel_mat.t();
        std::swap(anchor.x, anchor.y);
      }

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
        int height = static_cast<int>(cedar::aux::math::get1DMatrixSize(flipped_kernel_mat_x));
        int width = static_cast<int>(cedar::aux::math::get1DMatrixSize(flipped_kernel_mat_y));
        int dh = height / 2;
        int dw = width / 2;

        // height - dh makes sure that in uneven cases, padding is not too small or too large
        cv::copyMakeBorder(matrix, modified, dh, height - dh, dw, width - dw, cv::BORDER_WRAP);
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
      CEDAR_THROW(cedar::aux::UnhandledValueException, "Cannot convolve matrices of the given dimensionality.");
  }

  locker.unlock();

  return convolved;
}

cv::Mat cedar::aux::conv::OpenCV::convolve
        (
          const cv::Mat& matrix,
          cedar::aux::conv::BorderType::Id borderType,
          cedar::aux::conv::Mode::Id mode,
          bool alternateEvenCenter
        ) const
{
  CEDAR_DEBUG_ASSERT(this->getKernelList()->size() == this->mKernelTypes.size());

  CEDAR_ASSERT
  (
    mode == cedar::aux::conv::Mode::Same
    || mode == cedar::aux::conv::Mode::Full
    || mode == cedar::aux::conv::Mode::Valid
  );

  int cv_border_type = cedar::aux::conv::BorderType::toCvConstant(borderType);

  switch(mode)
  {
    case cedar::aux::conv::Mode::Same:
      {
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
                = cedar::aux::asserted_pointer_cast<const cedar::aux::kernel::Separable>
                  (
                    this->getKernelList()->getKernel(i)
                  );

              cv::Point anchor = cv::Point(-1, -1);
              this->translateAnchor(anchor, kernel, matrix, alternateEvenCenter);

              convolved = this->cvConvolve(matrix, kernel, cv_border_type, anchor);
              break;
            }

            //--------------------------------------------------------------------------------------
            case KERNEL_TYPE_FULL:
            //--------------------------------------------------------------------------------------
            {
              cedar::aux::kernel::ConstKernelPtr kernel = this->getKernelList()->getKernel(i);

              cv::Point anchor = cv::Point(-1, -1);
              this->translateAnchor(anchor, kernel, matrix, alternateEvenCenter);

              QReadLocker locker(kernel->getReadWriteLock());
              cv::Mat kernel_mat = kernel->getKernel();
              convolved = this->cvConvolve(matrix, kernel_mat, cv_border_type, anchor);
              locker.unlock();
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
      break;
    case cedar::aux::conv::Mode::Full:
      {
        bool same_size = this->getKernelList()->checkForSameKernelSize();

        if (same_size)
        {
          cv::Mat matrix_full = createFullMatrix(matrix, borderType);
          cv::Mat result = 0.0 * matrix_full;

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
                  = cedar::aux::asserted_pointer_cast<const cedar::aux::kernel::Separable>
                    (
                      this->getKernelList()->getKernel(i)
                    );

                cv::Point anchor = cv::Point(-1, -1);
                this->translateAnchor(anchor, kernel, matrix, alternateEvenCenter);

                convolved = this->cvConvolve(matrix_full, kernel, cv_border_type, anchor);
                break;
              }

              //--------------------------------------------------------------------------------------
              case KERNEL_TYPE_FULL:
              //--------------------------------------------------------------------------------------
              {
                cedar::aux::kernel::ConstKernelPtr kernel = this->getKernelList()->getKernel(i);

                cv::Point anchor = cv::Point(-1, -1);
                this->translateAnchor(anchor, kernel, matrix, alternateEvenCenter);

                QReadLocker locker(kernel->getReadWriteLock());
                cv::Mat kernel_mat = kernel->getKernel();
                convolved = this->cvConvolve(matrix_full, kernel_mat, cv_border_type, anchor);
                locker.unlock();
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
          return cutOutResult(result,matrix,this->getKernelList()->getKernel(0));
        }
        else // kernels do not have same size
        {
          CEDAR_THROW
          (
            cedar::aux::UnhandledValueException,
            "Full Convolution mode only supported if all kernels have same size"
          );
        }
      }
      break;
    case cedar::aux::conv::Mode::Valid:
      {
        if (this->getKernelList()->checkForSameKernelSize())
        {
          const cv::Mat& kernel_mat = this->getKernelList()->getKernel(0)->getKernel();
          if (matrix.rows < kernel_mat.rows || matrix.cols < kernel_mat.cols)
          {
            return cv::Mat::zeros(1,1,matrix.type());
          }
          else
          {
            cv::Mat result = 0.0 * matrix.clone();

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
                    = cedar::aux::asserted_pointer_cast<const cedar::aux::kernel::Separable>
                      (
                        this->getKernelList()->getKernel(i)
                      );

                  cv::Point anchor = cv::Point(-1, -1);
                  this->translateAnchor(anchor, kernel, matrix, alternateEvenCenter);

                  convolved = this->cvConvolve(matrix, kernel, cv_border_type, anchor);
                  break;
                }
              //--------------------------------------------------------------------------------------
              case KERNEL_TYPE_FULL:
              //--------------------------------------------------------------------------------------
                {
                  cedar::aux::kernel::ConstKernelPtr kernel = this->getKernelList()->getKernel(i);

                  cv::Point anchor = cv::Point(-1, -1);
                  this->translateAnchor(anchor, kernel, matrix, alternateEvenCenter);

                  QReadLocker locker(kernel->getReadWriteLock());
                  cv::Mat kernel_mat = kernel->getKernel();
                  convolved = this->cvConvolve(matrix, kernel_mat, cv_border_type, anchor);
                  locker.unlock();
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
            return cutOutResult(result,this->getKernelList()->getKernel(0));
          }
        }
        else // kernels do not have same size
        {
          CEDAR_THROW
          (
            cedar::aux::UnhandledValueException,
            "Full Convolution mode only supported if all kernels have same size"
          );
        }
      }
      break;
    default:
      CEDAR_THROW(cedar::aux::BadConnectionException,"\"CEDAR_ASSERT\" failed before this exception.");
      break;
  }
}

cv::Mat cedar::aux::conv::OpenCV::cvConvolve
(
  const cv::Mat& matrix,
  const cv::Mat& kernel,
  int cvBorderType,
  cv::Point anchor
) const
{
  if (cedar::aux::math::getDimensionalityOf(matrix) > 2)
  {
    CEDAR_THROW(cedar::aux::UnhandledValueException, "Cannot convolve matrices of the given dimensionality.");
  }

  cv::Mat flipped_kernel;
  cv::flip(kernel, flipped_kernel, -1);
  cv::Mat result;

  // make sure the kernel isn't too large in any of the directions
  if (cvBorderType == cv::BORDER_CONSTANT)
  {
    if (flipped_kernel.rows > 2 * matrix.rows)
    {
      int center = flipped_kernel.rows / 2;
      // matrix is zero outside the image, kernel can be reduced to twice the image size
      flipped_kernel = flipped_kernel(cv::Range(center - matrix.rows, center + matrix.rows + 1), cv::Range::all());
      anchor.y = -1;
    }
    if (flipped_kernel.cols > 2 * matrix.cols)
    {
      int center = flipped_kernel.cols / 2;
      // matrix is zero outside the image, kernel can be reduced to twice the image size
      flipped_kernel = flipped_kernel(cv::Range::all(), cv::Range(center - matrix.cols, center + matrix.cols + 1));
      anchor.x = -1;
    }
  }
  else if (cvBorderType == cv::BORDER_WRAP)
  {
    // if the kernel is larger than the image, it can be reduced to an equivalent kernel of the same size as the image
    // this is only possible due to the cyclic border conditions
    if (flipped_kernel.rows > matrix.rows || flipped_kernel.cols > matrix.cols)
    {
      // in this case, the anchor should be the middle of the kernel
      //!@todo Not sure this is the right/complete approach (the summation should take the anchor into account)
      anchor.x = -1;
      anchor.y = -1;

      cv::Mat summed = cv::Mat::zeros
                       (
                         std::min(matrix.rows, flipped_kernel.rows),
                         std::min(matrix.cols, flipped_kernel.cols),
                         flipped_kernel.type()
                       );

      // first, we have to (virtually) increase the size of the kernel to a multiple of the image size
      int num_complete_sums_row = flipped_kernel.rows / matrix.rows; // floored on purpose!
      int num_complete_sums_col = flipped_kernel.cols / matrix.cols; // floored on purpose!

      // if there is a remainder in the division, count that
      if (flipped_kernel.rows % matrix.rows != 0)
      {
        num_complete_sums_row += 1;
      }
      if (flipped_kernel.cols % matrix.cols != 0)
      {
        num_complete_sums_col += 1;
      }

      int adapted_kernel_rows = num_complete_sums_row * matrix.rows;
      int adapted_kernel_cols = num_complete_sums_col * matrix.cols;

      int row_offset_top = (adapted_kernel_rows - flipped_kernel.rows) / 2;
      int row_offset_bottom = (adapted_kernel_rows - flipped_kernel.rows + 1) / 2;
      int col_offset_left = (adapted_kernel_cols - flipped_kernel.cols) / 2;
      int col_offset_right = (adapted_kernel_cols - flipped_kernel.cols + 1) / 2;

      // then sum over image-sized sections of the kernel
      for (int start_row = 0; start_row < num_complete_sums_row; start_row += 1)
      {
        // construct ranges that lie within the kernel size
        cv::Range summed_row_range
                  (
                    start_row == 0? row_offset_top : 0,
                    start_row == num_complete_sums_row - 1? matrix.rows - row_offset_bottom : matrix.rows
                  );
        cv::Range kernel_row_range
                  (
                    std::max(0, start_row * matrix.rows - row_offset_top),
                    std::min((start_row + 1) * matrix.rows - row_offset_top, flipped_kernel.rows)
                  );

        for (int start_col = 0; start_col < num_complete_sums_col; start_col += 1)
        {
          cv::Range summed_col_range
          (
            start_col == 0? col_offset_left : 0,
            start_col == num_complete_sums_col - 1? matrix.cols - col_offset_right : matrix.cols
          );
          cv::Range kernel_col_range
          (
            std::max(0, start_col * matrix.cols - col_offset_left),
            std::min((start_col + 1) * matrix.cols - col_offset_left, flipped_kernel.cols)
          );

          summed(summed_row_range, summed_col_range) += flipped_kernel(kernel_row_range, kernel_col_range);
        }
      }

      flipped_kernel = summed;
    }
  }

  if (cvBorderType != cv::BORDER_WRAP)
  {
    cv::filter2D(matrix, result, -1, flipped_kernel, anchor, 0.0, cvBorderType);
  }
  else
  {
    cv::Mat modified;
    int dh = flipped_kernel.rows / 2;
    int dw = flipped_kernel.cols / 2;

    // rows - dh makes sure that in uneven cases, padding is not too small or too large
    cv::copyMakeBorder(matrix, modified, dh, flipped_kernel.rows - dh, dw, flipped_kernel.cols - dw, cv::BORDER_WRAP);
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
  this->mKernelTypes.resize(this->getKernelList()->size(), KERNEL_TYPE_UNKNOWN);

  CEDAR_DEBUG_ASSERT(index < this->mKernelTypes.size());

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
  mKernelAddedConnection.disconnect();
  mKernelChangedConnection.disconnect();
  mKernelRemovedConnection.disconnect();
  this->Engine::setKernelList(kernelList);
  this->mKernelTypes.clear();
  for (size_t i = 0; i < this->getKernelList()->size(); ++i)
  {
    this->updateKernelType(i);
  }
  mKernelAddedConnection
    = this->getKernelList()->connectToKernelAddedSignal
                             (
                               boost::bind(&cedar::aux::conv::OpenCV::updateKernelType, this, _1)
                             );
  mKernelChangedConnection
    = this->getKernelList()->connectToKernelChangedSignal
                             (
                               boost::bind(&cedar::aux::conv::OpenCV::updateKernelType, this, _1)
                             );
  mKernelRemovedConnection
    = this->getKernelList()->connectToKernelRemovedSignal
                             (
                               boost::bind(&cedar::aux::conv::OpenCV::kernelRemoved, this, _1)
                             );
}
