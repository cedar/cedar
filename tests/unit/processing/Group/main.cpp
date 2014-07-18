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
#include "cedar/processing/Group.h"
#include "cedar/processing/GroupDeclaration.h"
#include "cedar/processing/GroupDeclarationManager.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/Arguments.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/processing/sources/GaussInput.h"
#include "cedar/processing/steps/StaticGain.h"
#include "cedar/dynamics/fields/NeuralField.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/auxiliaries/sleepFunctions.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <iostream>
#include <list>

unsigned int errors = 0;

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

void run_test()
{
  using cedar::proc::Group;
  using cedar::proc::StepPtr;
  using cedar::proc::Step;


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
  cedar::proc::GroupPtr network(new cedar::proc::Group());
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
  cedar::proc::GroupPtr network_parent(new cedar::proc::Group());
  TestModulePtr step_parent (new TestModule());
  network_parent->add(step_parent, "parent step");
  cedar::proc::GroupPtr network_child(new cedar::proc::Group());
  network_child->setName("network child");
  TestModulePtr step_child (new TestModule());
  network_child->add(step_child, "child step");
  cedar::proc::LoopedTriggerPtr trigger(new cedar::proc::LoopedTrigger());
  network_child->add(trigger, "looped_trigger");
  network_child->connectTrigger(trigger, step_child);
  cedar::proc::GroupPtr network_grand_child(new cedar::proc::Group());
  network_grand_child->setName("network grand child");
  network_child->add(network_grand_child);
  network_parent->add(network_child);

  std::cout << "Write nested network." << std::endl;
  network_parent->writeJson("Nested.json");
  std::cout << "Read nested network." << std::endl;
  cedar::proc::GroupPtr network_nested(new cedar::proc::Group());
  network_nested->readJson("Nested.json");

  std::cout << "testing Group::getElement for nested networks" << std::endl;
  network_nested->getElement<Step>("parent step");
  if (network_nested->getElement<Group>("network child")->getElement<Step>("child step")->getName() != "child step")
  {
    ++errors;
    std::cout << "child step was not found in nested network" << std::endl;
  }
  if (network_nested->getElement<Step>("network child.child step")->getName() != "child step")
  {
    ++errors;
    std::cout << "child step was not found in nested network using dot notation" << std::endl;
  }
  if (network_nested->getElement<Group>("network child.network grand child")->getName() != "network grand child")
  {
    ++errors;
    std::cout << "child step was not found in nested network using dot notation" << std::endl;
  }

  std::cout << "testing Group::findPath" << std::endl;
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

  // testing a bug
  cedar::proc::GroupPtr network_upper(new cedar::proc::Group());
  network_upper->setName("root");
  cedar::proc::GroupPtr network_lower(new cedar::proc::Group());
  network_lower->setName("nested");
  network_upper->add(network_lower);
  cedar::proc::steps::StaticGainPtr gain_lower(new cedar::proc::steps::StaticGain());
  cedar::proc::steps::StaticGainPtr gain_upper(new cedar::proc::steps::StaticGain());
  network_upper->add(gain_lower, "gain lower");
  network_upper->add(gain_upper, "gain upper");
  network_upper->connectSlots("gain lower.output", "gain upper.input");
  std::list<cedar::proc::ElementPtr> to_move;
  to_move.push_back(gain_lower);
  network_lower->add(to_move);
  network_upper->add(to_move);

  // testing network connectors
  cedar::proc::GroupPtr network_root(new cedar::proc::Group());
  network_root->setName("root");
  cedar::proc::GroupPtr network_connector(new cedar::proc::Group());
  network_connector->setName("nested");
  network_root->add(network_connector);
  network_connector->addConnector("input", true);
  network_connector->addConnector("output", false);
  cedar::proc::sources::GaussInputPtr gauss(new cedar::proc::sources::GaussInput());
  network_root->add(gauss, "Gauss");
  network_root->connectSlots("Gauss.Gauss input", "nested.input");
  network_connector->connectSlots("input.output", "output.input");
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
  network_root = cedar::proc::GroupPtr(new cedar::proc::Group());
  network_root->setName("root");
  network_connector = cedar::proc::GroupPtr(new cedar::proc::Group());
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
  network_root = cedar::proc::GroupPtr(new cedar::proc::Group());
  network_root->setName("root");
  network_connector = cedar::proc::GroupPtr(new cedar::proc::Group());
  network_connector->setName("nested");
  cedar::proc::GroupPtr network_connector_2(new cedar::proc::Group());
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

  // test the 'connect across groups' functionality
  cedar::proc::GroupPtr network_base(new cedar::proc::Group());
  network_base->setName("root");
  cedar::proc::GroupPtr network_left(new cedar::proc::Group());
  network_left->setName("left");
  network_base->add(network_left);
  cedar::proc::GroupPtr network_right(new cedar::proc::Group());
  network_right->setName("right");
  network_base->add(network_right);
  cedar::proc::steps::StaticGainPtr gain_left(new cedar::proc::steps::StaticGain());
  network_left->add(gain_left, "left");
  cedar::proc::steps::StaticGainPtr gain_right(new cedar::proc::steps::StaticGain());
  network_left->add(gain_right, "right");
  auto output_slot = gain_left->getOutputSlot("output");
  auto input_slot = gain_right->getInputSlot("input");
  network_left->connectSlots(output_slot, input_slot);
  to_move.clear();
  to_move.push_back(gain_right);
  network_right->add(to_move);
  network_left->add(to_move);
  network_left->disconnectSlots(output_slot, input_slot);

  // test importing networks
  std::cout << "testing importing groups" << std::endl;
  cedar::proc::GroupPtr network_importing(new cedar::proc::Group());
  network_importing->importGroupFromFile("network child", "Nested.json");

  cedar::proc::GroupDeclarationManagerSingleton::getInstance()->addGroupTemplateToGroup("two-layer field", network_importing);

  // test connecting triggers to groups
  std::cout << "testing connecting triggers to groups" << std::endl;
  cedar::proc::GroupPtr group_trigger(new cedar::proc::Group());
  cedar::proc::GroupPtr group_triggered(new cedar::proc::Group());
  cedar::proc::LoopedTriggerPtr new_trigger(new cedar::proc::LoopedTrigger());
  group_trigger->add(group_triggered, "group");
  group_trigger->add(new_trigger, "trigger");
  group_trigger->connectTrigger(new_trigger, group_triggered);
  new_trigger->start();
  cedar::aux::sleep(0.5 * cedar::unit::seconds);
  new_trigger->stop();

  std::cout << "testing moving of some steps" << std::endl;
  cedar::proc::GroupPtr group_1(new cedar::proc::Group());
  cedar::proc::GroupPtr group_2(new cedar::proc::Group());
  cedar::proc::steps::StaticGainPtr gain_1(new cedar::proc::steps::StaticGain());
  cedar::proc::steps::StaticGainPtr gain_2(new cedar::proc::steps::StaticGain());
  cedar::proc::steps::StaticGainPtr gain_3(new cedar::proc::steps::StaticGain());
  group_1->add(group_2, "group");
  group_1->add(gain_1, "gain 1");
  group_1->add(gain_2, "gain 2");
  group_1->add(gain_3, "gain 3");
  group_1->connectSlots("gain 1.output", "gain 2.input");
  group_1->connectSlots("gain 1.output", "gain 3.input");
  std::list<cedar::proc::ElementPtr> moved_gains;
  moved_gains.push_back(gain_1);
  moved_gains.push_back(gain_3);
  group_2->add(moved_gains);
  group_1->add(moved_gains);

  {
    std::cout << "testing renaming of group connectors" << std::endl;

    cedar::proc::GroupPtr group(new cedar::proc::Group());
    group->addConnector("test_in", true);
    group->renameConnector("test_in", "test_in2", true);
    group->addConnector("test_out", false);
    group->renameConnector("test_out", "test_out2", false);
  }


  // return
  std::cout << "Done. There were " << errors << " errors." << std::endl;

  QApplication::exit(0);
}

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  cedar::aux::CallFunctionInThread caller(boost::bind(&run_test));

  caller.start();

  app.exec();

  return errors;
}
