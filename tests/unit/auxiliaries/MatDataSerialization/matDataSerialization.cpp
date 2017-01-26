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

    File:        matDataSerialization.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2015 07 13

    Description: Implements unit tests for the @em cedar::aux::MatData serialization functions.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/MatrixIterator.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/shared_ptr.hpp>
#endif
#include <string>

template <typename T>
bool checkMatrixEquality(cv::Mat a, cv::Mat b)
{
  if (a.type() != b.type())
  {
    std::cout << "  ERROR: types of matrices differ." << std::endl;
    return false;
  }

  if (a.size != b.size)
  {
    std::cout << "  ERROR: sizes of matrices differ." << std::endl;
    return false;
  }

  CEDAR_DEBUG_ASSERT(a.total() == b.total())

  if (a.dims <= 2)
  {
    for (size_t i = 0; i < a.total(); ++i)
    {
      if (a.at<T>(i) != b.at<T>(i))
      {
        std::cout << "  ERROR: contents of matrices differ." << std::endl;
        return false;
      }
    }
  }
  else
  {
    cedar::aux::MatrixIterator iter_a(a);
    cedar::aux::MatrixIterator iter_b(b);

    do
    {
      if (a.at<T>(iter_a.getCurrentIndex()) != b.at<T>(iter_b.getCurrentIndex()))
      {
        std::cout << "  ERROR: contents of matrices differ at (";
        for (size_t i = 0; i < static_cast<size_t>(a.dims); ++i)
        {
          if (i > 0)
          {
            std::cout << ", ";
          }
          std::cout << iter_a.getCurrentIndex()[i];
        }
        std::cout << ")" << std::endl;
        std::cout << "    (" << a.at<T>(iter_a.getCurrentIndex()) << " != " <<  b.at<T>(iter_b.getCurrentIndex()) << ")" << std::endl;
        return false;
      }
    }
    while (iter_a.increment() && iter_b.increment());
  }

  return true;
}

template <typename T>
int test_matrix(cv::Mat matrix)
{
  int errors = 0;

  for (const auto& mode : cedar::aux::SerializationFormat::type().list())
  {
    const auto& mode_str = mode.prettyString();
    std::cout << "Testing mode " << mode_str << std::endl;

    std::stringstream stream;
    cedar::aux::MatDataPtr write_data(new cedar::aux::MatData(matrix.clone()));
    std::cout << "Testing CSV writing" << std::endl;
    write_data->serialize(stream, mode);
    if (matrix.dims <= 2)
    {
      std::cout << "Write: " << matrix << std::endl;
    }
    cedar::aux::MatDataPtr read_data(new cedar::aux::MatData());
    read_data->deserialize(stream, mode);
    if (matrix.dims <= 2)
    {
      std::cout << "Read: " << read_data->getData() << std::endl;
    }

    if (!checkMatrixEquality<T>(matrix, read_data->getData()))
    {
      ++errors;
    }
  }
  return errors;
}

template <typename T>
void fill_matrix(cv::Mat mat)
{
  if (mat.dims <= 2)
  {
    for (int r = 0; r < mat.rows; ++r)
    {
      for (int c = 0; c < mat.cols; ++c)
      {
        mat.at<T>(r, c) = static_cast<T>(r * mat.cols + c) + static_cast<T>(0.5);
      }
    }
  }
  else
  {
    cedar::aux::MatrixIterator iter(mat);

    do
    {
      int sum = 0;
      for (size_t i = 0; i < static_cast<size_t>(mat.dims); ++i)
      {
        sum += iter.getCurrentIndex()[i];
      }
      mat.at<T>(iter.getCurrentIndex()) = static_cast<T>(sum) + static_cast<T>(0.5);
    }
    while (iter.increment());
  }
}

template <typename T>
int test_1d_matrix(int type)
{
  cv::Mat mat(4, 1, type);
  fill_matrix<T>(mat);
  return test_matrix<T>(mat);
}

template <typename T>
int test_2d_matrix(int type)
{
  cv::Mat mat(4, 4, type);
  fill_matrix<T>(mat);
  return test_matrix<T>(mat);
}

template <typename T>
int test_3d_matrix(int type)
{
  int sizes[3];
  sizes[0] = 4;
  sizes[1] = 4;
  sizes[2] = 4;
  cv::Mat mat(3, sizes, type);
  fill_matrix<T>(mat);
  return test_matrix<T>(mat);
}

//! Tests a given matrix type
template <typename T>
int test_type(int type)
{
  int errors = 0;

  errors += test_1d_matrix<T>(type);
  errors += test_2d_matrix<T>(type);
  errors += test_3d_matrix<T>(type);

  return errors;
}

int main()
{
  int errors = 0;

  errors += test_type<float>(CV_32F);

  return errors;
}
