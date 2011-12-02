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


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/convolution/Convolution.h"
#ifdef CEDAR_FFTW
#include "cedar/auxiliaries/convolution/FastConvolution.h"
#endif
#include "cedar/auxiliaries/LogFile.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>

using namespace cedar::aux;

int main()
{
  LogFile log_file("UnitTestConvolution.log");
  log_file.addTimeStamp();
  log_file << std::endl;
  // the number of errors encountered in this test
  int errors = 0;
  // the current test number
  int test_number = 0;

  // test stuff
  log_file << "test no" << test_number++ << std::endl;
  cedar::aux::conv::FastConvolution my_convolution;
  cv::Mat matrix = cv::Mat::ones(50,71, CV_64F);
  cv::Mat kernel = cv::Mat::ones(7,7, CV_64F);
  cv::Mat result = my_convolution(matrix, kernel);
  // test 3D
  int sizes_3D[3] = {31, 20, 57};
  int sizes_kernel_3D[3] = {5, 4, 16};
  cv::Mat matrix_3D(3, sizes_3D, CV_64F);
  cv::Mat kernel_3D(3, sizes_kernel_3D, CV_64F);
  cv::Mat result_3D = my_convolution(matrix_3D, kernel_3D);
  // test 4D
  int sizes_4D[4] = {11, 20, 37, 12};
  int sizes_kernel_4D[4] = {5, 4, 16, 7};
  cv::Mat matrix_4D(4, sizes_4D, CV_64F);
  cv::Mat kernel_4D(4, sizes_kernel_4D, CV_64F);
  cv::Mat result_4D = my_convolution(matrix_4D, kernel_4D);

  log_file << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
