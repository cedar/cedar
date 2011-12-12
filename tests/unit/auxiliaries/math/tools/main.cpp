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

using namespace cedar::aux;
using namespace cedar::aux::math;

int main()
{
  // the number of errors encountered in this test
  int errors = 0;
  // the current test number
  int test_number = 0;

  // test stuff
  std::cout << "test no" << test_number++ << std::endl;
  cv::Mat identity = cv::Mat::eye(3, 3, CV_64FC1);
  std::cout << "this should be the 3x3 identity matrix:" << std::endl;
  write(identity);

  double min_val, max_val;
  min_val = min(identity);
  max_val = max(identity);
  if (min_val!=0.0)
  {
    std::cout<<"error in min(const cv::Mat)"<<std::endl;
    errors++;
  }
  if ( max_val!=1.0)
   {
     std::cout<<"error in max(const cv::Mat)"<<std::endl;
     errors++;
   }
  std::cout << "test no" << test_number++ << std::endl;
  if (normalizeAngle(5.9) <= -cedar::aux::math::pi || normalizeAngle(5.9) > cedar::aux::math::pi)
  {
    std::cout << "error in normalizeAngle(double)" << std::endl;
    errors++;
  }
  if (normalizeAngle(-cedar::aux::math::pi) <= -cedar::aux::math::pi || normalizeAngle(-cedar::aux::math::pi) > cedar::aux::math::pi)
  {
    std::cout << "error in normalizeAngle(double)" << std::endl;
    errors++;
  }
  if (normalizeAngle(10000) <= -cedar::aux::math::pi || normalizeAngle(10000) > cedar::aux::math::pi)
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
  }
  cv::Mat matrix_1D(11, 1, CV_32F);
  dim = cedar::aux::math::getDimensionalityOf(matrix_1D);
  if (dim != 0)
  {
    std::cout << "error in getDimensionalityOf(cv::Mat), expected dimensionality 1, got " << dim << std::endl;
  }
  dim = cedar::aux::math::getDimensionalityOf(matrix_1D.t());
  if (dim != 0)
  {
    std::cout << "error in getDimensionalityOf(cv::Mat), expected dimensionality 1, got " << dim << std::endl;
  }
  cv::Mat matrix_2D(11, 30, CV_32F);
  dim = cedar::aux::math::getDimensionalityOf(matrix_2D);
  if (dim != 0)
  {
    std::cout << "error in getDimensionalityOf(cv::Mat), expected dimensionality 2, got " << dim << std::endl;
  }

  // test round


  std::cout << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
