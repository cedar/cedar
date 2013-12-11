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

    File:        main.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 06 24

    Description:

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/processing/Network.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/Arguments.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/processing/sources/GaussInput.h"
#include "cedar/processing/steps/StaticGain.h"
#include "cedar/dynamics/fields/NeuralField.h"

// SYSTEM INCLUDES
#include <iostream>
#include <list>


class TestModule : public cedar::proc::Step
{
public:
  TestModule()
  {
  }

  void compute(const cedar::proc::Arguments& /* arguments */)
  {
    std::cout << "processing " << this->getName() << " = " << this->_mMessage << std::endl;
  }

  void readConfiguration(const cedar::aux::ConfigurationNode& node)
  {
    this->cedar::proc::Step::readConfiguration(node);
    _mMessage = node.get<std::string>("outputString", "defaultOutputString");
  }

  std::string _mMessage;
};

CEDAR_GENERATE_POINTER_TYPES(TestModule);

int main(int /* argc */, char** /* argv */)
{
  using cedar::proc::Network;
  using cedar::proc::StepPtr;
  using cedar::proc::Step;

  unsigned int errors = 0;

  std::cout << "Creating step declaration ... ";
  cedar::proc::ElementDeclarationPtr test_module_decl
  (
    new cedar::proc::ElementDeclarationTemplate<TestModule>("Test")
  );
  std::cout << "done." << std::endl;

  std::cout << "Adding declaration to the registry ... ";
  test_module_decl->declare();
  std::cout << "done." << std::endl;

  std::cout << "Reading Sample.json ... ";
  cedar::proc::NetworkPtr network(new cedar::proc::Network());
  network->readJson("Sample.json");
  std::cout << "done." << std::endl;

  std::cout << "Trying to call compute functions ... ";
  StepPtr step_a = network->getElement<Step>("stepA");
  step_a->onTrigger();

  StepPtr step_b = network->getElement<Step>("stepB");
  step_b->onTrigger();
  std::cout << "done." << std::endl;

  std::cout << "Trying to access sub-element in a step (should not work)." << std::endl;
  try
  {
    // if this works ...
    network->getElement("stepB.foo");
    // ... increase error count.
    std::cout << "Somehow got past a call that was not supposed to work." << std::endl;
    ++errors;
  }
  catch(cedar::aux::ExceptionBase&) // this should throw some cedar exception.
  {
    std::cout << "Properly threw something." << std::endl;
  }

  // test duplicate
  //!@todo duplicate should return pointer to new object
  //!@todo check for null pointer, right element in right network, values
  std::cout << "test duplication of steps" << std::endl;
  try
  {
    std::string new_name = network->getUniqueIdentifier("stepB");
    network->duplicate("stepB");
    network->getElement(new_name);
  }
  catch(cedar::aux::ExceptionBase&) // simple copy did not work
  {
    std::cout << "simple duplication did not work" << std::endl;
    ++errors;
  }

  try
  {
    network->duplicate("stepB", "copied step B");
    network->getElement("copied step B");
  }
  catch(cedar::aux::ExceptionBase&) // named copy did not work
  {
    std::cout << "named duplication did not work" << std::endl;
    ++errors;
  }

  // test nested networks
  std::cout << "Test nested network." << std::endl;
  cedar::proc::NetworkPtr network_parent(new cedar::proc::Network());
  TestModulePtr step_parent (new TestModule());
  network_parent->add(step_parent, "parent step");
  cedar::proc::NetworkPtr network_child(new cedar::proc::Network());
  network_child->setName("network child");
  TestModulePtr step_child (new TestModule());
  network_child->add(step_child, "child step");
  cedar::proc::LoopedTriggerPtr trigger(new cedar::proc::LoopedTrigger());
  network_child->add(trigger, "looped_trigger");
  network_child->connectTrigger(trigger, step_child);
  cedar::proc::NetworkPtr network_grand_child(new cedar::proc::Network());
  network_grand_child->setName("network grand child");
  network_child->add(network_grand_child);
  network_parent->add(network_child);

  std::cout << "Write nested network." << std::endl;
  network_parent->writeJson("Nested.json");
  std::cout << "Read nested network." << std::endl;
  cedar::proc::NetworkPtr network_nested(new cedar::proc::Network());
  network_nested->readJson("Nested.json");

  std::cout << "testing Network::getElement for nested networks" << std::endl;
  network_nested->getElement<Step>("parent step");
  if (network_nested->getElement<Network>("network child")->getElement<Step>("child step")->getName() != "child step")
  {
    ++errors;
    std::cout << "child step was not found in nested network" << std::endl;
  }
  if (network_nested->getElement<Step>("network child.child step")->getName() != "child step")
  {
    ++errors;
    std::cout << "child step was not found in nested network using dot notation" << std::endl;
  }
  if (network_nested->getElement<Network>("network child.network grand child")->getName() != "network grand child")
  {
    ++errors;
    std::cout << "child step was not found in nested network using dot notation" << std::endl;
  }

  std::cout << "testing Network::findPath" << std::endl;
  if (network_nested->findPath(network_nested->getElement<Step>("network child.child step")) != "network child.child step")
  {
    ++errors;
    std::cout << "path to existing element not recovered" << std::endl;
  }

  TestModulePtr step_schmild (new TestModule());
  step_schmild->setName("schmild step");
  if (network_nested->findPath(step_schmild) != "")
  {
    ++errors;
    std::cout << "path to non-existing element not empty" << std::endl;
  }

  std::cout << "testing connectors" << std::endl;
  // testing network connectors
  cedar::proc::NetworkPtr network_root(new cedar::proc::Network());
  network_root->setName("root");
  cedar::proc::NetworkPtr network_connector(new cedar::proc::Network());
  network_connector->setName("nested");
  network_root->add(network_connector);
  network_connector->addConnector("input", true);
//  network_connector->addConnector("another input", true);
  network_connector->addConnector("output", false);
//  network_connector->addConnector("another output", false);
  cedar::proc::sources::GaussInputPtr gauss(new cedar::proc::sources::GaussInput());
  network_root->add(gauss, "Gauss");
  network_root->connectSlots("Gauss.Gauss input", "nested.input");
//  network_root->connectSlots("Gauss.Gauss input", "nested.another input");
  network_connector->connectSlots("input.output", "output.input");
//  network_connector->connectSlots("another input.output", "another output.input");
  cedar::proc::steps::StaticGainPtr gain(new cedar::proc::steps::StaticGain());
  network_root->add(gain, "gain");
  network_root->connectSlots("nested.output", "gain.input");
  network_connector->disconnectSlots("input.output", "output.input");
  network_connector->connectSlots("input.output", "output.input");
  network_root->disconnectSlots("Gauss.Gauss input", "nested.input");
  network_root->connectSlots("Gauss.Gauss input", "nested.input");

  std::cout << "testing moving elements between networks" << std::endl;
  network_connector.reset();
  network_root.reset();
  network_root = cedar::proc::NetworkPtr(new cedar::proc::Network());
  network_root->setName("root");
  network_connector = cedar::proc::NetworkPtr(new cedar::proc::Network());
  network_connector->setName("nested");
  network_root->add(network_connector);
  for (unsigned int i = 0; i < 4; ++i)
  {
    cedar::proc::steps::StaticGainPtr gain(new cedar::proc::steps::StaticGain());
    network_root->add(gain, cedar::aux::toString(i));
  }
  network_root->connectSlots("0.output", "1.input");
  network_root->connectSlots("1.output", "2.input");
  network_root->connectSlots("2.output", "3.input");
  std::list<cedar::proc::ElementPtr> list;
  list.push_back(network_root->getElement("1"));
  list.push_back(network_root->getElement("2"));
  network_connector->add(list);
  network_connector->disconnectSlots("1.output", "2.input");
  list.erase(list.begin());
  network_root->add(list);

  // next test
  network_connector.reset();
  network_root.reset();
  network_root = cedar::proc::NetworkPtr(new cedar::proc::Network());
  network_root->setName("root");
  network_connector = cedar::proc::NetworkPtr(new cedar::proc::Network());
  network_connector->setName("nested");
  cedar::proc::NetworkPtr network_connector_2(new cedar::proc::Network());
  network_connector_2->setName("nested 2");
  network_root->add(network_connector);
  network_connector->add(network_connector_2);
  cedar::proc::steps::StaticGainPtr gain_out(new cedar::proc::steps::StaticGain());
  network_root->add(gain_out, "out");
  cedar::proc::steps::StaticGainPtr gain_in(new cedar::proc::steps::StaticGain());
  network_root->add(gain_in, "in");
  network_root->connectSlots("out.output", "in.input");
  list.clear();
  list.push_back(gain_out);
  network_connector_2->add(list);
  network_connector->add(list);
  network_root->add(list);

  // return
  std::cout << "Done. There were " << errors << " errors." << std::endl;
  return errors;
}
