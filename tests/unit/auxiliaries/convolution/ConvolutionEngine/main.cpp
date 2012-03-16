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

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 16

    Description: Test convolution engine classes.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/convolution/Engine.h"
#include "cedar/auxiliaries/convolution/OpenCV.h"
#include "cedar/auxiliaries/utilities.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>

// this function makes sure that the modulus is always >= 0
inline int pos_mod(int v1, int m)
{
  CEDAR_DEBUG_ASSERT(((v1 + m) % m) >= 0);
  return (v1 + m) % m;
}

float border_interpolate(const cv::Mat& mat, int row, int col, cedar::aux::conv::BorderType::Id borderType)
{
  switch (borderType)
  {
    case cedar::aux::conv::BorderType::Cyclic:
      return mat.at<float>(pos_mod(row, mat.rows), pos_mod(col, mat.cols));

    case cedar::aux::conv::BorderType::Replicate:
    {
      int r = std::min(std::max(row, 0), mat.rows - 1);
      int c = std::min(std::max(col, 0), mat.cols - 1);
      return mat.at<float>(r, c);
    }

    default:
      std::cout << "Error: border type not implemented in unit test!" << std::endl;
      return 0.0;
  }
}

cv::Mat conv(const cv::Mat& op1, const cv::Mat& op2, cedar::aux::conv::BorderType::Id borderType)
{
  cv::Mat result = 0.0 * op1;
  int offset_r = op2.rows / 2;
  int offset_c = op2.cols / 2;
  for (int row = 0; row < result.rows; ++row)
  {
    for (int col = 0; col < result.cols; ++col)
    {
      for (int k_row = 0; k_row < op2.rows; ++k_row)
      {
        for (int k_col = 0; k_col < op2.cols; ++k_col)
        {
          result.at<float>(row, col)
            += border_interpolate(op1, row - k_row + offset_r, col - k_col + offset_c, borderType)
               * op2.at<float>(k_row, k_col);
        }
      }
    }
  }

  return result;
}

bool mat_eq(const cv::Mat& mat1, const cv::Mat& mat2)
{
  if (mat1.rows != mat2.rows || mat1.cols != mat2.cols)
  {
    return false;
  }

  for (int r = 0; r < mat1.rows; ++r)
  {
    for (int c = 0; c < mat1.cols; ++c)
    {
      if (mat1.at<float>(r, c) != mat2.at<float>(r, c))
      {
        return false;
      }
    }
  }

  return true;
}

int testEngine(cedar::aux::conv::EnginePtr engine)
{
  //!@todo Test anchoring
  //!@todo Test that the kernels are mirrored properly
  //!@todo Write a kernel type that allows passing arbitrary matrices to better test everything
  //!@todo Test the different border types
  //!@todo Test each convolve method in the engine interface

  int errors = 0;

  std::cout << "Testing engine of type " << cedar::aux::objectTypeToString(engine) << std::endl;

  {
    std::cout << "Convolving two matrices ..." << std::endl;

    cv::Mat op1, op2;
    op1 = cv::Mat::zeros(5, 1, CV_32F);
    op1.at<float>(3) = 1;
    op2 = cv::Mat::zeros(3, 1, CV_32F);
    op2.at<float>(0) = 0.5;
    op2.at<float>(1) = 1;
    op2.at<float>(2) = 0.5;

    cv::Mat res = engine->convolve(op1, op2, cedar::aux::conv::BorderType::Replicate);
    cv::Mat expected = conv(op1, op2, cedar::aux::conv::BorderType::Replicate);
    std::cout << op1 << " * " << op2 << " = " << res << std::endl;
    std::cout << "comparing to: " << expected << std::endl;

    if (!mat_eq(res, expected))
    {
      ++errors;
      std::cout << "ERROR." << std::endl;
    }
  }

  return errors;
}


int main()
{
  // the number of errors encountered in this test
  int errors = 0;

  cedar::aux::conv::OpenCVPtr open_cv (new cedar::aux::conv::OpenCV());
  errors += testEngine(open_cv);

  return errors;
}
