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

    File:        main.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@rub.de
    Date:        2011 11 28

    Description: Test convolution classes.

    Credits:

======================================================================================================================*/

#include "cedar/configuration.h"

#ifdef CEDAR_USE_FFTW

// LOCAL INCLUDES
#include "cedar/auxiliaries/convolution/Convolution.h"
#include "cedar/auxiliaries/convolution/FFTW.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>

//!@todo Disable compilation of this test in the CMakeLists.txt when CEDAR_USE_FFTW is not defined

int main()
{
  // the number of errors encountered in this test
  int errors = 0;
  // the current test number
  int test_number = 0;
  cedar::aux::conv::FFTWPtr fftw (new cedar::aux::conv::FFTW());
  // test stuff
  std::cout << "test no " << test_number++ << std::endl;
  cv::Mat matrix = cv::Mat::ones(50,71, CV_64F);
  cv::Mat kernel = cv::Mat::ones(7,7, CV_64F);
  cv::Mat result = fftw->convolve(matrix, kernel, cedar::aux::conv::BorderType::Cyclic);
  // test 3D
  int sizes_3D[3] = {31, 20, 57};
  int sizes_kernel_3D[3] = {5, 4, 16};
  cv::Mat matrix_3D(3, sizes_3D, CV_64F);
  cv::Mat kernel_3D(3, sizes_kernel_3D, CV_64F);
  cv::Mat result_3D = fftw->convolve(matrix_3D, kernel_3D, cedar::aux::conv::BorderType::Cyclic);
  // test 4D
  int sizes_4D[4] = {11, 20, 17, 12};
  int sizes_kernel_4D[4] = {5, 4, 6, 7};
  cv::Mat matrix_4D(4, sizes_4D, CV_64F);
  cv::Mat kernel_4D(4, sizes_kernel_4D, CV_64F);
  cv::Mat result_4D = fftw->convolve(matrix_4D, kernel_4D, cedar::aux::conv::BorderType::Cyclic);
  // test 5D
  int sizes_5D[5] = {13, 17, 12, 12, 10};
  int sizes_kernel_5D[5] = {5, 4, 6, 7, 6};
  cv::Mat matrix_5D(5, sizes_5D, CV_64F);
  cv::Mat kernel_5D(5, sizes_kernel_5D, CV_64F);
  cv::Mat result_5D = fftw->convolve(matrix_5D, kernel_5D, cedar::aux::conv::BorderType::Cyclic);

  std::cout << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
#endif // CEDAR_USE_FFTW
