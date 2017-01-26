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

    File:        tools.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 11

    Description: Math tools

    Credits:

======================================================================================================================*/

// CEDAR_INCLUDES
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"
#include <cedar/auxiliaries/utilities.h>

// SYSTEM INCLUDES
#include <iostream>
#include <math.h>
#include <limits.h>
#ifndef Q_MOC_RUN
  #include <boost/static_assert.hpp>
#endif

namespace cedar
{
  namespace aux
  {
    namespace math
    {
      //! Internal helper function for cedar::aux::math::flip.
      void flip2D(cv::Mat input, cv::Mat& output, bool flipFirst, bool flipSecond)
      {
        int flip_code;
        if (flipFirst && flipSecond)
        {
          flip_code = -1;
        }
        else if (flipFirst)
        {
          if (input.rows == 1)
          {
            flip_code = 1;
          }
          else
          {
            flip_code = 0;
          }
        }
        else if (flipSecond)
        {
          flip_code = 1;
        }
        else // nothing to do
        {
          input.copyTo(output);
          return;
        }

        cv::flip(input, output, flip_code);
      }
    }
  }
}

void cedar::aux::math::flip(const cv::Mat& toFlip, cv::Mat& flipped, const std::vector<bool>& flippedDimensions)
{
  auto input_dim = cedar::aux::math::getDimensionalityOf(toFlip);
  if (input_dim <= 2)
  {
    bool flip_first = false;
    if (input_dim > 0)
    {
      flip_first = flippedDimensions.at(0);
    }
    bool flip_second = false;
    if (input_dim > 1)
    {
      flip_second = flippedDimensions.at(1);
    }

    cedar::aux::math::flip2D(toFlip, flipped, flip_first, flip_second);
  }
  else if (input_dim <= 3)
  {
    int start, diff;

    // this method does not support in-place operation; therefore, if src == dst, throw an error
    CEDAR_ASSERT(toFlip.data != flipped.data);

    CEDAR_DEBUG_ASSERT(toFlip.dims == 3);
    // this makes sure the receiving matrix is properly initialized
    CEDAR_DEBUG_ASSERT(flipped.dims == 3);

    if (flippedDimensions.at(2))
    {
      start = toFlip.size[2] - 1;
      diff = -1;
    }
    else
    {
      start = 0;
      diff = 1;
    }

    cv::Range src_range[3], dst_range[3];
    src_range[0] = cv::Range::all();
    src_range[1] = cv::Range::all();
    dst_range[0] = cv::Range::all();
    dst_range[1] = cv::Range::all();
    for (int d3 = 0; d3 < toFlip.size[2]; ++d3)
    {
      src_range[2] = cv::Range(d3, d3 + 1);
      int dest_d3 = start + d3 * diff;
      dst_range[2] = cv::Range(dest_d3, dest_d3 + 1);

      cv::Mat input_slice = toFlip(src_range).clone();
      input_slice.copySize(cv::Mat(toFlip.size[0], toFlip.size[1], toFlip.type()));
      cv::Mat output_slice(toFlip.size[0], toFlip.size[1], toFlip.type());

      cedar::aux::math::flip2D(input_slice, output_slice, flippedDimensions.at(0), flippedDimensions.at(1));
      cv::Mat output_slice_clone = flipped(dst_range).clone();
      output_slice.copySize(output_slice_clone);

      flipped(dst_range) = 1.0 * output_slice;
    }
  }
}

cv::Mat cedar::aux::math::flip(const cv::Mat& toFlip, const std::vector<bool>& flippedDimensions)
{
  cv::Mat result = 0.0 * toFlip.clone();
  cedar::aux::math::flip(toFlip, result, flippedDimensions);
  return result;
}

int cedar::aux::math::matrixTypeFromString(const std::string& typeStr)
{
#define MAT_TYPE_STR(CV_TYPE) else if (typeStr == #CV_TYPE) { return CV_TYPE; }
  if (false)
  {
    // emppty to allow else if blocks in the macro
  }
  MAT_TYPE_STR(CV_8U)
  MAT_TYPE_STR(CV_8UC1)
  MAT_TYPE_STR(CV_8UC2)
  MAT_TYPE_STR(CV_8UC3)
  MAT_TYPE_STR(CV_8UC4)
  MAT_TYPE_STR(CV_8S)
  MAT_TYPE_STR(CV_8SC1)
  MAT_TYPE_STR(CV_8SC2)
  MAT_TYPE_STR(CV_8SC3)
  MAT_TYPE_STR(CV_8SC4)
  MAT_TYPE_STR(CV_16U)
  MAT_TYPE_STR(CV_16UC1)
  MAT_TYPE_STR(CV_16UC2)
  MAT_TYPE_STR(CV_16UC3)
  MAT_TYPE_STR(CV_16UC4)
  MAT_TYPE_STR(CV_16S)
  MAT_TYPE_STR(CV_16SC1)
  MAT_TYPE_STR(CV_16SC2)
  MAT_TYPE_STR(CV_16SC3)
  MAT_TYPE_STR(CV_16SC4)
  MAT_TYPE_STR(CV_32S)
  MAT_TYPE_STR(CV_32SC1)
  MAT_TYPE_STR(CV_32SC2)
  MAT_TYPE_STR(CV_32SC3)
  MAT_TYPE_STR(CV_32SC4)
  MAT_TYPE_STR(CV_32F)
  MAT_TYPE_STR(CV_32FC3)
  MAT_TYPE_STR(CV_64F)
  MAT_TYPE_STR(CV_64FC3)

#undef MAT_TYPE_STR

  else
  {
    CEDAR_THROW(cedar::aux::UnknownTypeException, "Cannot convert \"" + typeStr + "\" to OpenCV matrix type.");
  }
}

