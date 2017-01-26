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

template <typename T>
int testZeroness()
{
  int errors = 0;

  T first = static_cast<T>(0.0);
  if (!cedar::aux::math::isZero(first))
  {
    std::cout << "error in isZero():" << __LINE__ << std::endl;
    errors++;
  }

  T second = static_cast<T>(1e-10);
  if (!cedar::aux::math::isZero(second))
  {
    std::cout << "error in isZero():" << __LINE__ << std::endl;
    errors++;
  }
  if (!cedar::aux::math::isZero(-second))
  {
    std::cout << "error in isZero():" << __LINE__ << std::endl;
    errors++;
  }

  T third = static_cast<T>(1e-8);
  if (cedar::aux::math::isZero(third))
  {
    std::cout << "error in isZero():" << __LINE__ << std::endl;
    errors++;
  }
  
  if (cedar::aux::math::isZero(-third))
  {
    std::cout << "error in isZero():" << __LINE__ << std::endl;
    errors++;
  }

  return errors;
}


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

//!@todo Write this for 2d, nd access functions as well
template <typename MAT_T, typename INT_T, int CV_TYPE>
int testMatrixAccess1D()
{
  int errors = 0;

  std::cout << "Testing assignMatrixEntry() function..." << std::endl;

  cv::Mat mat = cv::Mat::zeros(2, 1, CV_TYPE);
  // generate a binary 11111...
  MAT_T value = static_cast<MAT_T>(static_cast<INT_T>(-1));
  cedar::aux::math::assignMatrixEntry(mat, 0, value);
  cedar::aux::math::assignMatrixEntry(mat, 1, value);

  for (int i = 0; i < mat.rows; ++i)
  {
    if (mat.at<MAT_T>(i) != value)
    {
      std::cout << "Wrong value written to index " << i
          << "; expected: " << value << ", got: " << mat.at<MAT_T>(i) << std::endl;
      ++errors;
    }

    MAT_T read = cedar::aux::math::getMatrixEntry<MAT_T>(mat, i);
    if (read != value)
    {
      std::cout << "Wrong value read at index " << i
          << "; expected: " << value << ", got: " << read << std::endl;
      ++errors;
    }
  }

  std::cout << "              ... done." << std::endl;

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

  errors += testMatrixAccess1D<uint8_t, uint8_t, CV_8U>();
  errors += testMatrixAccess1D<uint16_t, uint16_t, CV_16U>();
  errors += testMatrixAccess1D<float, uint32_t, CV_32F>();
  errors += testMatrixAccess1D<double, uint64_t, CV_64F>();

  std::cout << "test no " << test_number++ << std::endl;
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

  errors += testZeroness<float>();
  errors += testZeroness<double>();

  std::cout << "test finished with " << errors << " error(s)." << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
