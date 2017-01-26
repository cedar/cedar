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

    File:        functions.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2012 07 16

    Description: Math functions

    Credits:

======================================================================================================================*/

// CEDAR_INCLUDES
#include "cedar/auxiliaries/math/functions.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <iostream>
#include <math.h>

cv::Mat cedar::aux::math::boxMatrix
        (
          unsigned int dimensionality,
          const std::vector<unsigned int>& matrixSizes,
          double boxAmplitude,
          double referenceLevel,
          const std::vector<unsigned int>& widths,
          const std::vector<unsigned int>& leftBounds
        )
{
  std::vector<int> matrix_sizes_int(matrixSizes.size());
  for (unsigned int i = 0; i < matrixSizes.size(); ++i)
  {
    matrix_sizes_int.at(i) = static_cast<int>(matrixSizes.at(i));
  }
  if (dimensionality < 2)
  {
    matrix_sizes_int.push_back(1);
  }

  cv::Mat output
    = cv::Mat(static_cast<int>(dimensionality), &matrix_sizes_int.front(), CV_32F, cv::Scalar(referenceLevel));

  std::vector<int> box_widths_int(widths.size());
  for (unsigned int i = 0; i < widths.size(); ++i)
  {
    unsigned int left = leftBounds.at(i);
    if (left >= matrixSizes.at(i))
    {
      return output;
    }
    if (left + widths.at(i) > matrixSizes.at(i))
    {
      box_widths_int.at(i) = static_cast<int>(matrixSizes.at(i) - leftBounds.at(i));
    }
    else
    {
      box_widths_int.at(i) = static_cast<int>(widths.at(i));
    }
  }

  if (dimensionality < 2)
  {
    box_widths_int.push_back(1);
  }

  cv::Mat box = cv::Mat(static_cast<int>(dimensionality), &box_widths_int.front(), CV_32F, cv::Scalar(boxAmplitude));

  std::vector<cv::Range> box_ranges(dimensionality);
  for (unsigned int i = 0; i < dimensionality; ++i)
  {
    unsigned int left = leftBounds.at(i);
    unsigned int right = left + widths.at(i);
    if (right > matrixSizes.at(i))
    {
      right = matrixSizes.at(i);
    }
    box_ranges.at(i) = cv::Range(left, right);
  }

  if (dimensionality < 2)
  {
    box_ranges.push_back(cv::Range::all());
  }

  cv::Mat tmp = output(&box_ranges.front());
  box.copyTo(tmp);

  return output;
}

cv::Mat cedar::aux::math::gaussMatrix
        (
          unsigned int dimensionality,
          const std::vector<unsigned int>& matrixSizes,
          double amplitude,
          const std::vector<double>& sigmas,
          const std::vector<double>& centers,
          bool cyclic
        )
{
  cv::Mat output;
  std::vector<cv::Mat> kernel_parts;
  kernel_parts.resize(dimensionality);
  for (size_t dim = 0; dim < dimensionality; ++dim)
  {
    kernel_parts.at(dim) = cv::Mat(static_cast<int>(matrixSizes.at(dim)), 1, CV_32F);
    CEDAR_DEBUG_ASSERT(sigmas.at(dim) > 0.0);

    if (cyclic) // is this a cyclic kernel? (only check once)
    {
      for (int row = 0; row < kernel_parts.at(dim).rows; ++row)
      {
        double position = row - static_cast<double>(centers.at(dim));
        double current_size = static_cast<double>(matrixSizes.at(dim));
        /* if the kernel is shifted away from the center of the matrix, make sure to take as many values from a
         * cyclic kernel as the kernel center is shifted away from the matrix center
         */
        double shift = static_cast<double>(centers.at(dim)) - current_size / 2.0;
        if (shift > 0 && row - shift < 0) // kernel center is to the right of matrix center and index is in cyclic range
        {
          position += current_size;
        }
        if (shift < 0 && row - shift >= current_size) // kernel center is to the left of matrix center, cyclic index
        {
          position -= current_size;
        }
        kernel_parts.at(dim).at<float>(row, 0)
              = cedar::aux::math::gauss(position, sigmas.at(dim));
      }
    }
    else // nothing special to do here
    {
      for (int row = 0; row < kernel_parts.at(dim).rows; ++row)
      {
        kernel_parts.at(dim).at<float>(row, 0)
              = cedar::aux::math::gauss(static_cast<double>(row) - centers.at(dim), sigmas.at(dim));
      }
    }
  }
  kernel_parts.at(0) *= amplitude;
  // assemble the input
  std::vector<int> sizes(static_cast<size_t>(dimensionality));
  for (unsigned int i = 0; i < dimensionality; i++)
  {
    sizes[i] = matrixSizes.at(i);
  }
  if (dimensionality == 1)
  {
    output = cv::Mat(static_cast<int>(sizes.at(0)), 1, CV_32F);
  }
  else
  {
    output = cv::Mat(static_cast<int>(dimensionality), &sizes.at(0), CV_32F);
  }
  // now fill up the big kernel matrix
  std::vector<int> position(static_cast<size_t>(dimensionality));
  unsigned int max_index = 1;
  double max_index_d = 1.0;
  for (unsigned int dim = 0; dim < dimensionality; dim++)
  {
    position[dim] = 0;
    max_index_d *= sizes[dim];
    if (max_index_d > std::numeric_limits<unsigned int>::max()/100.0)
    {
      CEDAR_THROW(cedar::aux::RangeException, "cannot handle inputs of this size");
    }
  }
  max_index = static_cast<unsigned int>(max_index_d);
  for (unsigned int i = 0; i < max_index; i++)
  {
    float value = 1.0;
    for (unsigned int dim = 0; dim < dimensionality; dim++)
    {
      value *= kernel_parts.at(dim).at<float>(position.at(dim), 0);
    }
    if (dimensionality == 1)
    {
      output.at<float>(position[0], 0) = value;
    }
    else
    {
      output.at<float>(&position.at(0)) = value;
    }
    // increment index
    position[0]++;
    for (unsigned int dim = 0; dim < dimensionality; dim++)
    {
      if (position[dim] >= static_cast<int>(sizes[dim]))
      {
        position[dim] = 0;
        if (dim+1 < dimensionality)
        {
          position[dim+1]++;
        }
      }
    }
  }
  return output;
}