std::string cedar::aux::math::matrixTypeToString(const cv::Mat& matrix)
{
  switch (matrix.type())
  {
    case CV_8U:
      return "CV_8U";

    case CV_8UC2:
      return "CV_8UC2";

    case CV_8UC3:
      return "CV_8UC3";

    case CV_8UC4:
      return "CV_8UC4";


    case CV_8S:
      return "CV_8S";

    case CV_8SC3:
      return "CV_8SC3";


    case CV_16U:
      return "CV_16U";

    case CV_16UC3:
      return "CV_16UC3";

    case CV_16S:
      return "CV_16S";

    case CV_16SC3:
      return "CV_16SC3";


    case CV_32S:
      return "CV_32S";

    case CV_32SC3:
      return "CV_32SC3";

    case CV_32F:
      return "CV_32F";

    case CV_32FC3:
      return "CV_32FC3";


    case CV_64F:
      return "CV_64F";

    case CV_64FC3:
      return "CV_64FC3";


    default:
      return "Unknown type: " + cedar::aux::toString(matrix.type());
  }
}

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

unsigned int cedar::aux::math::maxIndex1D(const cv::Mat matrix)
{
  cv::Point max_index;
  cv::minMaxLoc(matrix, NULL, NULL, NULL, &max_index);

  CEDAR_DEBUG_ASSERT(cedar::aux::math::getDimensionalityOf(matrix) <= 1);

  if (matrix.rows == 1)
  {
    return static_cast<unsigned int>(max_index.x);
  }
  else
  {
    CEDAR_DEBUG_ASSERT(matrix.cols == 1);
    return static_cast<unsigned int>(max_index.y);
  }
}

double cedar::aux::math::max(const cv::Mat matrix)
{
  double max;
  cv::minMaxLoc(matrix, 0, &max);
  return max;
}

unsigned int cedar::aux::math::minIndex1D(const cv::Mat matrix)
{
  cv::Point min_index;
  cv::minMaxLoc(matrix, NULL, NULL, &min_index, NULL);

  CEDAR_DEBUG_ASSERT(cedar::aux::math::getDimensionalityOf(matrix) <= 1);

  if (matrix.rows == 1)
  {
    return static_cast<unsigned int>(min_index.x);
  }
  else
  {
    CEDAR_DEBUG_ASSERT(matrix.cols == 1);
    return static_cast<unsigned int>(min_index.y);
  }
}


double cedar::aux::math::min(const cv::Mat matrix)
{
  double min;
  cv::minMaxLoc(matrix, &min);
  return min;
}

