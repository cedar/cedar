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

    File:        tools.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 11

    Description: Math tools

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>
#include <math.h>

using namespace std;

double cedar::aux::math::normalizeAngle(double value)
{
  while (value <= -cedar::aux::math::pi)
  {
    value += 2 * cedar::aux::math::pi;
  }
  while (value > cedar::aux::math::pi)
  {
    value -= 2 * cedar::aux::math::pi;
  }
  return value;
}

double cedar::aux::math::max(const cv::Mat matrix)
{
  double max;
  cv::minMaxLoc(matrix, 0 , &max);
  return max;
}

double cedar::aux::math::min(const cv::Mat matrix)
{
  double min;
  cv::minMaxLoc(matrix, &min );
  return min;
}

template <typename T>
T cedar::aux::math::round(T val)
{
  return std::floor(val + static_cast<T>(0.5));
}

template CEDAR_AUX_LIB_EXPORT float cedar::aux::math::round<float>(float val);
template CEDAR_AUX_LIB_EXPORT double cedar::aux::math::round<double>(double val);

void cedar::aux::math::write(cv::Mat matrix)
{
  switch (matrix.type())
  {
  case CV_8U:
    for(int i = 0; i < matrix.rows; i++)
    {
      for (int j = 0; j < matrix.cols; j++)
      {
        if (IsZero(matrix.at<unsigned char>(i, j)))
        {
          cout << "0" << " ";
        }
        else
        {
          cout << matrix.at<unsigned char>(i, j) << " ";
        }
      }
      cout << "\n";
    }
  break;
  case CV_8S:
    for(int i = 0; i < matrix.rows; i++)
    {
      for (int j = 0; j < matrix.cols; j++)
      {
        if (IsZero(matrix.at<char>(i, j)))
        {
          cout << "0" << " ";
        }
        else
        {
          cout << matrix.at<char>(i, j) << " ";
        }
      }
      cout << "\n";
    }
  break;
  case CV_16U:
      for(int i = 0; i < matrix.rows; i++)
      {
        for (int j = 0; j < matrix.cols; j++)
        {
          if (IsZero(matrix.at<unsigned short int>(i, j)))
          {
            cout << "0" << " ";
          }
          else
          {
            cout << matrix.at<unsigned short int>(i, j) << " ";
          }
        }
        cout << "\n";
      }
    break;
  case CV_16S:
      for(int i = 0; i < matrix.rows; i++)
      {
        for (int j = 0; j < matrix.cols; j++)
        {
          if (IsZero(matrix.at<short int>(i, j)))
          {
            cout << "0" << " ";
          }
          else
          {
            cout << matrix.at<short int>(i, j) << " ";
          }
        }
        cout << "\n";
      }
    break;
  case CV_32S:
      for(int i = 0; i < matrix.rows; i++)
      {
        for (int j=0; j<matrix.cols; j++)
        {
          if (IsZero(matrix.at<int>(i, j)))
          {
            cout << "0" << " ";
          }
          else
          {
            cout << matrix.at<int>(i, j) << " ";
          }
        }
        cout << "\n";
      }
    break;
  case CV_32F:
    for(int i = 0; i < matrix.rows; i++)
    {
      for (int j=0; j<matrix.cols; j++)
      {
        if (IsZero(matrix.at<float>(i, j)))
        {
          cout << "0" << " ";
        }
        else
        {
          cout << matrix.at<float>(i, j) << " ";
        }
      }
      cout << "\n";
    }
  break;
  case CV_64F:
  for(int i = 0; i < matrix.rows; i++)
  {
    for (int j=0; j<matrix.cols; j++)
    {
      if (IsZero(matrix.at<double>(i, j)))
      {
        cout << "0" << " ";
      }
      else
      {
        cout << matrix.at<double>(i, j) << " ";
      }
    }
    cout << "\n";
  }
  default:
    break;
  }
  cout << "\n";
}

