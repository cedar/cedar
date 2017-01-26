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

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 16

    Description: Test convolution engine classes.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "cedar/auxiliaries/convolution/Engine.h"
#include "cedar/auxiliaries/convolution/OpenCV.h"
#include "cedar/auxiliaries/convolution/FFTW.h"
#include "cedar/auxiliaries/kernel/Kernel.h"
#include "cedar/auxiliaries/kernel/Separable.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/kernel/Gauss.h"
#include "cedar/configuration.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <iostream>
#include <iomanip>


//!@todo Test each convolve method in the engine interface
//!@todo Test anchoring

// some matrices used throughout

// 1x1 matrix containing number 1
cv::Mat numbers_0d;
// 3x1 matrix containing all numbers from 0 to 3
cv::Mat numbers_1d;
// 3x3 matrix containing all numbers from 0 to 8
cv::Mat numbers_2d;

cv::Mat numbers_even_2d;


class DemoKernel : public cedar::aux::kernel::Kernel
{
  public:
    DemoKernel(const cv::Mat& matrix) : cedar::aux::kernel::Kernel(cedar::aux::math::getDimensionalityOf(matrix))
    {
      this->mKernel->setData(matrix);
    }

    void calculate()
    {
      // nothing to do -- matrix is fixed.
    }
};

CEDAR_GENERATE_POINTER_TYPES(DemoKernel);

class DemoKernel0D : public cedar::aux::kernel::Kernel
{
  public:
    DemoKernel0D(double amplitude) : cedar::aux::kernel::Kernel(0)
    {
      this->mKernel->setData(cv::Mat::ones(1, 1, CV_32F) * amplitude);
    }

    void calculate()
    {
      // nothing to do -- matrix is fixed.
    }
};

CEDAR_GENERATE_POINTER_TYPES(DemoKernel0D);

class DemoSeparable0D : public cedar::aux::kernel::Separable
{
  public:
  DemoSeparable0D(double amplitude)
    :
    Separable(0)
    {
      this->setKernelPart(0, cv::Mat::ones(1, 1, CV_32F) * amplitude);

      this->updateKernelMatrix();
    }

    void calculateParts()
    {
      // nothing to do -- matrix is fixed.
    }
};

CEDAR_GENERATE_POINTER_TYPES(DemoSeparable0D);

class DemoSeparable1D : public cedar::aux::kernel::Separable
{
  public:
    DemoSeparable1D(const cv::Mat& matrix)
    :
    Separable(1)
    {
      this->setKernelPart(0, matrix);

      this->updateKernelMatrix();
    }

    void calculateParts()
    {
      // nothing to do -- matrix is fixed.
    }
};

CEDAR_GENERATE_POINTER_TYPES(DemoSeparable1D);

class DemoSeparable2D : public cedar::aux::kernel::Separable
{
  public:
    DemoSeparable2D(const cv::Mat& matrix1, const cv::Mat& matrix2)
    :
    Separable(2)
    {
      this->setKernelPart(0, matrix1);
      this->setKernelPart(1, matrix2);

      this->updateKernelMatrix();
    }

    void calculateParts()
    {
      // nothing to do -- matrix is fixed.
    }
};

CEDAR_GENERATE_POINTER_TYPES(DemoSeparable2D);

// Collection of information for a matrix x kernel test.
struct TestSetMatKernel
{
  TestSetMatKernel
  (
    cv::Mat matrix,
    cedar::aux::conv::BorderType::Id borderType,
    cedar::aux::conv::Mode::Id mode,
    const std::string& specialName = std::string()
  )
  :
  mBorderType(borderType),
  mMode(mode),
  mMatrix(matrix),
  mSpecialName(specialName)
  {
  }

  cedar::aux::conv::BorderType::Id mBorderType;
  cedar::aux::conv::Mode::Id mMode;
  cv::Mat mMatrix;
  std::string mSpecialName;
};

void printMatrix(cv::Mat m, std::string name = "", int setW = 5)
{
  // if name is given print name
  if(!name.empty())
  {
    std::cout << name << std::endl;
  }
  // print matrix
  for(int r = 0; r < m.rows; ++r)
  {
    for(int c = 0; c < m.cols; ++c)
    {
      std::cout << std::setw(setW) << m.at<float>(r, c);
    }
    std::cout << std::endl;
  }
}

// this function makes sure that the modulus is always >= 0
inline int pos_mod(int v1, int m)
{
  CEDAR_DEBUG_ASSERT(((v1 + m) % m) >= 0);
  return (v1 + m) % m;
}

inline int reflect(int value, int size)
{
  while (value >= size || value < 0)
  {
    if (value < 0)
    {
      value = -(value + 1);
    }
    if (value >= size)
    {
      value = 2 * size - value - 1;
    }
  }

  CEDAR_DEBUG_ASSERT(value >= 0);
  CEDAR_DEBUG_ASSERT(value < size);
  return value;
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

    case cedar::aux::conv::BorderType::Reflect:
    {
      int r = reflect(row, mat.rows);
      int c = reflect(col, mat.cols);
      return mat.at<float>(r, c);
    }

    case cedar::aux::conv::BorderType::Zero:
    {
      if (row >= mat.rows || row < 0)
      {
        return 0.0;
      }
      if (col >= mat.cols || col < 0)
      {
        return 0.0;
      }

      return mat.at<float>(row, col);
    }

    default:
      std::cout << "Error: border type not implemented in unit test!" << std::endl;
      return 0.0;
  }
}

