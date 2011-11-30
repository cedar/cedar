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

#ifdef CEDAR_FFTW
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
    result_fourier[xyz][0] = (matrix_fourier[xyz][0]*kernel_fourier[xyz][0]-matrix_fourier[xyz][1]*kernel_fourier[xyz][1])  / number_of_elements;
    result_fourier[xyz][1] = (matrix_fourier[xyz][1]*kernel_fourier[xyz][0]+matrix_fourier[xyz][0]*kernel_fourier[xyz][1])  / number_of_elements;
  }

  // transform interaction back to time domain (ifft)
  fftw_execute(matrix_plan_backward);

  fftw_free(matrix_fourier);
  fftw_free(result_fourier);
  fftw_free(kernel_fourier);
  return matrix;
}

cv::Mat cedar::aux::conv::FastConvolution::padKernel(const cv::Mat& matrix, const cv::Mat& kernel) const
{
  //!@todo do some asserting here
  cv::Mat output = matrix.clone();
  output = 0.0;
  std::vector<cv::Range> region;
  for (size_t dim = 0; dim < cedar::aux::math::getDimensionalityOf(matrix); ++dim)
  {
    int kernel_center = kernel.size[dim]/2;
    int adjusted_start = matrix.size[dim] / 2 - kernel_center;
    if (matrix.size[dim] % 2 != 0)
    {
      ++adjusted_start;
    }
    region.push_back(cv::Range(adjusted_start, adjusted_start + kernel.size[dim]));
  }
  output(&region.at(0)) = kernel;
  return output;
}
#endif // CEDAR_FFTW