cv::Mat cedar::aux::math::convolve(const cv::Mat& matrix, const cv::Mat& kernel)
{
  cv::Mat result;

  CEDAR_ASSERT(matrix.dims == kernel.dims);

  switch (matrix.dims)
  {
    case 1:
    case 2:
      cv::filter2D(matrix, result, -1, kernel, cv::Point(-1, -1), 0.0, cv::BORDER_CONSTANT);
      break;

    default:
      CEDAR_THROW(cedar::aux::UnhandledValueException, "Cannot convolve functions of the given dimensionality.");
  }

  return result;
}

template <typename T>
void cedar::aux::math::reduceCvMat3D(const cv::Mat& source, cv::Mat& destination, int dimensionToReduce, int reductionOperator)
{
  int source_dimension_size_0 = source.size[0];
  int source_dimension_size_1 = source.size[1];
  int source_dimension_size_2 = source.size[2];

  double sum;
  double min;
  double max;

  switch (dimensionToReduce)
  {
    case 0:
    {
      switch (reductionOperator)
      {
        case (CV_REDUCE_SUM):
        {
          for (int dim_1 = 0; dim_1 < source_dimension_size_1; ++dim_1)
          {
            for (int dim_2 = 0; dim_2 < source_dimension_size_2; ++dim_2)
            {
              sum = 0;
              for (int dim_0 = 0; dim_0 < source_dimension_size_0; ++dim_0)
              {
                sum += source.at<T>(dim_0, dim_1, dim_2);
              }
              destination.at<T>(dim_1, dim_2) = sum;
            }
          }
          break;
        }
        case (CV_REDUCE_AVG):
        {
          for (int dim_1 = 0; dim_1 < source_dimension_size_1; ++dim_1)
          {
            for (int dim_2 = 0; dim_2 < source_dimension_size_2; ++dim_2)
            {
              sum = 0;
              for (int dim_0 = 0; dim_0 < source_dimension_size_0; ++dim_0)
              {
                sum += source.at<T>(dim_0, dim_1, dim_2);
              }
              destination.at<T>(dim_1, dim_2) = sum/source_dimension_size_0;
            }
          }
          break;
        }
        case (CV_REDUCE_MAX):
        {
          for (int dim_1 = 0; dim_1 < source_dimension_size_1; ++dim_1)
          {
            for (int dim_2 = 0; dim_2 < source_dimension_size_2; ++dim_2)
            {
              max = -10000;
              for (int dim_0 = 0; dim_0 < source_dimension_size_0; ++dim_0)
              {
                min = source.at<T>(dim_0, dim_1, dim_2);
                if (min > max)
                {
                  max = min;
                }
              }
              destination.at<T>(dim_1, dim_2) = max;
            }
          }
          break;
        }
        case (CV_REDUCE_MIN):
        {
          for (int dim_1 = 0; dim_1 < source_dimension_size_1; ++dim_1)
          {
            for (int dim_2 = 0; dim_2 < source_dimension_size_2; ++dim_2)
            {
              min = 10000;
              for (int dim_0 = 0; dim_0 < source_dimension_size_0; ++dim_0)
              {
                max = source.at<T>(dim_0, dim_1, dim_2);
                if(max < min)
                {
                  min = max;
                }
              }
              destination.at<T>(dim_1, dim_2) = min;
            }
          }
          break;
        }
        default:
        {
          break;
        }
      } // END switch reductionOperator
      break;
    }
    case 1:
    {
      switch (reductionOperator)
      {
        case (CV_REDUCE_SUM):
        {
          for (int dim_0 = 0; dim_0 < source_dimension_size_0; ++dim_0)
          {
            for (int dim_2 = 0; dim_2 < source_dimension_size_2; ++dim_2)
            {
              sum = 0;
              for (int dim_1 = 0; dim_1 < source_dimension_size_1; ++dim_1)
              {
                sum += source.at<T>(dim_0, dim_1, dim_2);
              }
              destination.at<T>(dim_0, dim_2) = sum;
            }
          }
          break;
        }
        case (CV_REDUCE_AVG):
        {
          for (int dim_0 = 0; dim_0 < source_dimension_size_0; ++dim_0)
          {
            for (int dim_2 = 0; dim_2 < source_dimension_size_2; ++dim_2)
            {
              sum = 0;
              for (int dim_1 = 0; dim_1 < source_dimension_size_1; ++dim_1)
              {
                sum += source.at<T>(dim_0, dim_1, dim_2);
              }
              destination.at<T>(dim_0, dim_2) = sum/source_dimension_size_0;
            }
          }
          break;
        }
        case (CV_REDUCE_MAX):
        {
          for (int dim_0 = 0; dim_0 < source_dimension_size_0; ++dim_0)
          {
            for (int dim_2 = 0; dim_2 < source_dimension_size_2; ++dim_2)
            {
              max = -10000;
              for (int dim_1 = 0; dim_1 < source_dimension_size_1; ++dim_1)
              {
                min = source.at<T>(dim_0, dim_1, dim_2);
                if (min > max)
                {
                  max = min;
                }
              }
              destination.at<T>(dim_0, dim_2) = max;
            }
          }
          break;
        }
        case (CV_REDUCE_MIN):
        {
          for (int dim_0 = 0; dim_0 < source_dimension_size_0; ++dim_0)
          {
            for (int dim_2 = 0; dim_2 < source_dimension_size_2; ++dim_2)
            {
              min = 10000;
              for (int dim_1 = 0; dim_1 < source_dimension_size_1; ++dim_1)
              {
                max = source.at<T>(dim_0, dim_1, dim_2);
                if (max < min)
                {
                  min = max;
                }
              }
              destination.at<T>(dim_0, dim_2) = min;
            }
          }
          break;
        }
        default:
        {
          break;
        }
      } // END switch reductionOperator
      break;
    }
    case 2:
    {
      switch (reductionOperator)
      {
        case (CV_REDUCE_SUM):
        {
          for (int dim_0 = 0; dim_0 < source_dimension_size_0; ++dim_0)
          {
            for (int dim_1 = 0; dim_1 < source_dimension_size_1; ++dim_1)
            {
              sum = 0;
              for (int dim_2 = 0; dim_2 < source_dimension_size_2; ++dim_2)
              {
                sum += source.at<T>(dim_0 ,dim_1, dim_2);
              }
              destination.at<T>(dim_0, dim_1) = sum;
            }
          }
          break;
        }
        case (CV_REDUCE_AVG):
        {
          for (int dim_0 = 0; dim_0 < source_dimension_size_0; ++dim_0)
          {
            for (int dim_1 = 0; dim_1 < source_dimension_size_1; ++dim_1)
            {
              sum = 0;
              for (int dim_2 = 0; dim_2 < source_dimension_size_2; ++dim_2)
              {
                sum += source.at<T>(dim_0, dim_1, dim_2);
              }
              destination.at<T>(dim_0, dim_1) = sum/source_dimension_size_0;
            }
          }
          break;
        }
        case (CV_REDUCE_MAX):
        {
          for (int dim_0 = 0; dim_0 < source_dimension_size_0; ++dim_0)
          {
            for (int dim_1 = 0; dim_1 < source_dimension_size_1; ++dim_1)
            {
              max = -10000;
              for (int dim_2 = 0; dim_2 < source_dimension_size_2; ++dim_2)
              {
                min = source.at<T>(dim_0, dim_1, dim_2);
                if (min > max)
                {
                  max = min;
                }
              }
              destination.at<T>(dim_0, dim_1) = max;
            }
          }
          break;
        }
        case (CV_REDUCE_MIN):
        {
          for (int dim_0 = 0; dim_0 < source_dimension_size_0; ++dim_0)
          {
            for (int dim_1 = 0; dim_1 < source_dimension_size_1; ++dim_1)
            {
              min = 10000;
              for (int dim_2 = 0; dim_2 < source_dimension_size_2; ++dim_2)
              {
                max = source.at<T>(dim_0, dim_1, dim_2);
                if (max < min)
                {
                  min = max;
                }
              }
              destination.at<T>(dim_0, dim_1) = min;
            }
          }
          break;
        }
        default:
        {
          break;
        }
      } // END switch reductionOperator
      break;
    }
    default:
    {
      break;
    }
  }
}

template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::reduceCvMat3D<float>(const cv::Mat& source, cv::Mat& dst, int dimensionToReduce, int reductionOperator);
template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::reduceCvMat3D<double>(const cv::Mat& source, cv::Mat& dst, int dimensionToReduce, int reductionOperator);
