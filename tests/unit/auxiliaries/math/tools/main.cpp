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
    Date:        2010 11 11

    Description: Tests all math tools.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/math/constants.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>

int testMinMax(int cvMatType)
{
  int errors = 0;

  cv::Mat identity = cv::Mat::eye(3, 3, cvMatType);

  double min_val, max_val;
  min_val = cedar::aux::math::min(identity);
  max_val = cedar::aux::math::max(identity);
  if (min_val != 0.0)
  {
    std::cout << "error in min(const cv::Mat" << cvMatType << ")" << std::endl;
    errors++;
  }
  if (max_val != 1.0)
  {
    std::cout << "error in max(const cv::Mat" << cvMatType << ")" << std::endl;
    errors++;
  }

  return errors;
}

int test1dMinMaxIndices()
{
  int errors = 0;
  int min_index = 2;
  int max_index = 4;

  cv::Mat matrix = cv::Mat::ones(5, 1, CV_32F);

  matrix.at<float>(min_index) = 0;
  matrix.at<float>(max_index) = 2;

  int read_min_index = static_cast<int>(cedar::aux::math::minIndex1D(matrix));
  if (read_min_index != min_index)
  {
    std::cout << "Error: got the wrong min index. Got: " << read_min_index << ", expected: " << min_index
        << ". Matrix is: " << matrix << std::endl;
    ++errors;
  }

  int read_max_index = static_cast<int>(cedar::aux::math::maxIndex1D(matrix));
  if (read_max_index != max_index)
  {
    std::cout << "Error: got the wrong max index. Got: " << read_max_index << ", expected: " << max_index
        << ". Matrix is: " << matrix << std::endl;
    ++errors;
  }

  return errors;
}

int main()
{
  // the number of errors encountered in this test
  int errors = 0;
  // the current test number
  int test_number = 0;

  // test stuff
  errors += testMinMax(CV_8U);
  errors += testMinMax(CV_32F);
  errors += testMinMax(CV_64F);

  std::cout << "test no" << test_number++ << std::endl;
  if (cedar::aux::math::normalizeAngle(5.9) <= -cedar::aux::math::pi
      || cedar::aux::math::normalizeAngle(5.9) > cedar::aux::math::pi)
  {
    std::cout << "error in normalizeAngle(double)" << std::endl;
    errors++;
  }
  if (cedar::aux::math::normalizeAngle(-cedar::aux::math::pi) <= -cedar::aux::math::pi
      || cedar::aux::math::normalizeAngle(-cedar::aux::math::pi) > cedar::aux::math::pi)
  {
    std::cout << "error in normalizeAngle(double)" << std::endl;
    errors++;
  }
  if (cedar::aux::math::normalizeAngle(10000) <= -cedar::aux::math::pi
      || cedar::aux::math::normalizeAngle(10000) > cedar::aux::math::pi)
  {
    std::cout << "error in normalizeAngle(double)" << std::endl;
    errors++;
  }
  // test getDimensionalityOf
  cv::Mat matrix_0D(1, 1, CV_32F);
  unsigned int dim = cedar::aux::math::getDimensionalityOf(matrix_0D);
  if (dim != 0)
  {
    std::cout << "error in getDimensionalityOf(cv::Mat), expected dimensionality 0, got " << dim << std::endl;
    ++errors;
  }
  cv::Mat matrix_1D(11, 1, CV_32F);
  dim = cedar::aux::math::getDimensionalityOf(matrix_1D);
  if (dim != 1)
  {
    std::cout << "error in getDimensionalityOf(cv::Mat), expected dimensionality 1, got " << dim << std::endl;
    ++errors;
  }
  dim = cedar::aux::math::getDimensionalityOf(matrix_1D.t());
  if (dim != 1)
  {
    std::cout << "error in getDimensionalityOf(cv::Mat), expected dimensionality 1, got " << dim << std::endl;
    ++errors;
  }
  cv::Mat matrix_2D(11, 30, CV_32F);
  dim = cedar::aux::math::getDimensionalityOf(matrix_2D);
  if (dim != 2)
  {
    std::cout << "error in getDimensionalityOf(cv::Mat), expected dimensionality 2, got " << dim << std::endl;
    ++errors;
  }

  errors += test1dMinMaxIndices();

  std::cout << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
