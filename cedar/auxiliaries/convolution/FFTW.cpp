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

    File:        FFTW.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 11 28

    Description:

    Credits:

======================================================================================================================*/

#include "cedar/configuration.h"

#ifdef CEDAR_USE_FFTW

// CEDAR INCLUDES
#include "cedar/auxiliaries/convolution/FFTW.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/kernel/Kernel.h"
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/Singleton.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register type with the factory
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered
    = cedar::aux::conv::EngineManagerSingleton::getInstance()->registerType<cedar::aux::conv::FFTWPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
cv::Mat cedar::aux::conv::FFTW::convolve
(
  const cv::Mat& matrix,
  cedar::aux::conv::BorderType::Id borderType,
  cedar::aux::conv::Mode::Id /* mode */
) const
{
  return this->convolveInternal(matrix, this->getKernelList()->getCombinedKernel(), borderType);
}

cv::Mat cedar::aux::conv::FFTW::convolve
(
  const cv::Mat& matrix,
  const cv::Mat& kernel,
  cedar::aux::conv::BorderType::Id borderType,
  cedar::aux::conv::Mode::Id /* mode */,
  const std::vector<int>& /* anchor */
) const
{
  return this->convolveInternal(matrix, kernel, borderType);
}

cv::Mat cedar::aux::conv::FFTW::convolve
(
  const cv::Mat& matrix,
  cedar::aux::kernel::ConstKernelPtr kernel,
  cedar::aux::conv::BorderType::Id borderType,
  cedar::aux::conv::Mode::Id /* mode */
) const
{
  return this->convolveInternal(matrix, kernel->getKernel(), borderType);
}

cv::Mat cedar::aux::conv::FFTW::convolve
(
  const cv::Mat& matrix,
  const cedar::aux::conv::KernelList& kernel,
  cedar::aux::conv::BorderType::Id borderType,
  cedar::aux::conv::Mode::Id /* mode */
) const
{
  return this->convolveInternal(matrix, kernel.getCombinedKernel(), borderType);
}

cv::Mat cedar::aux::conv::FFTW::convolveInternal
        (
          const cv::Mat& matrix,
          const cv::Mat& kernel,
          cedar::aux::conv::BorderType::Id /* borderType */
        ) const
{
  if (cedar::aux::math::getDimensionalityOf(kernel) == 0)
  {
    return matrix * cedar::aux::math::getMatrixEntry<double>(kernel, 0, 0);
  }
  else if (cedar::aux::math::getDimensionalityOf(matrix) == 0)
  {

    return cv::Mat(1, 1, kernel.type(), cv::sum(kernel * cedar::aux::math::getMatrixEntry<double>(matrix, 0, 0)));
  }
  for (unsigned int dim = 0 ; dim < cedar::aux::math::getDimensionalityOf(matrix) - 1; ++dim)
  {
    CEDAR_ASSERT(matrix.size[dim] >= kernel.size[dim])
  }

  cv::Mat matrix_64;
  cv::Mat kernel_64;
  if (matrix.type() == CV_32F)
  {
    matrix.convertTo(matrix_64, CV_64F);
  }
  else
  {
    CEDAR_ASSERT(matrix.type() == CV_64F);
    matrix_64 = matrix;
  }

  if (kernel.type() == CV_32F)
  {
    kernel.convertTo(kernel_64, CV_64F);
  }
  else
  {
    CEDAR_ASSERT(kernel.type() == CV_64F);
    kernel_64 = kernel;
  }

  cv::Mat output = matrix_64.clone();
  output = 0.0;
  cv::Mat padded_kernel = this->padKernel(matrix_64, kernel_64);

  unsigned int transformed_elements = 1;
  double number_of_elements = 1.0;
  for (unsigned int dim = 0 ; dim < cedar::aux::math::getDimensionalityOf(matrix_64) - 1; ++dim)
  {
    transformed_elements *= matrix_64.size[dim];
    number_of_elements *= matrix_64.size[dim];
  }
  transformed_elements *= matrix_64.size[cedar::aux::math::getDimensionalityOf(matrix_64) -1] / 2 + 1;
  number_of_elements *= matrix_64.size[cedar::aux::math::getDimensionalityOf(matrix_64) -1];
  fftw_complex *matrix_fourier = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * transformed_elements);
  fftw_complex *result_fourier = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * transformed_elements);
  fftw_complex *kernel_fourier = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * transformed_elements);
  // transform sigmoid U to frequency domain (fft)
  fftw_plan matrix_plan_forward = fftw_plan_dft_r2c(cedar::aux::math::getDimensionalityOf(matrix_64), matrix_64.size, const_cast<double*>(matrix_64.clone().ptr<double>()), matrix_fourier, FFTW_FORWARD + FFTW_ESTIMATE);
  fftw_plan kernel_plan_forward = fftw_plan_dft_r2c(cedar::aux::math::getDimensionalityOf(padded_kernel), padded_kernel.size, const_cast<double*>(padded_kernel.clone().ptr<double>()), kernel_fourier, FFTW_FORWARD + FFTW_ESTIMATE);
  fftw_plan matrix_plan_backward = fftw_plan_dft_c2r(cedar::aux::math::getDimensionalityOf(matrix_64), matrix_64.size, result_fourier, const_cast<double*>(output.clone().ptr<double>()), FFTW_BACKWARD + FFTW_ESTIMATE);

  matrix_plan_forward = fftw_plan_dft_r2c(cedar::aux::math::getDimensionalityOf(matrix_64), matrix_64.size, const_cast<double*>(matrix_64.ptr<double>()), matrix_fourier, FFTW_FORWARD + FFTW_ESTIMATE);
  kernel_plan_forward = fftw_plan_dft_r2c(cedar::aux::math::getDimensionalityOf(padded_kernel), padded_kernel.size, const_cast<double*>(padded_kernel.ptr<double>()), kernel_fourier, FFTW_FORWARD + FFTW_ESTIMATE);
  matrix_plan_backward = fftw_plan_dft_c2r(cedar::aux::math::getDimensionalityOf(matrix_64), matrix_64.size, result_fourier, const_cast<double*>(output.ptr<double>()), FFTW_BACKWARD + FFTW_ESTIMATE);
  fftw_execute(matrix_plan_forward);
  fftw_execute(kernel_plan_forward);

  // this is the simplest indexing method compared to the other ones
  for (unsigned int xyz = 0; xyz < transformed_elements; ++xyz)
  {
    // complex multiplication (lateral)
    result_fourier[xyz][0] = (kernel_fourier[xyz][0] * matrix_fourier[xyz][0] - kernel_fourier[xyz][1] * matrix_fourier[xyz][1]) / number_of_elements;
    result_fourier[xyz][1] = (kernel_fourier[xyz][1] * matrix_fourier[xyz][0] + kernel_fourier[xyz][0] * matrix_fourier[xyz][1]) / number_of_elements;
  }

  // transform interaction back to time domain (ifft)
  fftw_execute(matrix_plan_backward);

  fftw_free(matrix_fourier);
  fftw_free(result_fourier);
  fftw_free(kernel_fourier);

  if (matrix.type() == CV_32F)
  {
    cv::Mat output_32;
    output.convertTo(output_32, CV_32F);
    return output_32;
  }
  else
  {
    return output;
  }
}

