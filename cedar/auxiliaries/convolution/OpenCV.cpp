/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
          const std::vector<int>& anchorVector
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
        this->translateAnchor(anchor, anchorVector, kernel.size);
        int border_type = cedar::aux::conv::BorderType::toCvConstant(borderType);
        result = this->cvConvolve(matrix, kernel, border_type, anchor);
      }
      break;

    case cedar::aux::conv::Mode::Full:
      {
        cv::Mat matrix_full = createFullMatrix(matrix, kernel, borderType);
        cv::Point anchor = cv::Point(-1, -1);
        this->translateAnchor(anchor, anchorVector, kernel.size);
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
        this->translateAnchor(anchor, anchorVector, kernel.size);
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
          cedar::aux::conv::Mode::Id mode
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
        this->translateAnchor(anchor, kernel);
        int border_type = cedar::aux::conv::BorderType::toCvConstant(borderType);
        result = this->cvConvolve(matrix, kernel, border_type, anchor);
      }
      break;
    case cedar::aux::conv::Mode::Full:
      {
        cv::Mat matrix_full = createFullMatrix(matrix, kernel, borderType);
        cv::Point anchor = cv::Point(-1, -1);
        this->translateAnchor(anchor, kernel);

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
          this->translateAnchor(anchor, kernel);
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
          cedar::aux::conv::Mode::Id mode
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
        this->translateAnchor(anchor, kernel);
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
          this->translateAnchor(anchor, kernel);
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
            this->translateAnchor(anchor, kernel);
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

cv::Mat cedar::aux::conv::OpenCV::convolve
        (
          const cv::Mat& matrix,
          const cedar::aux::kernel::ConstSeparablePtr kernel,
          cedar::aux::conv::BorderType::Id borderType,
          cedar::aux::conv::Mode::Id mode
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

        this->translateAnchor(anchor, kernel);
        return cvConvolve(matrix, kernel, border_type, anchor);
      }
      break;
    case cedar::aux::conv::Mode::Full:
      {
        cv::Mat matrix_full = createFullMatrix(matrix, kernel, borderType);

        int border_type = cedar::aux::conv::BorderType::toCvConstant(borderType);

        this->translateAnchor(anchor, kernel);
        cv::Mat result_full = cvConvolve(matrix_full, kernel, border_type, anchor);
        return cutOutResult(result_full,matrix,kernel);
      }
      break;
    case cedar::aux::conv::Mode::Valid:
      {
//        cv::Mat kernel_rows_cols = kernel->getRowsCols();
        const cv::Mat& kernel_mat = kernel->getKernel();
        if (matrix.rows < kernel_mat.rows || matrix.cols < kernel_mat.cols)
        {
          return cv::Mat::zeros(1,1,matrix.type());
        }
        else
        {
          int border_type = cedar::aux::conv::BorderType::toCvConstant(borderType);

          this->translateAnchor(anchor, kernel);
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
      cv::Mat kernel_mat = kernel->getKernelPart(0);
      if (matrix.rows == 1 && kernel_mat.rows != 1)
      {
        kernel_mat = kernel_mat.t();
      }
      if (matrix.cols == 1 && kernel_mat.cols != 1)
      {
        kernel_mat = kernel_mat.t();
      }
      cv::Mat flipped;
      cv::flip(kernel_mat, flipped, -1);
      convolved = this->cvConvolve(matrix, flipped, cvBorderType, anchor);
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
              this->translateAnchor(anchor, kernel);

              convolved = this->cvConvolve(matrix, kernel, cv_border_type, anchor);
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
              convolved = this->cvConvolve(matrix, kernel_mat, cv_border_type, anchor);
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
                this->translateAnchor(anchor, kernel);

                convolved = this->cvConvolve(matrix_full, kernel, cv_border_type, anchor);
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
                convolved = this->cvConvolve(matrix_full, kernel_mat, cv_border_type, anchor);
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
                  this->translateAnchor(anchor, kernel);

                  convolved = this->cvConvolve(matrix, kernel, cv_border_type, anchor);
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
                  convolved = this->cvConvolve(matrix, kernel_mat, cv_border_type, anchor);
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
  this->Engine::setKernelList(kernelList);
  this->mKernelTypes.clear();
  for (size_t i = 0; i < this->getKernelList()->size(); ++i)
  {
    this->updateKernelType(i);
  }
  this->getKernelList()->connectToKernelAddedSignal(boost::bind(&cedar::aux::conv::OpenCV::updateKernelType, this, _1));
  this->getKernelList()->connectToKernelChangedSignal(boost::bind(&cedar::aux::conv::OpenCV::updateKernelType, this, _1));
  this->getKernelList()->connectToKernelRemovedSignal(boost::bind(&cedar::aux::conv::OpenCV::kernelRemoved, this, _1));
}
