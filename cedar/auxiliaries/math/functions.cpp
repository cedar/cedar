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

    //!@todo may want to move this outside of (for dim) block, since all dims are cyclic
    if (cyclic) // is this a cyclic kernel? (only check once)
    {
      for (int row = 0; row < kernel_parts.at(dim).rows; ++row)
      {
        int index = row - static_cast<int>(centers.at(dim));
        int current_size = static_cast<int>(matrixSizes.at(dim));
        /* if the kernel is shifted away from the center of the matrix, make sure to take as many values from a
         * cyclic kernel as the kernel center is shifted away from the matrix center
         */
        int shift = static_cast<int>(centers.at(dim)) - current_size / 2;
        if (shift > 0 && row - shift < 0) // kernel center is to the right of matrix center and index is in cyclic range
        {
          index += current_size;
        }
        if (shift < 0 && row - shift >= current_size) // kernel center is to the left of matrix center, cyclic index
        {
          index -= current_size;
        }
        kernel_parts.at(dim).at<float>(row, 0)
              = cedar::aux::math::gauss(index, sigmas.at(dim));
      }
    }
    else // nothing special to do here
    {
      for (int row = 0; row < kernel_parts.at(dim).rows; ++row)
      {
        kernel_parts.at(dim).at<float>(row, 0)
              = cedar::aux::math::gauss(static_cast<int>(row) - centers.at(dim), sigmas.at(dim));
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
