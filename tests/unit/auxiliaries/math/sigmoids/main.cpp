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

    Description: Tests all sigmoids.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/sigmoids.h"
#include "cedar/auxiliaries/math/Sigmoid.h"
#include "cedar/auxiliaries/math/transferFunctions/AbsSigmoid.h"
#include "cedar/auxiliaries/math/transferFunctions/ExpSigmoid.h"
#include "cedar/auxiliaries/math/transferFunctions/HeavisideSigmoid.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/math/TransferFunctionDeclaration.h"
#include "cedar/auxiliaries/utilities.h"

// SYSTEM INCLUDES

int main()
{
  // the number of errors encountered in this test
  int errors = 0;
  // the current test number
  int test_number = 0;

  // test sigmoid(double, double, double)
  std::cout << "test no " << test_number++ << std::endl;
  if (cedar::aux::math::sigmoidExp(5,1000,0) < 0.9)
  {
    std::cout << "error in sigmoid(double, double, double)" << std::endl;
    errors++;
  }
  // test sigmoidAbs()
  std::cout << "test no " << test_number++ << std::endl;
  if (cedar::aux::math::sigmoidAbs(5,1000,0) < 0.9)
  {
    std::cout << "error in sigmoidAbs(double, double, double)" << std::endl;
    errors++;
  }

  // test sigmoid(std::vector<double>, double, double)
  std::cout << "test no " << test_number++ << std::endl;
  std::vector<double> test_vector;
  test_vector.push_back(5.0);
  test_vector.push_back(-5.0);
  std::vector<double> result = cedar::aux::math::sigmoid(test_vector,1000,0);
  if (result.at(0) < 0.9 || result.at(1) > 0.1)
  {
    std::cout << "error in sigmoid(std::vector<double>, double, double)" << std::endl;
    errors++;
  }

  // test sigmoidInterval()
  std::cout << "test no " << test_number++ << std::endl;
  cedar::aux::math::sigmoidInterval(5,1000,0);

  // test sigmoid classes
  std::cout << "test no " << test_number++ << std::endl;
  cv::Mat my_values = cv::Mat::ones(1, 1, CV_32F);
  cv::Mat my_values_double = cv::Mat::ones(1, 1, CV_64F);
  cedar::aux::math::SigmoidPtr abs_sigmoid(new cedar::aux::math::AbsSigmoid(0.0, 10.0));
  cv::Mat sigmoid_my_values = abs_sigmoid->compute<float>(my_values);
  cv::Mat sigmoid_my_values_double = abs_sigmoid->compute<double>(my_values_double);
  cedar::aux::write(sigmoid_my_values);
  cedar::aux::write(sigmoid_my_values_double);

  my_values = cv::Mat::ones(1, 1, CV_32F);
  my_values_double = cv::Mat::ones(1, 1, CV_64F);
  cedar::aux::math::SigmoidPtr exp_sigmoid(new cedar::aux::math::ExpSigmoid(0.0, 10.0));
  sigmoid_my_values = exp_sigmoid->compute<float>(my_values);
  sigmoid_my_values_double = exp_sigmoid->compute<double>(my_values_double);
  cedar::aux::write(sigmoid_my_values);
  cedar::aux::write(sigmoid_my_values_double);

  my_values = cv::Mat::ones(1, 1, CV_32F);
  my_values_double = cv::Mat::ones(1, 1, CV_64F);
  cedar::aux::math::SigmoidPtr heaviside_sigmoid(new cedar::aux::math::HeavisideSigmoid(0.0));
  sigmoid_my_values = heaviside_sigmoid->compute<float>(my_values);
  sigmoid_my_values_double = heaviside_sigmoid->compute<double>(my_values_double);
  cedar::aux::write(sigmoid_my_values);
  cedar::aux::write(sigmoid_my_values_double);

  cedar::aux::math::TransferFunctionDeclarationPtr sigmoid_declaration
  (
    new cedar::aux::math::TransferFunctionDeclarationT<cedar::aux::math::AbsSigmoidPtr>("cedar.aux.math.AbsSigmoid")
  );
  cedar::aux::math::TransferFunctionPtr my_sigmoid = sigmoid_declaration->getObjectFactory()->allocate();
  sigmoid_my_values = my_sigmoid->compute<float>(my_values);
  sigmoid_my_values_double = my_sigmoid->compute<double>(my_values_double);
  cedar::aux::write(sigmoid_my_values);
  cedar::aux::write(sigmoid_my_values_double);

  std::cout << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
