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

    File:        componentMultiply.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2014 10 01

    Description: Test component multiply performance.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "cedar/configuration.h"
#include "cedar/processing/sources/GaussInput.h"
#include "cedar/processing/steps/ComponentMultiply.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/Group.h"
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

void measure(const std::string& stepName, unsigned int repetitions)
{
  using cedar::proc::Group;
  using cedar::proc::GroupPtr;
  using cedar::proc::steps::ComponentMultiply;
  using cedar::proc::steps::ComponentMultiplyPtr;

  GroupPtr group(new Group());
  group->readJson("base.json");

  event_loop();

  auto cm = group->getElement<ComponentMultiply>(stepName);

  std::string id;
  id += stepName;
  cedar::test::test_time
  (
    id,
    boost::bind(&cedar::proc::Step::onTrigger, cm, cedar::proc::ArgumentsPtr(), cedar::proc::TriggerPtr()),
    repetitions
  );

  if (cm->getState() == cedar::proc::Triggerable::STATE_EXCEPTION)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Configuration \"" + id + "\" resulted in an exception.",
      "void measure()"
    );
  }
}

int main(int, char**)
{
  unsigned int repetitions = 100;

  measure("cm 2d", repetitions);
  measure("cm 3d", repetitions);

  return 0; // no errors -- this is a performance test.
}
