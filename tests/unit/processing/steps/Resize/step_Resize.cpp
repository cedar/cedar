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

    File:        Projection.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2011 12 05

    Description: Unit test for the cedar::proc::Projection class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/Resize.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/math/tools.h"


int testResize1D(cv::Mat inputOnes, unsigned int reSize, cv::Mat expectedValue)
{
  CEDAR_ASSERT(cedar::aux::math::get1DMatrixSize(expectedValue) == reSize);
  int errors = 0;

  std::cout << "Testing 1D resizing." << std::endl;

  cedar::aux::MatDataPtr input(new cedar::aux::MatData(inputOnes));
  cedar::proc::steps::ResizePtr resizer(new cedar::proc::steps::Resize());

  // set target size
  cedar::aux::UIntVectorParameterPtr size
    = boost::dynamic_pointer_cast<cedar::aux::UIntVectorParameter>(resizer->getParameter("output size"));

  if (size->size() != 1)
  {
    ++errors;
    std::cout << "ERROR: Size parameter was not adapted to the new dimensionality properly." << std::endl;
  }

  size->setValue(0, reSize);

  resizer->setInput("input", input);
  resizer->onTrigger();
  cedar::aux::ConstMatDataPtr output = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(resizer->getOutput("output"));

  const cv::Mat& res = output->getData();

  if (res.rows != 1 && res.cols != 1)
  {
    ++errors;
    std::cout << "ERROR: Result of 1D resize is not 1D any more." << std::endl;
  }

  if (cedar::aux::math::get1DMatrixSize(res) != reSize)
  {
    ++errors;
    std::cout << "ERROR: result matrix has the wrong size." << std::endl;
  }

  for (unsigned int i = 0; i < cedar::aux::math::get1DMatrixSize(res); ++i)
  {
    float value = res.at<float>(static_cast<int>(i));
    float expected = expectedValue.at<float>(i);
    if (value != expected)
    {
      ++errors;
      std::cout << "ERROR: entry " << i << ": " << value << " != " << expected << std::endl;
    }
  }

  std::cout << "Original matrix: " << inputOnes << std::endl;
  std::cout << "Resized matrix:  " << res << std::endl;
  std::cout << "Expected:        " << expectedValue << std::endl;

  return errors;
}

// SYSTEM INCLUDES
int main(int, char**)
{
  int errors = 0;

  errors += testResize1D(0.5 * cv::Mat::ones(5, 1, CV_32F), 35, 0.5 * cv::Mat::ones(35, 1, CV_32F));
  errors += testResize1D(0.5 * cv::Mat::ones(1, 5, CV_32F), 35, 0.5 * cv::Mat::ones(35, 1, CV_32F));

  cv::Mat pyramid = cv::Mat::zeros(5, 1, CV_32F);
  pyramid.at<float>(1) = 1;
  pyramid.at<float>(2) = 2;
  pyramid.at<float>(3) = 1;

  cv::Mat pyramid_big;
  cv::resize(pyramid, pyramid_big, cv::Size(1, 9), 0, 0, cv::INTER_LINEAR);
  errors += testResize1D(pyramid, 9, pyramid_big);
  errors += testResize1D(pyramid.t(), 9, pyramid_big);

  std::cout << "Test finished with " << errors << " error(s)." << std::endl;
  return errors;
}
