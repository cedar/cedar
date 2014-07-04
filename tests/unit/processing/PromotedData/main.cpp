/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
    Date:        2012 01 31

    Description: 

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/Element.h"
#include "cedar/processing/Network.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/OwnedData.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/MatData.h"

// SYSTEM INCLUDES
#include <iostream>

class TestSource : public cedar::proc::Step
{
public:
  TestSource()
  :
  mOutput(new cedar::aux::MatData(cv::Mat(20,20,CV_32F))),
  mTriggerCounter(0)
  {
    this->declareOutput("output", mOutput);
  }
  void compute(const cedar::proc::Arguments&)
  {
    ++mTriggerCounter;
  };
  unsigned int getTriggerCounter()
  {
    return mTriggerCounter;
  }
private:
  //!@brief The data containing the output.
  cedar::aux::MatDataPtr mOutput;
  unsigned int mTriggerCounter;
};

class TestClass : public cedar::proc::Step
{
public:
  TestClass()
  :
  mOutput(new cedar::aux::MatData(cv::Mat(20,20,CV_32F))),
  mTriggerCounter(0)
  {
    this->declareInput("input");
    this->declareOutput("output", mOutput);
  }
  void compute(const cedar::proc::Arguments&)
  {
    ++mTriggerCounter;
  };
  unsigned int getTriggerCounter()
  {
    return mTriggerCounter;
  }
private:
  //!@brief The data containing the output.
  cedar::aux::MatDataPtr mOutput;
  unsigned int mTriggerCounter;
};

CEDAR_GENERATE_POINTER_TYPES(TestSource);
CEDAR_GENERATE_POINTER_TYPES(TestClass);

int main(int, char**)
{
  // the number of errors encountered in this test
  int errors = 0;
  
  cedar::proc::ElementDeclarationPtr test_decl
                                     (
                                       new cedar::proc::ElementDeclarationTemplate<TestClass>("blubber","TestClass")
                                     );
  test_decl->setIconPath(":/steps/resize.svg");
  test_decl->declare();

  cedar::proc::NetworkPtr root(new cedar::proc::Network());
  TestSourcePtr rootClass(new TestSource());
  rootClass->setName("rootClass");
  root->add(rootClass);

  cedar::proc::NetworkPtr network(new cedar::proc::Network());
  network->setName("network");
  TestClassPtr childClass(new TestClass());
  childClass->setName("childClass");
  network->add(childClass);
  network->promoteSlot(childClass->getInputSlot("input"));
  network->promoteSlot(childClass->getOutputSlot("output"));

  root->add(network);
  root->connectSlots("rootClass.output", "network.childClass.input");

  std::cout << "trigger count before: " << childClass->getTriggerCounter() << std::endl;
  rootClass->onTrigger();
  std::cout << "trigger count after: " << childClass->getTriggerCounter() << std::endl;

  std::cout << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