cv::Mat conv
        (
          const cv::Mat& op1,
          const cv::Mat& op2,
          cedar::aux::conv::BorderType::Id borderType,
          cedar::aux::conv::Mode::Id mode
        )
{
  if (mode == cedar::aux::conv::Mode::Same)
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
  else if (mode == cedar::aux::conv::Mode::Full)
  {
    cv::Mat result = cv::Mat::zeros(op1.rows + op2.rows - 1, op1.cols + op2.cols - 1, CV_32F);

    for (int row = 0; row < result.rows; ++row)
    {
      for (int col = 0; col < result.cols; ++col)
      {
        for (int k_row = 0; k_row < op2.rows; ++k_row)
        {
          for (int k_col = 0; k_col < op2.cols; ++k_col)
          {
            result.at<float>(row, col)
              += border_interpolate(op1, row - k_row, col - k_col, borderType)
                 * op2.at<float>(k_row, k_col);
          }
        }
      }
    }

    return result;
  }
  else if (mode == cedar::aux::conv::Mode::Valid)
  {
    if (op2.rows > op1.rows || op2.cols > op1.cols)
    {
      return cv::Mat::zeros(1,1,CV_32F);
    }
    else
    {
      cv::Mat result = cv::Mat::zeros(op1.rows - op2.rows + 1, op1.cols - op2.cols + 1, CV_32F);

      cv::Mat op2_flipped;
      cv::flip(op2, op2_flipped, -1);

      for (int row = 0; row < result.rows; ++row)
      {
        for (int col = 0; col < result.cols; ++col)
        {
          for (int k_row = 0; k_row < op2_flipped.rows; ++k_row)
          {
            for (int k_col = 0; k_col < op2_flipped.cols; ++k_col)
            {
              result.at<float>(row, col)
                += op1.at<float>(row + k_row, col + k_col)
                   * op2_flipped.at<float>(k_row, k_col);
            }
          }
        }
      }
      return result;
    }
  }
  else
  {
    std::cout << "ERROR!\n\tConvolution mode unknown!" << std::endl;
    return 3.33 * op1;
  }
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

void test_matxmat_convolution
    (
      cedar::aux::conv::EnginePtr engine,
      const cv::Mat& op1,
      const cv::Mat& op2,
      cedar::aux::conv::BorderType::Id borderType,
      cedar::aux::conv::Mode::Id mode,
      int& ok, int& skipped, int& errors
    )
{
  // check if the engine is capable of this operation
  if
  (
    !engine->checkCapability
    (
      cedar::aux::math::getDimensionalityOf(op1),
      cedar::aux::math::getDimensionalityOf(op2),
      borderType,
      mode
    )
  )
  {
    std::cout << "SKIPPED --- Engine is not capable of this operation." << std::endl;
    skipped += 1;
    return;
  }

  // if so, do it
  cv::Mat res = engine->convolve(op1, op2, borderType, mode);
  cv::Mat expected = conv(op1, op2, borderType, mode);


  if (!mat_eq(res, expected))
  {
    std::cout << "ERROR." << std::endl;

    // display the operation
    printMatrix(op1);
    std::cout << " * " << std::endl;
    printMatrix(op2);
    std::cout << " = (engine result:)" << std::endl;
    printMatrix(res);
    std::cout << " ?= (expected result:)" << std::endl;
    printMatrix(expected);

    errors += 1;
    return;
  }

  std::cout << "OK" << std::endl;
  ok += 1;
}

void testMatrixKernelListOperation
     (
       cedar::aux::conv::EnginePtr engine,
       const cv::Mat& mat,
       cedar::aux::conv::BorderType::Id borderType,
       cedar::aux::conv::Mode::Id mode,
       int& ok, int& skipped, int& errors
     )
{
  cedar::aux::conv::KernelListPtr kernel_list = engine->getKernelList();

  // check if the engine is capable of this operation
  if
  (
    !engine->checkCapability
    (
      cedar::aux::math::getDimensionalityOf(mat),
      cedar::aux::math::getDimensionalityOf(kernel_list->getCombinedKernel()),
      borderType,
      mode
    )
  )
  {
    std::cout << "SKIPPED --- Engine is not capable of this operation." << std::endl;
    skipped += 1;
    return;
  }

  cv::Mat kernel_mat = kernel_list->getKernel(0)->getKernel();
  cv::Mat expected = conv(mat, kernel_mat, borderType, mode);

  for (size_t i = 1; i < kernel_list->size(); ++i)
  {
    kernel_mat = kernel_list->getKernel(i)->getKernel();
    expected += conv(mat, kernel_mat, borderType, mode);
  }

  cv::Mat result = engine->convolve(mat, borderType, mode);

  if (!mat_eq(expected, result))
  {
    std::cout << "ERROR:" << std::endl;
    std::cout << "----------------------------" << std::endl;
    // display the operation
    printMatrix(mat);
    std::cout << " * " << std::endl;
    printMatrix(kernel_list->getCombinedKernel());
    std::cout << " = (engine result:)" << std::endl;
    printMatrix(result);
    std::cout << " ?= (expected result:)" << std::endl;
    printMatrix(expected);
    std::cout << "----------------------------" << std::endl;

    errors += 1;
    return;
  }

  std::cout << "OK" << std::endl;
  ok += 1;
}

void testMatrixKernelListOperation
     (
       cedar::aux::conv::EnginePtr engine,
       const cv::Mat& mat,
       cedar::aux::conv::ConstKernelListPtr kernelList,
       cedar::aux::conv::BorderType::Id borderType,
       cedar::aux::conv::Mode::Id mode,
       int& ok, int& skipped, int& errors
     )
{
  // check if the engine is capable of this operation
  if
  (
    !engine->checkCapability
    (
      cedar::aux::math::getDimensionalityOf(mat),
      cedar::aux::math::getDimensionalityOf(kernelList->getCombinedKernel()),
      borderType,
      mode
    )
  )
  {
    std::cout << "SKIPPED --- Engine is not capable of this operation." << std::endl;
    skipped += 1;
    return;
  }

  cv::Mat kernel_mat = kernelList->getKernel(0)->getKernel();
  cv::Mat expected = conv(mat, kernel_mat, borderType, mode);

  for (size_t i = 1; i < kernelList->size(); ++i)
  {
    kernel_mat = kernelList->getKernel(i)->getKernel();
    expected += conv(mat, kernel_mat, borderType, mode);
  }

  cv::Mat result = engine->convolve(mat, kernelList, borderType, mode);

  if (!mat_eq(expected, result))
  {
    std::cout << "ERROR:" << std::endl;
    std::cout << "----------------------------" << std::endl;
    // display the operation
    printMatrix(mat);
    std::cout << " * " << std::endl;
    printMatrix(kernelList->getCombinedKernel());
    std::cout << " = (engine result:)" << std::endl;
    printMatrix(result);
    std::cout << " ?= (expected result:)" << std::endl;
    printMatrix(expected);
    std::cout << "----------------------------" << std::endl;

    errors += 1;
    return;
  }

  std::cout << "OK" << std::endl;
  ok += 1;
  return;
}

template <typename T> // T = cedar::aux::conv::{KernelPtr, SeparablePtr}
void testMatrixKernelOperation
     (
       cedar::aux::conv::EnginePtr engine,
       const cv::Mat& mat,
       T kernel,
       cedar::aux::conv::BorderType::Id borderType,
       cedar::aux::conv::Mode::Id mode,
       int& ok, int& skipped, int& errors
     )
{
  cv::Mat kernel_mat = kernel->getKernel();
  // check if the engine is capable of this operation
  if
  (
    !engine->checkCapability
    (
      cedar::aux::math::getDimensionalityOf(mat),
      kernel->getDimensionality(),
      borderType,
      mode
    )
  )
  {
    std::cout << "SKIPPED --- Engine is not capable of this operation." << std::endl;
    skipped += 1;
    return;
  }

  cv::Mat expected = conv(mat, kernel_mat, borderType, mode);
  cv::Mat result = engine->convolve(mat, kernel, borderType, mode);

  if (!mat_eq(expected, result))
  {
    std::cout << "ERROR:" << std::endl;

    // display the operation
    printMatrix(mat);
    std::cout << " * " << std::endl;
    printMatrix(kernel_mat);
    std::cout << " = (engine result:)" << std::endl;
    printMatrix(result);
    std::cout << " ?= (expected result:)" << std::endl;
    printMatrix(expected);

    errors += 1;
    return;
  }

  std::cout << "OK" << std::endl;
  ok += 1;
  return;
}

template <typename T> // T = cedar::aux::conv::{KernelPtr, SeparablePtr}
void testMatrixKernelOperations
     (
       cedar::aux::conv::EnginePtr engine,
       T kernel,
       int& ok, int& skipped, int& errors
     )
{
  std::cout << "-----------------------------------------------------------------------------" << std::endl;
  std::cout << "                                          Testing matrix * kernel operations." << std::endl;
  std::cout << "-----------------------------------------------------------------------------" << std::endl;

  std::vector<TestSetMatKernel> test_sets;

  // automatically generated test-cases
  const size_t dims_size = 3;
  cv::Mat dims[dims_size] = {numbers_0d, numbers_1d, numbers_2d};
  for (size_t border_type_i = 0; border_type_i < cedar::aux::conv::BorderType::type().list().size(); ++border_type_i)
  {
    cedar::aux::conv::BorderType::Id border_type = cedar::aux::conv::BorderType::type().list().at(border_type_i);
    for (size_t dim_i = 0; dim_i < dims_size; ++dim_i)
    {
      for (size_t mode_type_i = 0; mode_type_i < cedar::aux::conv::Mode::type().list().size(); ++mode_type_i)
      {
        cedar::aux::conv::Mode::Id mode = cedar::aux::conv::Mode::type().list().at(mode_type_i);
        test_sets.push_back(TestSetMatKernel(dims[dim_i], border_type, mode));
      }
    }
  }

  // hand-crafted test-cases
  test_sets.push_back(TestSetMatKernel(cv::Mat::zeros(3, 3, CV_32F), cedar::aux::conv::BorderType::Zero, cedar::aux::conv::Mode::Valid));
  test_sets.push_back(TestSetMatKernel(cv::Mat::ones(4, 4, CV_32F), cedar::aux::conv::BorderType::Zero, cedar::aux::conv::Mode::Valid, "even"));
  test_sets.push_back(TestSetMatKernel(cv::Mat::ones(1, 1, CV_32F), cedar::aux::conv::BorderType::Zero, cedar::aux::conv::Mode::Valid, "void"));

  // execute test-cases
  for (auto iter = test_sets.begin(); iter != test_sets.end(); ++iter)
  {
    const TestSetMatKernel& test_set = *iter;
    std::cout << "(" << cedar::aux::conv::BorderType::type().get(test_set.mBorderType).prettyString()
        << " " << cedar::aux::math::getDimensionalityOf(test_set.mMatrix) << "D, "
        << cedar::aux::conv::Mode::type().get(test_set.mMode).prettyString();
    if (!test_set.mSpecialName.empty())
    {
      std::cout << " " << test_set.mSpecialName;
    }
    std::cout << ") ... ";
    testMatrixKernelOperation
    (
      engine,
      test_set.mMatrix,
      kernel,
      test_set.mBorderType,
      test_set.mMode,
      ok, skipped, errors
    );

  }
}

void testMatrixKernelOperations
     (
       cedar::aux::conv::EnginePtr engine,
       cedar::aux::conv::KernelListPtr& kernelList,
       int& ok, int& skipped, int& errors
     )
{
  std::cout << "-----------------------------------------------------------------------------" << std::endl;
  std::cout << "                 Testing matrix * kernel list operations. (passed on the fly)" << std::endl;
  std::cout << "-----------------------------------------------------------------------------" << std::endl;


  std::vector<TestSetMatKernel> test_sets;

  // automatically generated test-cases
  const size_t dims_size = 3;
  cv::Mat dims[dims_size] = {numbers_0d, numbers_1d, numbers_2d};
  for (size_t border_type_i = 0; border_type_i < cedar::aux::conv::BorderType::type().list().size(); ++border_type_i)
  {
    cedar::aux::conv::BorderType::Id border_type = cedar::aux::conv::BorderType::type().list().at(border_type_i);
    for (size_t dim_i = 0; dim_i < dims_size; ++dim_i)
    {
      for (size_t mode_type_i = 0; mode_type_i < cedar::aux::conv::Mode::type().list().size(); ++mode_type_i)
      {
        cedar::aux::conv::Mode::Id mode = cedar::aux::conv::Mode::type().list().at(mode_type_i);
        test_sets.push_back(TestSetMatKernel(dims[dim_i], border_type, mode));
      }
    }
  }

  // hand-crafted test-cases
  test_sets.push_back(TestSetMatKernel(cv::Mat::zeros(3, 3, CV_32F), cedar::aux::conv::BorderType::Zero, cedar::aux::conv::Mode::Valid));
  test_sets.push_back(TestSetMatKernel(cv::Mat::ones(4, 4, CV_32F), cedar::aux::conv::BorderType::Zero, cedar::aux::conv::Mode::Valid, "even"));
  test_sets.push_back(TestSetMatKernel(cv::Mat::ones(1, 1, CV_32F), cedar::aux::conv::BorderType::Zero, cedar::aux::conv::Mode::Valid, "void"));

  // execute test-cases
  for (auto iter = test_sets.begin(); iter != test_sets.end(); ++iter)
  {
    const TestSetMatKernel& test_set = *iter;
    std::cout << "(" << cedar::aux::conv::BorderType::type().get(test_set.mBorderType).prettyString()
        << " " << cedar::aux::math::getDimensionalityOf(test_set.mMatrix) << "D, "
        << cedar::aux::conv::Mode::type().get(test_set.mMode).prettyString();
    if (!test_set.mSpecialName.empty())
    {
      std::cout << " " << test_set.mSpecialName;
    }
    std::cout << ") ... ";
    testMatrixKernelListOperation
    (
      engine,
      test_set.mMatrix,
      kernelList,
      test_set.mBorderType,
      test_set.mMode,
      ok, skipped, errors
    );

  }


  std::cout << "-----------------------------------------------------------------------------" << std::endl;
  std::cout << "                 Testing matrix * kernel list operations. (passed beforehand)" << std::endl;
  std::cout << "-----------------------------------------------------------------------------" << std::endl;

  engine->setKernelList(kernelList);

  // execute test-cases
  for (auto iter = test_sets.begin(); iter != test_sets.end(); ++iter)
  {
    const TestSetMatKernel& test_set = *iter;
    std::cout << "(" << cedar::aux::conv::BorderType::type().get(test_set.mBorderType).prettyString()
        << " " << cedar::aux::math::getDimensionalityOf(test_set.mMatrix) << "D, "
        << cedar::aux::conv::Mode::type().get(test_set.mMode).prettyString();
    if (!test_set.mSpecialName.empty())
    {
      std::cout << " " << test_set.mSpecialName;
    }
    std::cout << ") ... ";
    testMatrixKernelListOperation
    (
      engine,
      test_set.mMatrix,
      test_set.mBorderType,
      test_set.mMode,
      ok, skipped, errors
    );
  }
}

void testMatrixKernelOperations(cedar::aux::conv::EnginePtr engine, int& errors, int& skipped, int& ok)
{
  std::cout << "-----------------------------------------------------------------------------" << std::endl;
  std::cout << "                Testing matrix * kernel operations (non-separable, symmetric)" << std::endl;
  std::cout << "-----------------------------------------------------------------------------" << std::endl;
  {
    DemoKernel0DPtr demo_kernel_0d(new DemoKernel0D(2.0));
    testMatrixKernelOperations(engine, demo_kernel_0d, errors, skipped, ok);

    DemoKernelPtr demo_kernel_1d(new DemoKernel(cv::Mat::ones(3, 1, CV_32F)));
    testMatrixKernelOperations(engine, demo_kernel_1d, errors, skipped, ok);

    DemoKernelPtr demo_kernel_2d(new DemoKernel(cv::Mat::ones(3, 3, CV_32F)));
    testMatrixKernelOperations(engine, demo_kernel_2d, errors, skipped, ok);

  }

  std::cout << "-----------------------------------------------------------------------------" << std::endl;
  std::cout << "               Testing matrix * kernel operations (non-separable, asymmetric)" << std::endl;
  std::cout << "-----------------------------------------------------------------------------" << std::endl;
  {
    DemoKernelPtr demo_kernel_1d(new DemoKernel(numbers_1d));
    testMatrixKernelOperations(engine, demo_kernel_1d, errors, skipped, ok);

    DemoKernelPtr demo_kernel_2d(new DemoKernel(numbers_2d));
    testMatrixKernelOperations(engine, demo_kernel_2d, errors, skipped, ok);
  }

  std::cout << "-----------------------------------------------------------------------------" << std::endl;
  std::cout << "                    Testing matrix * kernel operations (separable, symmetric)" << std::endl;
  std::cout << "-----------------------------------------------------------------------------" << std::endl;
  {
    DemoSeparable0DPtr demo_kernel_0d(new DemoSeparable0D(2.0));
    testMatrixKernelOperations(engine, demo_kernel_0d, errors, skipped, ok);

    cv::Mat ones = cv::Mat::ones(3, 1, CV_32F);
    DemoSeparable1DPtr demo_kernel_1d(new DemoSeparable1D(ones));
    testMatrixKernelOperations(engine, demo_kernel_1d, errors, skipped, ok);

    DemoSeparable2DPtr demo_kernel_2d(new DemoSeparable2D(ones, ones));
    testMatrixKernelOperations(engine, demo_kernel_2d, errors, skipped, ok);

    cedar::aux::kernel::GaussPtr gauss(new cedar::aux::kernel::Gauss(2.0, 100.0, 0.0, 10.0, 1));
    testMatrixKernelOperations(engine, gauss, errors, skipped, ok);
  }

  std::cout << "-----------------------------------------------------------------------------" << std::endl;
  std::cout << "                   Testing matrix * kernel operations (separable, asymmetric)" << std::endl;
  std::cout << "-----------------------------------------------------------------------------" << std::endl;
  {
    DemoSeparable1DPtr demo_kernel_1d(new DemoSeparable1D(numbers_1d));
    testMatrixKernelOperations(engine, demo_kernel_1d, errors, skipped, ok);

    DemoSeparable2DPtr demo_kernel_2d(new DemoSeparable2D(numbers_1d, numbers_1d));
    testMatrixKernelOperations(engine, demo_kernel_2d, errors, skipped, ok);
  }

  std::cout << "-----------------------------------------------------------------------------" << std::endl;
  std::cout << "                        Testing matrix * kernel operations (using KernelList)" << std::endl;
  std::cout << "-----------------------------------------------------------------------------" << std::endl;
  {
    cv::Mat ones = cv::Mat::ones(3, 1, CV_32F);

    DemoKernel0DPtr demo_kernel_0d(new DemoKernel0D(2.0));
    DemoKernelPtr demo_kernel_1d_ones(new DemoKernel(cv::Mat::ones(3, 1, CV_32F)));
    DemoKernelPtr demo_kernel_2d_ones(new DemoKernel(cv::Mat::ones(3, 3, CV_32F)));
    DemoKernelPtr demo_kernel_1d(new DemoKernel(numbers_1d));
    DemoKernelPtr demo_kernel_2d(new DemoKernel(numbers_2d));
    DemoSeparable0DPtr demo_kernel_sep_0d(new DemoSeparable0D(2.0));
    DemoSeparable1DPtr demo_kernel_sep_1d_ones(new DemoSeparable1D(ones));
    DemoSeparable2DPtr demo_kernel_sep_2d_ones(new DemoSeparable2D(ones, ones));
    DemoSeparable1DPtr demo_kernel_sep_1d(new DemoSeparable1D(numbers_1d));
    DemoSeparable2DPtr demo_kernel_sep_2d(new DemoSeparable2D(numbers_1d, numbers_1d));


    cedar::aux::conv::KernelListPtr kernel_list_0d(new cedar::aux::conv::KernelList());
    cedar::aux::conv::KernelListPtr kernel_list_1d(new cedar::aux::conv::KernelList());
    cedar::aux::conv::KernelListPtr kernel_list_2d(new cedar::aux::conv::KernelList());

    kernel_list_0d->append(demo_kernel_0d);
    kernel_list_0d->append(demo_kernel_sep_0d);

    kernel_list_1d->append(demo_kernel_1d_ones);
    kernel_list_1d->append(demo_kernel_1d);
    kernel_list_1d->append(demo_kernel_sep_1d_ones);
    kernel_list_1d->append(demo_kernel_sep_1d);

    kernel_list_2d->append(demo_kernel_2d_ones);
    kernel_list_2d->append(demo_kernel_2d);
    kernel_list_2d->append(demo_kernel_sep_2d_ones);
    kernel_list_2d->append(demo_kernel_sep_2d);

    testMatrixKernelOperations(engine, kernel_list_0d, errors, skipped, ok);
    testMatrixKernelOperations(engine, kernel_list_1d, errors, skipped, ok);
    testMatrixKernelOperations(engine, kernel_list_2d, errors, skipped, ok);
  }
}

void testMatrixMatrixOperations(cedar::aux::conv::EnginePtr engine, int& ok, int& skipped, int& errors)
{
  std::cout << "-----------------------------------------------------------------------------" << std::endl;
  std::cout << "                                          Testing matrix * matrix operations." << std::endl;
  std::cout << "-----------------------------------------------------------------------------" << std::endl;
  {
    cv::Mat op1, op2;
    op1 = cv::Mat::zeros(5, 1, CV_32F);
    op1.at<float>(3) = 1;
    op2 = cv::Mat::zeros(3, 1, CV_32F);
    op2.at<float>(0) = 0.5;
    op2.at<float>(1) = 1;
    op2.at<float>(2) = 0.5;

    std::cout << "(Replicate, Same) ... ";
    test_matxmat_convolution
              (
                engine,
                op1,
                op2,
                cedar::aux::conv::BorderType::Replicate,
                cedar::aux::conv::Mode::Same,
                ok, skipped, errors
              );

    std::cout << "(Replicate, Full) ... ";
    test_matxmat_convolution
              (
                engine,
                op1,
                op2,
                cedar::aux::conv::BorderType::Replicate,
                cedar::aux::conv::Mode::Full,
                ok, skipped, errors
              );
  }

  {
    cv::Mat op1, op2;
    op1 = cv::Mat::zeros(3, 3, CV_32F);
    op1.at<float>(0, 2) = 1;
    op1.at<float>(1, 2) = 1;
    op2 = cv::Mat::zeros(3, 3, CV_32F);
    op2.at<float>(1, 2) = 0.5;
    op2.at<float>(0, 0) = 0.5;

    std::cout << "(Reflect, Same) ... ";
    test_matxmat_convolution
    (
      engine,
      op1,
      op2,
      cedar::aux::conv::BorderType::Reflect,
      cedar::aux::conv::Mode::Same,
      ok, skipped, errors
    );

    std::cout << "(Reflect, Full) ... ";
    test_matxmat_convolution
    (
      engine,
      op1,
      op2,
      cedar::aux::conv::BorderType::Reflect,
      cedar::aux::conv::Mode::Full,
      ok, skipped, errors
    );
  }

  {
    std::cout << "(Zero, Same) ... ";
    test_matxmat_convolution
    (
      engine,
      cv::Mat::ones(3, 3, CV_32F),
      cv::Mat::ones(3, 3, CV_32F),
      cedar::aux::conv::BorderType::Zero,
      cedar::aux::conv::Mode::Same,
      ok, skipped, errors
    );

    std::cout << "(Zero, Full) ... ";
    test_matxmat_convolution
    (
      engine,
      cv::Mat::ones(3, 3, CV_32F),
      cv::Mat::ones(3, 3, CV_32F),
      cedar::aux::conv::BorderType::Zero,
      cedar::aux::conv::Mode::Full,
      ok, skipped, errors
    );
  }

  {
    std::cout << "(Zero II, Same) ... ";
    test_matxmat_convolution
    (
      engine,
      cv::Mat::zeros(3, 3, CV_32F),
      cv::Mat::ones(3, 3, CV_32F),
      cedar::aux::conv::BorderType::Zero,
      cedar::aux::conv::Mode::Same,
      ok, skipped, errors
    );

    std::cout << "(Zero II, Full) ... ";
    test_matxmat_convolution
    (
      engine,
      cv::Mat::zeros(3, 3, CV_32F),
      cv::Mat::ones(3, 3, CV_32F),
      cedar::aux::conv::BorderType::Zero,
      cedar::aux::conv::Mode::Full,
      ok, skipped, errors
    );
  }

  {
    std::cout << "(Cyclic 1D, Same) ... ";
    test_matxmat_convolution
    (
      engine,
      numbers_1d,
      cv::Mat::ones(3, 1, CV_32F),
      cedar::aux::conv::BorderType::Cyclic,
      cedar::aux::conv::Mode::Same,
      ok, skipped, errors
    );

    std::cout << "(Cyclic 1D, Full) ... ";
    test_matxmat_convolution
    (
      engine,
      numbers_1d,
      cv::Mat::ones(3, 1, CV_32F),
      cedar::aux::conv::BorderType::Cyclic,
      cedar::aux::conv::Mode::Full,
      ok, skipped, errors
    );
  }

  {
    std::cout << "(Cyclic 2D, Same) ... ";
    test_matxmat_convolution
    (
      engine,
      numbers_2d,
      cv::Mat::ones(3, 3, CV_32F),
      cedar::aux::conv::BorderType::Cyclic,
      cedar::aux::conv::Mode::Same,
      ok, skipped, errors
    );

    std::cout << "(Cyclic 2D, Full) ... ";
    test_matxmat_convolution
    (
      engine,
      numbers_2d,
      cv::Mat::ones(3, 3, CV_32F),
      cedar::aux::conv::BorderType::Cyclic,
      cedar::aux::conv::Mode::Full,
      ok, skipped, errors
    );
  }

  {
    cv::Mat op1, op2;
    op1 = cv::Mat::zeros(3, 3, CV_32F);
    op1.at<float>(0, 2) = 1;
    op1.at<float>(1, 2) = 1;
    op2 = cv::Mat::zeros(3, 3, CV_32F);
    op2.at<float>(1, 2) = 0.5;
    op2.at<float>(0, 0) = 0.5;

    std::cout << "(Valid 2D zero) ... ";
    test_matxmat_convolution
    (
      engine,
      cv::Mat::zeros(3, 3, CV_32F),
      cv::Mat::zeros(3, 3, CV_32F),
      cedar::aux::conv::BorderType::Zero,
      cedar::aux::conv::Mode::Valid,
      ok, skipped, errors
    );

    std::cout << "(Valid 2D) ... ";
    test_matxmat_convolution
    (
      engine,
      op1,
      op2,
      cedar::aux::conv::BorderType::Zero,
      cedar::aux::conv::Mode::Valid,
      ok, skipped, errors
    );

    std::cout << "(Valid 2D even) ... ";
    test_matxmat_convolution
    (
      engine,
      cv::Mat::ones(4, 4, CV_32F),
      numbers_2d,
      cedar::aux::conv::BorderType::Zero,
      cedar::aux::conv::Mode::Valid,
      ok, skipped, errors
    );

    std::cout << "(Valid 2D void) ... ";
    test_matxmat_convolution
    (
      engine,
      op1,
      cv::Mat::ones(4, 4, CV_32F),
      cedar::aux::conv::BorderType::Zero,
      cedar::aux::conv::Mode::Valid,
      ok, skipped, errors
    );


  }

  {
    cv::Mat op1, op2;
    op1 = cv::Mat::zeros(5, 1, CV_32F);
    op1.at<float>(3) = 1;
    op2 = cv::Mat::zeros(3, 1, CV_32F);
    op2.at<float>(0) = 0.5;
    op2.at<float>(1) = 1;
    op2.at<float>(2) = 0.5;

    std::cout << "(Valid 1D) ... ";
    test_matxmat_convolution
    (
      engine,
      op1,
      op2,
      cedar::aux::conv::BorderType::Zero,
      cedar::aux::conv::Mode::Valid,
      ok, skipped, errors
    );
  }

  {
    cv::Mat op1, op2;
    op1 = cv::Mat::zeros(3, 3, CV_32F);
    op1.at<float>(0, 2) = 1;
    op1.at<float>(1, 2) = 1;
    op2 = cv::Mat::zeros(3, 1, CV_32F);
    op2.at<float>(0) = 0.5;
    op2.at<float>(1) = 1;
    op2.at<float>(2) = 0.5;

    std::cout << "(Valid m2Dk1D) ... ";
    test_matxmat_convolution
    (
      engine,
      op1,
      op2,
      cedar::aux::conv::BorderType::Zero,
      cedar::aux::conv::Mode::Valid,
      ok, skipped, errors
    );
  }
}


int testEngine(cedar::aux::conv::EnginePtr engine)
{
  std::cout << "=============================================================================" << std::endl;
  std::cout << "Testing engine of type " << cedar::aux::objectTypeToString(engine) << std::endl;
  std::cout << "=============================================================================" << std::endl;

  int ok = 0;
  int skipped = 0;
  int errors = 0;
  testMatrixMatrixOperations(engine, ok, skipped, errors);
  testMatrixKernelOperations(engine, ok, skipped, errors);


  std::cout << std::endl << std::endl;
  std::cout << "=============================================================================" << std::endl;
  std::cout << "Results for engine of type " << cedar::aux::objectTypeToString(engine) << std::endl;
  std::cout << "-----------------------------------------------------------------------------" << std::endl;
  std::cout << "       Ok: " << ok << std::endl;
  std::cout << "  Skipped: " << skipped << std::endl;
  std::cout << "   Errors: " << errors << std::endl;
  std::cout << "=============================================================================" << std::endl;
  std::cout << std::endl << std::endl;
  return errors;
}

void testConv()
{
  numbers_2d = cv::Mat(5, 5, CV_32F);
  numbers_2d.at<float>(0, 0) = 0;
  numbers_2d.at<float>(0, 1) = 1;
  numbers_2d.at<float>(0, 2) = 2;
  numbers_2d.at<float>(0, 3) = 3;
  numbers_2d.at<float>(0, 4) = 4;

  numbers_2d.at<float>(1, 0) = 5;
  numbers_2d.at<float>(1, 1) = 6;
  numbers_2d.at<float>(1, 2) = 7;
  numbers_2d.at<float>(1, 3) = 8;
  numbers_2d.at<float>(1, 4) = 9;

  numbers_2d.at<float>(2, 0) = 10;
  numbers_2d.at<float>(2, 1) = 11;
  numbers_2d.at<float>(2, 2) = 12;
  numbers_2d.at<float>(2, 3) = 13;
  numbers_2d.at<float>(2, 4) = 14;

  numbers_2d.at<float>(3, 0) = 15;
  numbers_2d.at<float>(3, 1) = 16;
  numbers_2d.at<float>(3, 2) = 17;
  numbers_2d.at<float>(3, 3) = 18;
  numbers_2d.at<float>(3, 4) = 19;

  numbers_2d.at<float>(4, 0) = 20;
  numbers_2d.at<float>(4, 1) = 21;
  numbers_2d.at<float>(4, 2) = 22;
  numbers_2d.at<float>(4, 3) = 23;
  numbers_2d.at<float>(4, 4) = 24;

  cv::Mat expected = conv(numbers_2d, numbers_2d, cedar::aux::conv::BorderType::Zero, cedar::aux::conv::Mode::Full);
  printMatrix(expected);

  numbers_even_2d = cv::Mat(4, 4, CV_32F);
  numbers_even_2d.at<float>(0, 0) = 0;
  numbers_even_2d.at<float>(0, 1) = 1;
  numbers_even_2d.at<float>(0, 2) = 2;
  numbers_even_2d.at<float>(0, 3) = 3;

  numbers_even_2d.at<float>(1, 0) = 4;
  numbers_even_2d.at<float>(1, 1) = 5;
  numbers_even_2d.at<float>(1, 2) = 6;
  numbers_even_2d.at<float>(1, 3) = 7;

  numbers_even_2d.at<float>(2, 0) = 8;
  numbers_even_2d.at<float>(2, 1) = 9;
  numbers_even_2d.at<float>(2, 2) = 10;
  numbers_even_2d.at<float>(2, 3) = 11;

  numbers_even_2d.at<float>(3, 0) = 12;
  numbers_even_2d.at<float>(3, 1) = 13;
  numbers_even_2d.at<float>(3, 2) = 14;
  numbers_even_2d.at<float>(3, 3) = 15;

  cv::Mat expected_even = conv(numbers_even_2d, numbers_even_2d, cedar::aux::conv::BorderType::Zero, cedar::aux::conv::Mode::Full);
  printMatrix(expected_even);
}

int main()
{
  // initialize matrices
  numbers_2d = cv::Mat(3, 3, CV_32F);
  numbers_2d.at<float>(0, 0) = 0;
  numbers_2d.at<float>(0, 1) = 1;
  numbers_2d.at<float>(0, 2) = 2;
  numbers_2d.at<float>(1, 0) = 3;
  numbers_2d.at<float>(1, 1) = 4;
  numbers_2d.at<float>(1, 2) = 5;
  numbers_2d.at<float>(2, 0) = 6;
  numbers_2d.at<float>(2, 1) = 7;
  numbers_2d.at<float>(2, 2) = 8;


  numbers_1d = cv::Mat(3, 1, CV_32F);
  numbers_1d.at<float>(0) = 0;
  numbers_1d.at<float>(1) = 1;
  numbers_1d.at<float>(2) = 2;


  numbers_0d = cv::Mat(1, 1, CV_32F);
  numbers_0d.at<float>(0) = 1;

  // the number of errors encountered in this test
  int errors = 0;

  cedar::aux::conv::OpenCVPtr open_cv (new cedar::aux::conv::OpenCV());
  errors += testEngine(open_cv);

#ifdef CEDAR_USE_FFTW
  cedar::aux::conv::FFTWPtr fftw (new cedar::aux::conv::FFTW());
  errors += testEngine(fftw);
#endif // CEDAR_USE_FFTW

  return errors;
}