cv::Mat cedar::aux::conv::FFTW::padKernel(const cv::Mat& matrix, const cv::Mat& kernel) const
{
  /* prepare the kernel for Fourier transform (pad to matrix size and flip); example for 2D:
   * 010    211
   * 121 => 100 anchor moves to 0,0
   * 010    001
   */

  cv::Mat output = matrix.clone();
  output = 0.0;
  std::vector<cv::Range> region;
  std::vector<std::vector<cv::Range> > regions;
  regions.resize(2);
  std::vector<std::vector<cv::Range> > kernel_regions;
  kernel_regions.resize(2);
  for (size_t dim = 0; dim < cedar::aux::math::getDimensionalityOf(matrix); ++dim)
  {
    int kernel_center = kernel.size[dim]/2;
    regions.at(0).push_back(cv::Range(0, kernel_center + (kernel.size[dim] % 2))); // lower limit
    if (dim >= cedar::aux::math::getDimensionalityOf(kernel)) // fix for strange kernels (kind of a ridge padding)
    {
      regions.at(1).push_back(cv::Range(matrix.size[dim] - 1, matrix.size[dim])); // upper limit
      kernel_regions.at(0).push_back(cv::Range(0, 1)); // lower limit
    }
    else
    {
      regions.at(1).push_back(cv::Range(matrix.size[dim] - kernel_center, matrix.size[dim])); // upper limit
      kernel_regions.at(0).push_back(cv::Range(0, kernel_center)); // lower limit
    }

    kernel_regions.at(1).push_back(cv::Range(kernel_center, kernel.size[dim])); // upper limit
  }
  for (size_t part = 0; part < static_cast<unsigned int>((1 << cedar::aux::math::getDimensionalityOf(matrix))); ++part)
  {
    unsigned int dim_0_fix = 0;
    if (cedar::aux::math::getDimensionalityOf(matrix) == 1)
    {
      dim_0_fix = 1;
    }
    cv::Range output_index[cedar::aux::math::getDimensionalityOf(matrix) + dim_0_fix];
    cv::Range kernel_index[cedar::aux::math::getDimensionalityOf(matrix) + dim_0_fix];
    for (size_t dim = 0; dim < cedar::aux::math::getDimensionalityOf(matrix); ++dim)
    {
      if (part & (1 << dim))
      {
        output_index[dim] = regions.at(0).at(dim);
        kernel_index[dim] = kernel_regions.at(1).at(dim);
      }
      else
      {
        output_index[dim] = regions.at(1).at(dim);
        kernel_index[dim] = kernel_regions.at(0).at(dim);
      }
    }
    if (cedar::aux::math::getDimensionalityOf(matrix) == 1)
    {
      output_index[1] = cv::Range(0,1);
      kernel_index[1] = cv::Range(0,1);
    }
  // if 1.0 is missing, the temporary cv::Mat view onto output is replaced by kernel, instead of setting the values
  // at the specified region to the values of kernel (1.0 * kernel returns a cv::MatExpr, not cv::Mat...)
    output(output_index) = 1.0 * kernel(kernel_index);
  }
  return output;
}

bool cedar::aux::conv::FFTW::checkCapability
     (
       size_t matrixDim,
       size_t kernelDim,
       cedar::aux::conv::BorderType::Id borderType,
       cedar::aux::conv::Mode::Id mode
     )
     const
{
  if (matrixDim != kernelDim)
  {
    return false;
  }

  if (!this->checkBorderTypeCapability(borderType) || !this->checkModeCapability(mode))
  {
    return false;
  }

  return true;
}

bool cedar::aux::conv::FFTW::checkBorderTypeCapability
     (
       cedar::aux::conv::BorderType::Id borderType
     ) const
{
  switch (borderType)
  {
    case cedar::aux::conv::BorderType::Cyclic:
      return true;

    default:
      return false;
  }
}

bool cedar::aux::conv::FFTW::checkModeCapability
     (
       cedar::aux::conv::Mode::Id mode
     ) const
{
  // currently, only the full same is supported
  return mode == cedar::aux::conv::Mode::Same;
}

#endif // CEDAR_FFTW