template <typename T>
void cedar::aux::math::reduceCvMat3D
                       (
                         const cv::Mat& source,
                         cv::Mat& destination,
                         int dimensionToReduce,
                         int reductionOperator,
                         bool swapDimensions
                       )
{
  CEDAR_ASSERT(dimensionToReduce < 3);
  CEDAR_DEBUG_ASSERT(source.dims == 3);
  CEDAR_DEBUG_ASSERT(destination.dims == 2);

  size_t mapped_dimensions[2];

  switch (dimensionToReduce)
  {
    case 0:
      mapped_dimensions[0] = 1;
      mapped_dimensions[1] = 2;
      break;

    case 1:
      mapped_dimensions[0] = 0;
      mapped_dimensions[1] = 2;
      break;

    case 2:
      mapped_dimensions[0] = 0;
      mapped_dimensions[1] = 1;
      break;

    default:
      // should never happen -- dimensionsToReduce < 3
      CEDAR_ASSERT(false);
  }

  if (swapDimensions)
  {
    size_t other = mapped_dimensions[0];
    mapped_dimensions[0] = mapped_dimensions[1];
    mapped_dimensions[1] = other;
  }

  int source_size[2] = {source.size[mapped_dimensions[0]], source.size[mapped_dimensions[1]]};

  CEDAR_ASSERT(destination.rows == source_size[0]);
  CEDAR_ASSERT(destination.cols == source_size[1]);

  int dropped_size = source.size[dimensionToReduce];

  T sum;
  T min;
  T max;

  int src_index[3] = {0, 0, 0};
  int& dim_dropped = src_index[static_cast<size_t>(dimensionToReduce)];
  int& dim_1 = src_index[mapped_dimensions[0]];
  int& dim_2 = src_index[mapped_dimensions[1]];

  switch (reductionOperator)
  {
    case CEDAR_OPENCV_CONSTANT(REDUCE_SUM):
    {
      for (dim_1 = 0; dim_1 < source_size[0]; ++dim_1)
      {
        for (dim_2 = 0; dim_2 < source_size[1]; ++dim_2)
        {
          sum = 0;
          for (dim_dropped = 0; dim_dropped < dropped_size; ++dim_dropped)
          {
            sum += source.at<T>(src_index);
          }
          destination.at<T>(dim_1, dim_2) = sum;
        }
      }
      break;
    }
    case CEDAR_OPENCV_CONSTANT(REDUCE_AVG):
    {
      for (dim_1 = 0; dim_1 < source_size[0]; ++dim_1)
      {
        for (dim_2 = 0; dim_2 < source_size[1]; ++dim_2)
        {
          sum = 0;
          for (dim_dropped = 0; dim_dropped < dropped_size; ++dim_dropped)
          {
            sum += source.at<T>(src_index);
          }
          destination.at<T>(dim_1, dim_2) = sum/dropped_size;
        }
      }
      break;
    }
    case CEDAR_OPENCV_CONSTANT(REDUCE_MAX):
    {
      for (dim_1 = 0; dim_1 < source_size[0]; ++dim_1)
      {
        for (dim_2 = 0; dim_2 < source_size[1]; ++dim_2)
        {
          max = static_cast<T>(std::numeric_limits<int>::min());
          for (dim_dropped = 0; dim_dropped < dropped_size; ++dim_dropped)
          {
            max = std::max(max, source.at<T>(src_index));
          }
          destination.at<T>(dim_1, dim_2) = max;
        }
      }
      break;
    }
    case CEDAR_OPENCV_CONSTANT(REDUCE_MIN):
    {
      for (dim_1 = 0; dim_1 < source_size[0]; ++dim_1)
      {
        for (dim_2 = 0; dim_2 < source_size[1]; ++dim_2)
        {
          min = static_cast<T>(std::numeric_limits<int>::max());
          for (dim_dropped = 0; dim_dropped < dropped_size; ++dim_dropped)
          {
            min = std::min(min, source.at<T>(src_index));
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
}

template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::reduceCvMat3D<float>(const cv::Mat& source, cv::Mat& dst, int dimensionToReduce, int reductionOperator, bool swapDimensions);
template CEDAR_AUX_LIB_EXPORT void cedar::aux::math::reduceCvMat3D<double>(const cv::Mat& source, cv::Mat& dst, int dimensionToReduce, int reductionOperator, bool swapDimensions);


void cedar::aux::math::findPeaks(const cv::Mat& activation, std::vector<cv::Point>& peakCenters, double threshold)
{
  if (activation.empty())
  {
    return;
  }

  cv::Mat marked_map = activation.clone();

  while (true)
  {
    cv::Point max_loc;
    double max;
    cv::minMaxLoc(marked_map, nullptr, &max, nullptr, &max_loc);
    if (max < threshold)
    {
      break;
    }

    std::vector<cv::Point> points;
    std::vector<cv::Point> to_explore;
    to_explore.push_back(max_loc);

    // find all connected above-threshold points around the maximum
    while (!to_explore.empty())
    {
      cv::Point pt = to_explore.back();
      to_explore.pop_back();

      if (pt.x < 0 || pt.y < 0 || pt.x >= marked_map.cols || pt.y >= marked_map.rows)
      {
        continue;
      }

      if (marked_map.at<float>(pt) >= threshold)
      {
        points.push_back(pt);
        for (int i = -1; i <= 1; ++i)
        {
          for (int j = -1; j <= 1; ++j)
          {
            if (i != 0 || j != 0)
            {
              to_explore.push_back(pt + cv::Point(i, j));
            }
          }
        }
      }
      // remove the point from those to be clustered
      marked_map.at<float>(pt) = 0.0f;
    }

    // find center of the point (use mean value)
    cv::Point sum(0, 0);
    for (auto point : points)
    {
      sum += point;
    }
    cv::Point center = cv::Point(sum.x / static_cast<int>(points.size()), sum.y / static_cast<int>(points.size()));
    peakCenters.push_back(center);
  }
}
