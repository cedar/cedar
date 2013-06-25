/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        ArchitectureIO.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 10 14

    Description: Unit test that first writes a processing architecture to a file and then attempts to load it again.

    Credits:

======================================================================================================================*/

#include "cedar/processing/namespace.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/Network.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/DoubleData.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/units/Time.h"


class TestModule : public cedar::proc::Step
{
  public:
    TestModule()
    {
      this->declareInput("input1", true);
      this->declareInput("input2", true);
    }

    void compute(const cedar::proc::Arguments&)
    {
      // access both inputs, this makes sure that the step is not computed when it shouldn't be
      double data1 = this->getInput("input1")->getData<double>();
      std::cout << "input1:" << data1 << std::endl;
      double data2 = this->getInput("input2")->getData<double>();
      std::cout << "input2:" << data2 << std::endl;
    }
};
CEDAR_GENERATE_POINTER_TYPES(TestModule);

class TestSource : public cedar::proc::Step
{
  public:
    TestSource(double value)
    :
    mOutput(new cedar::aux::DoubleData(value))
    {
      this->declareOutput("output", mOutput);
    }

    void compute(const cedar::proc::Arguments&)
    {
      // nothing to do here
    }

    cedar::aux::DoubleDataPtr mOutput;
};
CEDAR_GENERATE_POINTER_TYPES(TestSource);


int testOnlineDisconnecting()
{
  int errors = 0;

  std::cout << "Testing projection for online removal ..." << std::endl;

  size_t trials = 10;

  for (size_t i = 0; i < trials; ++i)
  {
    cedar::proc::NetworkPtr network(new cedar::proc::Network());
    network->readFile("projection.json");

    auto trigger = network->getElement<cedar::proc::LoopedTrigger>("trigger");

    trigger->startTrigger();

    cedar::aux::sleep(cedar::unit::Milliseconds(50));

    network->disconnectSlots("step.result", "projection.input");

    cedar::aux::sleep(cedar::unit::Milliseconds(50));

    trigger->stopTrigger();
  }

  for (size_t i = 0; i < trials; ++i)
  {
    std::cout << "Testing double-triggered case ..." << std::endl;
    cedar::proc::NetworkPtr network(new cedar::proc::Network());
    network->readFile("taste_the_double_trigger.json");

    auto trigger1 = network->getElement<cedar::proc::LoopedTrigger>("trigger1");
    auto trigger2 = network->getElement<cedar::proc::LoopedTrigger>("trigger2");

    trigger1->startTrigger();
    trigger2->startTrigger();

    cedar::aux::sleep(cedar::unit::Milliseconds(50));

    network->disconnectSlots("step.Gauss input", "projection.input");

    cedar::aux::sleep(cedar::unit::Milliseconds(50));

    trigger1->stopTrigger();
    trigger2->stopTrigger();
  }

  std::cout << "Online (dis-)connecting revealed " << errors << " error(s)." << std::endl;
  return errors;
}


int main(int, char**)
{
  unsigned int errors = 0;

  cedar::proc::NetworkPtr network (new cedar::proc::Network());
  network->add(TestSourcePtr(new TestSource(1)), "source1");
  network->add(TestSourcePtr(new TestSource(2)), "source2");
  network->add(TestModulePtr(new TestModule()), "target");

  network->connectSlots("source1.output", "target.input1");
  network->connectSlots("source2.output", "target.input2");

  errors += testOnlineDisconnecting();

  std::cout << "Done. There were " << errors << " error(s)." << std::endl;
  return errors;
}
