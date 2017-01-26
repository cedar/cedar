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

    File:        step_MatrixSlice.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 08 29

    Description: Unit test for the cedar::proc::step::MatrixSlice class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/MatrixSlice.h"
#include "cedar/auxiliaries/MatData.h"

// STL INCLUDES
#include <vector>

//!@todo Test 2d, nd matrices
//!@todo Test multichannel matrices
//!@todo Test different matrix types

void fillMat(cv::Mat& mat)
{
  for (size_t i = 0; i < mat.total(); ++i)
  {
    mat.at<float>(i) = static_cast<float>(i);
  }
}

int check_1d_slice(cv::Mat slice, cv::Mat expected)
{
  int errors = 0;

  cv::MatConstIterator_<float> slice_it = slice.begin<float>();
  cv::MatConstIterator_<float> slice_end = slice.end<float>();

  cv::MatConstIterator_<float> expected_it = expected.begin<float>();
  cv::MatConstIterator_<float> expected_end = expected.end<float>();

  for (; slice_it != slice_end && expected_it != expected_end; ++slice_it, ++expected_it)
  {
    float slice_val = *slice_it;
    float expected_val = *expected_it;
    if (slice_val != expected_val)
    {
      ++errors;
    }
  }

  if (slice_it != slice_end || expected_it != expected_end)
  {
    ++errors;
    std::cout << "Error: slices don't have the same size." << std::endl;
  }

  if (errors != 0)
  {
    std::cout << "Slice:" << slice << std::endl;
    std::cout << "Expected: " << expected << std::endl;
  }

  return errors;
}

// SYSTEM INCLUDES
int main(int, char**)
{
  int errors = 0;

  std::cout << "Testing n x 1 matrix." << std::endl;
  {
    cv::Mat n_x_1_mat(5, 1, CV_32F);
    fillMat(n_x_1_mat);
    cedar::aux::MatDataPtr input(new cedar::aux::MatData(n_x_1_mat));
    cedar::proc::steps::MatrixSlicePtr slicer(new cedar::proc::steps::MatrixSlice());

    slicer->setInput("matrix", input);

    std::vector<cv::Range> ranges;
    ranges.push_back(cv::Range(1, 3));
    slicer->setRanges(ranges);

    slicer->onTrigger();

    auto output_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(slicer->getOutput("slice"));
    cv::Mat expected = (cv::Mat_<float>(2, 1) << 1.0f , 2.0f);
    errors += check_1d_slice(output_data->getData(), expected);
  }

  std::cout << "Testing 1 x n matrix." << std::endl;
  {
    cv::Mat _1_x_n_mat(1, 5, CV_32F);
    fillMat(_1_x_n_mat);
    cedar::aux::MatDataPtr input(new cedar::aux::MatData(_1_x_n_mat));
    cedar::proc::steps::MatrixSlicePtr slicer(new cedar::proc::steps::MatrixSlice());

    slicer->setInput("matrix", input);

    std::vector<cv::Range> ranges;
    ranges.push_back(cv::Range(1, 3));
    slicer->setRanges(ranges);

    slicer->onTrigger();

    auto output_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(slicer->getOutput("slice"));
    cv::Mat expected = (cv::Mat_<float>(2, 1) << 1.0f , 2.0f);
    errors += check_1d_slice(output_data->getData(), expected);
  }

  std::cout << "Test finished with " << errors << " error(s)." << std::endl;
  return errors;
}
