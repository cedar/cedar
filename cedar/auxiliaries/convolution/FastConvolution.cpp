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

    File:        FastConvolution.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 11 28

    Description:

    Credits:

======================================================================================================================*/

#ifdef CEDAR_USE_FFTW

// CEDAR INCLUDES
#include "cedar/auxiliaries/convolution/FastConvolution.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
cv::Mat cedar::aux::conv::FastConvolution::convolve(const cv::Mat& matrix, const cv::Mat& kernel) const
{
  CEDAR_ASSERT(cedar::aux::math::getDimensionalityOf(matrix) == cedar::aux::math::getDimensionalityOf(kernel));
  for (unsigned int dim = 0 ; dim < cedar::aux::math::getDimensionalityOf(matrix) - 1; ++dim)
  {
    CEDAR_ASSERT(matrix.size[dim] >= kernel.size[dim])
  }
  cv::Mat output = matrix.clone();
  output = 0.0;
  cv::Mat padded_kernel = this->padKernel(matrix, kernel);
  unsigned int transformed_elements = 1;
  double number_of_elements = 1.0;
  for (unsigned int dim = 0 ; dim < cedar::aux::math::getDimensionalityOf(matrix) - 1; ++dim)
  {
    transformed_elements *= matrix.size[dim];
    number_of_elements *= matrix.size[dim];
  }
  transformed_elements *= matrix.size[cedar::aux::math::getDimensionalityOf(matrix) -1] / 2 + 1;
  number_of_elements *= matrix.size[cedar::aux::math::getDimensionalityOf(matrix) -1];
  fftw_complex *matrix_fourier = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * transformed_elements);
  fftw_complex *result_fourier = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * transformed_elements);
  fftw_complex *kernel_fourier = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * transformed_elements);
  // transform sigmoid U to frequency domain (fft)
  fftw_plan matrix_plan_forward = fftw_plan_dft_r2c(cedar::aux::math::getDimensionalityOf(matrix), matrix.size, const_cast<double*>(matrix.ptr<double>()), matrix_fourier, FFTW_FORWARD + FFTW_PATIENT);
  fftw_plan kernel_plan_forward = fftw_plan_dft_r2c(cedar::aux::math::getDimensionalityOf(padded_kernel), padded_kernel.size, const_cast<double*>(padded_kernel.ptr<double>()), kernel_fourier, FFTW_FORWARD + FFTW_PATIENT);
  fftw_plan matrix_plan_backward =  fftw_plan_dft_c2r(cedar::aux::math::getDimensionalityOf(matrix), matrix.size, result_fourier, const_cast<double*>(output.ptr<double>()), FFTW_BACKWARD + FFTW_PATIENT);
  fftw_execute(matrix_plan_forward);
  fftw_execute(kernel_plan_forward);

  // this is the simplest indexing method compared to the other ones
  for (unsigned int xyz = 0 ; xyz < transformed_elements; ++xyz)
  {
    // complex multiplication (lateral)
//    result_fourier[xyz][0] = (matrix_fourier[xyz][0]*kernel_fourier[xyz][0]-matrix_fourier[xyz][1]*kernel_fourier[xyz][1]) / number_of_elements;
//    result_fourier[xyz][1] = (matrix_fourier[xyz][1]*kernel_fourier[xyz][0]+matrix_fourier[xyz][0]*kernel_fourier[xyz][1]) / number_of_elements;
    result_fourier[xyz][0] = (kernel_fourier[xyz][0]*matrix_fourier[xyz][0]-kernel_fourier[xyz][1]*matrix_fourier[xyz][1]) / number_of_elements;
    result_fourier[xyz][1] = (kernel_fourier[xyz][1]*matrix_fourier[xyz][0]+kernel_fourier[xyz][0]*matrix_fourier[xyz][1]) / number_of_elements;
  }

  // transform interaction back to time domain (ifft)
  fftw_execute(matrix_plan_backward);

  fftw_free(matrix_fourier);
  fftw_free(result_fourier);
  fftw_free(kernel_fourier);
  return output;
}

cv::Mat cedar::aux::conv::FastConvolution::padKernel(const cv::Mat& matrix, const cv::Mat& kernel) const
{
  //!@todo do some asserting here
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
    regions.at(0).push_back(cv::Range(0, kernel_center + 1)); // lower limit
    regions.at(1).push_back(cv::Range(matrix.size[dim] - kernel_center, matrix.size[dim])); // upper limit
    kernel_regions.at(0).push_back(cv::Range(0, kernel_center)); // lower limit
    kernel_regions.at(1).push_back(cv::Range(kernel_center, kernel.size[dim])); // upper limit
  }
  for (size_t part = 0; part < static_cast<unsigned int>((1 << cedar::aux::math::getDimensionalityOf(matrix))); ++part)
  {
    cv::Range output_index[cedar::aux::math::getDimensionalityOf(matrix)];
    cv::Range kernel_index[cedar::aux::math::getDimensionalityOf(matrix)];
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
  // if 1.0 is missing, the temporary cv::Mat view onto output is replaced by kernel, instead of setting the values
  // at the specified region to the values of kernel (1.0 * kernel returns a cv::MatExpr, not cv::Mat...)
    output(output_index) = 1.0 * kernel(kernel_index);
  }
  return output;
}

#endif // CEDAR_USE_FFTW
