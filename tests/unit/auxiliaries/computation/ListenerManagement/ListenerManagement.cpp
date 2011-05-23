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
#include "auxiliaries/computation/Trigger.h"
#include "auxiliaries/LogFile.h"

// SYSTEM INCLUDES
#include <iostream>

class TriggerableTest : public cedar::aux::comp::Triggerable
{
  void onTrigger(const cedar::aux::comp::Arguments& arguments)
  {
    std::cout << "onTrigger called." << std::endl;
  }
};

typedef boost::shared_ptr<TriggerableTest> TriggerableTestPtr;

int main(int argc, char** argv)
{
  using cedar::aux::LogFile;
  using cedar::aux::comp::Trigger;
  using cedar::aux::comp::TriggerPtr;

  unsigned int errors = 0;

  LogFile log_file("ListenerManagement.log");
  log_file.addTimeStamp();
  log_file << std::endl;

  log_file << "Creating triggerable." << std::endl;
  TriggerableTestPtr triggerable (new TriggerableTest());

  log_file << "Creating trigger." << std::endl;
  TriggerPtr trigger (new Trigger());

  log_file << "Adding triggerable." << std::endl;
  trigger->addListener(triggerable);

  log_file << "Adding triggerable again." << std::endl;
  trigger->addListener(triggerable);

  if (trigger->getListeners().size() != 1)
  {
    ++errors;
    log_file << "Error: same listener was added twice." << std::endl;
  }

  log_file << "Removing triggerable." << std::endl;
  trigger->removeListener(triggerable);
  if (trigger->getListeners().size() != 0)
  {
    ++errors;
    log_file << "Error: listener was not removed correctly." << std::endl;
  }

  log_file << "Adding a second triggerable." << std::endl;
  trigger->addListener(triggerable);
  trigger->addListener(TriggerableTestPtr(new TriggerableTest()));
  if (trigger->getListeners().size() != 2)
  {
    ++errors;
    log_file << "Error: adding two triggers doesn't work." << std::endl;
  }

  log_file << "Done. There were " << errors << " errors." << std::endl;

  return errors;
}
