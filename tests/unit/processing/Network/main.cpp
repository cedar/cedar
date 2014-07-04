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
    std::string new_name = network->getUniqueName("stepB");
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

  // testing auto-promoting of slots with multiple incoming or outgoing connections (issue #270)
  cedar::proc::NetworkPtr network_promote(new cedar::proc::Network());
  network_promote->setName("root");
  cedar::proc::NetworkPtr network_promoted_nested(new cedar::proc::Network());
  network_promoted_nested->setName("nested");
  std::list<cedar::proc::ElementPtr> to_add;
  to_add.push_back(network_promoted_nested);
  cedar::dyn::NeuralFieldPtr field_a(new cedar::dyn::NeuralField());
  field_a->setName("field a");
  to_add.push_back(field_a);
  cedar::dyn::NeuralFieldPtr field_b(new cedar::dyn::NeuralField());
  field_b->setName("field b");
  to_add.push_back(field_b);
  cedar::dyn::NeuralFieldPtr field_c(new cedar::dyn::NeuralField());
  field_c->setName("field c");
  to_add.push_back(field_c);
  network_promote->add(to_add);
  network_promote->connectSlots("field a.sigmoided activation", "field b.input");
  network_promote->connectSlots("field a.sigmoided activation", "field c.input");
  std::list<cedar::proc::ElementPtr> to_move;
  to_move.push_back(field_a);
  network_promoted_nested->add(to_move);


  // return
  std::cout << "Done. There were " << errors << " errors." << std::endl;
  return errors;
}
