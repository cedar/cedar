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

    Date:        2014 02 24

    Description:

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/dynamics/fields/NeuralField.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/kernel/Kernel.h"
#include "cedar/auxiliaries/kernel/Gauss.h"
#include "cedar/auxiliaries/convolution/Convolution.h"
#include "cedar/auxiliaries/math/TransferFunction.h"
#include "cedar/auxiliaries/math/transferFunctions/AbsSigmoid.h"
#include "cedar/auxiliaries/ObjectParameterTemplate.h"
#include "cedar/auxiliaries/ObjectListParameterTemplate.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/auxiliaries/sleepFunctions.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <iostream>
#include <list>

#define TEST_ASSERTION(cond) if (!(cond)) { ++errors; std::cout << "ERROR in line " << __LINE__ << ": " << # cond << std::endl; } \
  else { std::cout << "passed: " << # cond << std::endl; }

void run_test()
{
  using cedar::proc::Group;
  using cedar::proc::GroupPtr;
  using cedar::proc::Step;
  using cedar::proc::StepPtr;
  using cedar::dyn::NeuralField;

  int errors = 0;

  GroupPtr group(new Group());
  group->readJson("test://unit/processing/StepParameterLinking/Architecture.json");

  auto field_1 = group->getElement<NeuralField>("field 1");
  auto field_2 = group->getElement<NeuralField>("field 2");

  // we want to change the border type, so it should start different from what we assign next
  double new_resting_level = -1.234;
  TEST_ASSERTION(field_1->getRestingLevel() != new_resting_level);
  TEST_ASSERTION(field_2->getRestingLevel() != new_resting_level);
  // the resting levels of both fields are linked and should thus be the same if one of them changes
  field_1->setRestingLevel(new_resting_level);
  TEST_ASSERTION(field_1->getRestingLevel() == new_resting_level);
  TEST_ASSERTION(field_2->getRestingLevel() == field_1->getRestingLevel());

  auto convolution_1 = boost::dynamic_pointer_cast<cedar::aux::conv::Convolution>(field_1->getConfigurableChild("lateral kernel convolution"));
  auto convolution_2 = boost::dynamic_pointer_cast<cedar::aux::conv::Convolution>(field_2->getConfigurableChild("lateral kernel convolution"));

  TEST_ASSERTION(convolution_1);
  TEST_ASSERTION(convolution_2);

  // we want to change the border type, so it should start different from what we assign next
  auto new_border_type = cedar::aux::conv::BorderType::Reflect;
  TEST_ASSERTION(convolution_1->getBorderType() != new_border_type);
  TEST_ASSERTION(convolution_2->getBorderType() != new_border_type);
  // check that both border types are synchronized
  convolution_1->setBorderType(new_border_type);
  TEST_ASSERTION(convolution_1->getBorderType() == new_border_type);
  TEST_ASSERTION(convolution_1->getBorderType() == convolution_2->getBorderType());

  // same as above, this time for the sigmoid's beta
  auto transfer_fun_parameter_1 =
      boost::dynamic_pointer_cast<cedar::aux::ObjectParameterTemplate<cedar::aux::math::TransferFunction> >
        (
          field_1->getParameter("sigmoid")
        );
  auto transfer_fun_parameter_2 =
      boost::dynamic_pointer_cast<cedar::aux::ObjectParameterTemplate<cedar::aux::math::TransferFunction> >
        (
          field_2->getParameter("sigmoid")
        );

  TEST_ASSERTION(transfer_fun_parameter_1);
  TEST_ASSERTION(transfer_fun_parameter_2);

  auto sigmoid_1 = boost::dynamic_pointer_cast<cedar::aux::math::AbsSigmoid>(transfer_fun_parameter_1->getValue());
  auto sigmoid_2 = boost::dynamic_pointer_cast<cedar::aux::math::AbsSigmoid>(transfer_fun_parameter_2->getValue());

  TEST_ASSERTION(sigmoid_1);
  TEST_ASSERTION(sigmoid_2);

  double new_beta = 5.678;
  TEST_ASSERTION(sigmoid_1->getBeta() != new_beta);
  TEST_ASSERTION(sigmoid_2->getBeta() != new_beta);

  sigmoid_1->setBeta(new_beta);
  TEST_ASSERTION(sigmoid_1->getBeta() == new_beta);
  TEST_ASSERTION(sigmoid_1->getBeta() == sigmoid_2->getBeta());

  // same old, but for the kernel amplitudes
  auto kernel_parameter_1 =
      boost::dynamic_pointer_cast<cedar::aux::ObjectListParameterTemplate<cedar::aux::kernel::Kernel> >
        (
          field_1->getParameter("lateral kernels")
        );
  auto kernel_parameter_2 =
      boost::dynamic_pointer_cast<cedar::aux::ObjectListParameterTemplate<cedar::aux::kernel::Kernel> >
        (
          field_2->getParameter("lateral kernels")
        );

  TEST_ASSERTION(kernel_parameter_1);
  TEST_ASSERTION(kernel_parameter_2);

  auto kernel_1 = boost::dynamic_pointer_cast<cedar::aux::kernel::Gauss>(kernel_parameter_1->at(0));
  auto kernel_2 = boost::dynamic_pointer_cast<cedar::aux::kernel::Gauss>(kernel_parameter_2->at(0));

  TEST_ASSERTION(kernel_1);
  TEST_ASSERTION(kernel_2);

  double new_amplitude = 5.678;
  TEST_ASSERTION(kernel_1->getAmplitude() != new_amplitude);
  TEST_ASSERTION(kernel_2->getAmplitude() != new_amplitude);

  kernel_1->setAmplitude(new_amplitude);

  TEST_ASSERTION(kernel_1->getAmplitude() == new_amplitude);
  TEST_ASSERTION(kernel_1->getAmplitude() == kernel_2->getAmplitude());

  QApplication::exit(errors);
}

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  cedar::aux::CallFunctionInThreadPtr caller(new cedar::aux::CallFunctionInThread(boost::bind(&run_test)));
  caller->start();

  return app.exec();
}
