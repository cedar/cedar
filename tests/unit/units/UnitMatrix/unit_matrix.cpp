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

    File:        unit_matrix.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2013 03 06

    Description: 

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/units/UnitMatrix.h"
#include "cedar/units/Length.h"
#include "cedar/units/Time.h"
#include "cedar/units/Velocity.h"

// SYSTEM INCLUDES
#include <iostream>

int main()
{
  // the number of errors encountered in this test
  int errors = 0;

  // test a simple self-made matrix multiplication
  cedar::unit::UnitMatrix<cedar::unit::Length::unit_type> length_matrix(cv::Mat(2,1,CV_32F), 1.0 * cedar::unit::meters);
  length_matrix.matrix.at<float>(0,0) = 2.0;
  length_matrix.matrix.at<float>(1,0) = 1.0;

  cedar::unit::UnitMatrix<cedar::unit::Time::unit_type> time_matrix(cv::Mat(2,1,CV_32F), 1.0 * cedar::unit::seconds);
  time_matrix.matrix.at<float>(0,0) = 4.0;
  time_matrix.matrix.at<float>(1,0) = 2.0;

  cedar::unit::UnitMatrix<cedar::unit::Velocity::unit_type> velocity_matrix(cv::Mat(2,1,CV_32F), 1.0 * cedar::unit::meters/cedar::unit::seconds);
  velocity_matrix.matrix = length_matrix.matrix.mul(1/time_matrix.matrix);
  velocity_matrix.unit = length_matrix.unit / time_matrix.unit;

  if (velocity_matrix.matrix.at<float>(0,0) != 0.5 || velocity_matrix.matrix.at<float>(1,0) != 0.5)
  {
    std::cout << "Error: dividing unit matrices does not work - wrong resulting values!" << std::endl;
    ++errors;
  }
  if (velocity_matrix.unit != 1.0 * cedar::unit::meters/cedar::unit::seconds)
  {
    std::cout << "Error: dividing unit matrices does not work - wrong resulting unit!" << std::endl;
    ++errors;
  }

  std::cout << velocity_matrix << std::endl;


  return errors;
}
