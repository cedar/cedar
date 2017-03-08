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

    File:        groupOnStartStop.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 07 31

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/units/Time.h"

// SYSTEM INCLUDES
#include <QApplication>
#ifdef CEDAR_USE_QT5
  #include <QtConcurrent/QtConcurrentRun>
#else
  #include <QtConcurrentRun>
#endif
#include <QFuture>


class TestStep : public cedar::proc::Step
{
  public:
    TestStep()
    :
    cedar::proc::Step(true)
    {
      slot = this->declareInput("input");
      this->declareOutput("output", cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat())));
    }

    cedar::proc::DataSlotPtr slot;

  private:
    void compute(const cedar::proc::Arguments&)
    {
    }
};
CEDAR_GENERATE_POINTER_TYPES(TestStep);


void run_test()
{
  using cedar::proc::Group;
  using cedar::proc::GroupPtr;
  using cedar::proc::LoopedTrigger;
  using cedar::proc::LoopedTriggerPtr;

  int errors = 0;

  GroupPtr root(new Group());

  GroupPtr sub_1(new Group());
  root->add(sub_1, "sub 1");
  sub_1->setIsLooped(true);

  GroupPtr sub_2(new Group());
  root->add(sub_2, "sub 2");
  sub_2->setIsLooped(true);

  LoopedTriggerPtr trigger(new LoopedTrigger());
  root->add(trigger, "trigger");


  TestStepPtr step_1(new TestStep());
  sub_1->add(step_1, "step 1");

  TestStepPtr step_2(new TestStep());
  sub_2->add(step_2, "step 2");

  sub_1->addConnector("input", true);
  sub_1->addConnector("output", false);

  sub_2->addConnector("input", true);
  sub_2->addConnector("output", false);

  sub_1->connectSlots("step 1.output", "output.input");
  root->connectSlots("sub 1.output", "sub 2.input");
  sub_2->connectSlots("input.output", "step 2.input");
  sub_2->connectSlots("step 2.output", "output.input");
  root->connectSlots("sub 2.output", "sub 1.input");
  sub_1->connectSlots("input.output", "step 1.input");

  root->connectTrigger(trigger, sub_1);
  root->connectTrigger(trigger, sub_2);

  std::cout << "starting trigger" << std::endl;
  root->startTriggers(true);

  std::cout << "stopping trigger" << std::endl;
  root->stopTriggers(true);

  std::cout << "Done. There were " << errors << " error(s)." << std::endl;
  QApplication::exit(errors);
}

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  cedar::aux::CallFunctionInThread test_thread(run_test);

  test_thread.start();

  return app.exec();
}
