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

    File:        ListenerManagement.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 05 23

    Description:

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/processing/Trigger.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/Group.h"

// SYSTEM INCLUDES
#include <iostream>

class StepTest : public cedar::proc::Step
{
  void compute(const cedar::proc::Arguments&)
  {
    std::cout << "compute called." << std::endl;
  }
};

typedef boost::shared_ptr<StepTest> StepTestPtr;

int main(int /* argc */, char** /* argv */)
{
  using cedar::proc::Trigger;
  using cedar::proc::TriggerPtr;

  unsigned int errors = 0;

  std::cout << "Creating network." << std::endl;
  cedar::proc::GroupPtr network (new cedar::proc::Group());

  std::cout << "Creating step." << std::endl;
  StepTestPtr step (new StepTest());
  step->setName("step");

  std::cout << "Creating trigger." << std::endl;
  TriggerPtr trigger (new Trigger());
  trigger->setName("trigger");

  network->add(step);
  network->add(trigger);

  std::cout << "Adding step." << std::endl;
  network->connectTrigger(trigger, step);

  std::cout << "Removing step." << std::endl;
  network->disconnectTrigger(trigger, step);
  if (trigger->getListeners().size() != 0)
  {
    ++errors;
    std::cout << "Error: listener was not removed correctly." << std::endl;
  }

  std::cout << "Adding a second step." << std::endl;
  std::cout << "Creating step 2." << std::endl;
  StepTestPtr step_2(new StepTest());
  step_2->setName("step_2");
  network->add(step_2);
  network->connectTrigger(trigger, step);
  network->connectTrigger(trigger, step_2);

  if (trigger->getListeners().size() != 2)
  {
    ++errors;
    std::cout << "Error: adding two steps doesn't work." << std::endl;
  }

  std::cout << "Done. There were " << errors << " errors." << std::endl;

  return errors;
}
