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
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2014 01 17

    Description: Test field performance.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "cedar/configuration.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/StepTime.h"
#include "cedar/dynamics/fields/NeuralField.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/testingUtilities/measurementFunctions.h"

// SYSTEM INCLUDES
#include <QApplication>

// helper method
void event_loop()
{
  int i = 0;

  while (QApplication::hasPendingEvents() && ++i < 1000)
  {
    QApplication::processEvents();
  }
}

void measure(unsigned int dim, unsigned int repetitions)
{
  cedar::dyn::NeuralFieldPtr field(new cedar::dyn::NeuralField());
  field->setDimensionality(dim);
  for (unsigned int i = 0; i < dim; ++i)
  {
    field->setSize(i, 50);
  }

  std::string id;
  id += "dimensions: " + cedar::aux::toString(dim);
  id += ", repetitions: " + cedar::aux::toString(repetitions);
  cedar::test::test_time
  (
    id,
    boost::bind(&cedar::proc::Step::onTrigger, field, cedar::proc::StepTimePtr(new cedar::proc::StepTime(0.001 * cedar::unit::seconds)), cedar::proc::TriggerPtr()),
    repetitions
  );

  QApplication::exit(0); // no errors -- this is a performance test.
}

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  cedar::aux::CallFunctionInThread caller(boost::bind(&measure, 3, 100));
  caller.start();
//  measure(2, 20000);
//  measure(3, 100);
  return app.exec();
}
