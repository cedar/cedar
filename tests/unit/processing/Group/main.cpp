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
#include "cedar/processing/sources/Noise.h"
#include "cedar/processing/steps/StaticGain.h"
#include "cedar/dynamics/fields/NeuralField.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/units/Time.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <iostream>
#include <list>

unsigned int errors = 0;

class TestModule : public cedar::proc::Step
{
public:
  TestModule()
  :
  cedar::proc::Step(true)
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

class FieldCheat : public cedar::dyn::NeuralField
{
public:
  bool inputsValid()
  {
    return this->allInputsValid();
  }
};
CEDAR_GENERATE_POINTER_TYPES(FieldCheat);

int test_input_revalidation()
{
  std::cout << "Testing revalidation of group slots." << std::endl;
  int errors = 0;
  cedar::proc::GroupPtr root(new cedar::proc::Group());
  cedar::proc::GroupPtr group(new cedar::proc::Group());
  root->add(group, "group");
  group->addConnector("input", true);

  FieldCheatPtr field1(new FieldCheat());
  FieldCheatPtr field2(new FieldCheat());
  root->add(field1, "field1");
  group->add(field2, "field2");

  root->connectSlots("field1.sigmoided activation", "group.input");
  group->connectSlots("input.output", "field2.input");

  field1->setDimensionality(field1->getDimensionality() - 1);

  if (field2->inputsValid())
  {
    std::cout << "ERROR: validity of nested field was not updated properly." << std::endl;
    ++errors;
  }


  return errors;
}

int test_name_exists()
{
  std::cout << "-- Testing nameExists() --" << std::endl << std::endl;
  int errors = 0;
  cedar::proc::GroupPtr root(new cedar::proc::Group());
  cedar::proc::GroupPtr subgroup1(new cedar::proc::Group());
  cedar::proc::GroupPtr subgroup2(new cedar::proc::Group());
  root->add(subgroup1, "subgroup1");
  subgroup1->add(subgroup2, "subgroup2");

  TestModulePtr test_1(new TestModule());
  root->add(test_1, "test");
  TestModulePtr test_2(new TestModule());
  subgroup1->add(test_2, "test");
  TestModulePtr test_3(new TestModule());
  subgroup2->add(test_3, "test");

  std::vector<std::string> names_to_check;
  names_to_check.push_back("test");
  names_to_check.push_back("subgroup1");
  names_to_check.push_back("subgroup1.test");
  names_to_check.push_back("subgroup1.subgroup2");
  names_to_check.push_back("subgroup1.subgroup2.test");

  for (const auto& name : names_to_check)
  {
    if (!root->nameExists(name))
    {
      std::cout << "ERROR: name \"" + name + "\" not found in root group." << std::endl;
      ++errors;
    }
    else
    {
      std::cout << "name \"" + name + "\" properly found." << std::endl;
    }
  }

  return errors;
}

int test_looped_group_cycle()
{
  std::cout << "Testing a cycle in connections between looped groups." << std::endl;
  cedar::proc::GroupPtr root(new cedar::proc::Group());
  cedar::proc::GroupPtr group1(new cedar::proc::Group());
  cedar::proc::GroupPtr group2(new cedar::proc::Group());

  root->add(group1, "group1");
  root->add(group2, "group2");

  cedar::proc::LoopedTriggerPtr trigger(new cedar::proc::LoopedTrigger());
  root->add(trigger, "trigger");

  group1->addConnector("input", true);
  group2->addConnector("input", true);
  group1->addConnector("output", false);
  group2->addConnector("output", false);

  root->connectSlots("group1.output", "group2.input");
  root->connectSlots("group2.output", "group1.input");
  group1->connectSlots("input.output", "output.input");
  group2->connectSlots("input.output", "output.input");

  std::cout << "Starting triggers" << std::endl;
  trigger->start();

  std::cout << "Waiting." << std::endl;
  cedar::aux::sleep(0.25 * cedar::unit::seconds);

  std::cout << "Stopping triggers" << std::endl;
  trigger->stop();

  return 0;
}

int test_connector_renaming()
{
  using cedar::proc::Group;
  using cedar::proc::GroupPtr;
  using cedar::proc::Element;
  using cedar::proc::ElementPtr;

  int errors = 0;
  std::cout << "testing renaming of group connectors" << std::endl;

  { // simple renaming
    cedar::proc::GroupPtr group(new cedar::proc::Group());
    group->addConnector("test_in", true);
    group->renameConnector("test_in", "test_in2", true);
    group->addConnector("test_out", false);
    group->renameConnector("test_out", "test_out2", false);
  }

  { // duplicate names
    GroupPtr test(new Group());
    test->addConnector("test", true);
    test->addConnector("test 2", true);
    try
    {
      std::cout << " trying to use a name that already exists" << std::endl;
      test->renameConnector("test", "test 2", true);
      std::cout << " ERROR: rename worked even though it shouldn't." << std::endl;
      ++errors;
    }
    catch (const cedar::aux::ExceptionBase& e)
    {
      std::cout << "Properly caught an exception: " << e.exceptionInfo() << std::endl;
    }
  }


  { // renaming & element names
    GroupPtr test(new Group());
    test->add(ElementPtr(new TestModule()), "test element");
    test->addConnector("test", true);

    try
    {
      std::cout << " trying rename with an element's name" << std::endl;
      test->renameConnector("test", "test element", true);
      std::cout << " ERROR: rename worked even though it shouldn't." << std::endl;
      ++errors;
    }
    catch (const cedar::aux::ExceptionBase& e)
    {
      std::cout << "Properly caught an exception: " << e.exceptionInfo() << std::endl;
    }

    try
    {
      std::cout << " trying to undo rename" << std::endl;
      test->renameConnector("test element", "test", true);
      std::cout << " ERROR: rename worked even though it shouldn't." << std::endl;
      ++errors;
    }
    catch (const cedar::aux::ExceptionBase& e)
    {
      std::cout << "Properly caught an exception: " << e.exceptionInfo() << std::endl;
    }

//    try
//    {
//      std::cout << " trying to use the name of an element for a connector" << std::endl;
//      test->renameConnector("test", "test element", true);
//      std::cout << " ERROR: rename worked even though it shouldn't." << std::endl;
//      ++errors;
//    }
//    catch (const cedar::aux::ExceptionBase& e)
//    {
//      std::cout << "Properly caught an exception: " << e.exceptionInfo() << std::endl;
//    }

    // try to give a different
  }
  return errors;
}

int test_looped_trigger_auto_connect_internal(cedar::proc::TriggerablePtr child)
{
  int errors = 0;
  cedar::proc::GroupPtr group_auto(new cedar::proc::Group());
  // hand looped element to group, this should create a default trigger
  group_auto->add(boost::dynamic_pointer_cast<cedar::proc::Element>(child), "child");
  // get the default trigger
  cedar::proc::LoopedTriggerPtr loopy;
  if (!group_auto->nameExists("default trigger"))
  {
    std::cout << "error: adding a looped element to a group did not create a looped trigger" << std::endl;
    return ++errors;
  }
  else
  {
    loopy = group_auto->getElement<cedar::proc::LoopedTrigger>("default trigger");
  }

  // parent trigger of looped element should be default trigger
  if (loopy != child->getLoopedTrigger())
  {
    std::cout << "error: adding a looped element to a group did not connect it to the default trigger" << std::endl;
    return ++errors;
  }

  // create a new trigger and add it to group
  cedar::proc::LoopedTriggerPtr pretender(new cedar::proc::LoopedTrigger());
  std::string name = group_auto->getUniqueIdentifier("pretender");
  group_auto->add(pretender, name);

  // do the following five times
  for (unsigned int i = 0; i < 5; ++ i)
  {
    // now connect the looped element to the new trigger
    group_auto->connectTrigger(pretender, child);
    if (child->getLoopedTrigger() != pretender)
    {
      std::cout << "error: looped element has the wrong parent trigger" << std::endl;
      ++errors;
    }
    // also check the other way around
    if (!pretender->isListener(child))
    {
      std::cout << "error: looped trigger does not know of its looped listener" << std::endl;
      ++errors;
    }

    if (loopy->isListener(child))
    {
      std::cout << "error: looped element still listens to default trigger" << std::endl;
      ++errors;
    }

    // now, back to the default trigger!
    group_auto->connectTrigger(loopy, child);
    if (child->getLoopedTrigger() != loopy)
    {
      std::cout << "error: looped element has the wrong parent trigger" << std::endl;
      ++errors;
    }
    // also check the other way around
    if (!loopy->isListener(child))
    {
      std::cout << "error: looped trigger does not know of its looped listener" << std::endl;
      ++errors;
    }

    if (pretender->isListener(child))
    {
      std::cout << "error: looped element still listens to custom trigger" << std::endl;
      ++errors;
    }
  }

  // now try the same thing, but this time, remove the looped trigger
  group_auto->connectTrigger(pretender, child);
  group_auto->remove(pretender);
  if (child->getLoopedTrigger() != loopy)
  {
    std::cout << "error: looped element has the wrong parent trigger after removing custom trigger" << std::endl;
    ++errors;
  }
  // also check the other way around
  if (!loopy->isListener(child))
  {
    std::cout << "error: looped trigger does not know of its looped listener after removing custom trigger" << std::endl;
    ++errors;
  }

  if (pretender->isListener(child))
  {
    std::cout << "error: looped element still listens to custom trigger after removing custom trigger" << std::endl;
    ++errors;
  }

  return errors;
}

int test_looped_trigger_auto_connect()
{
  int errors = 0;
  std::cout << "Testing trigger-auto-connect for a step" << std::endl;
  cedar::proc::sources::NoisePtr noise(new cedar::proc::sources::Noise());
  // noise should not have a parent trigger
  if (noise->getLoopedTrigger())
  {
    std::cout << "error: looped element has a parent trigger outside of a group" << std::endl;
    ++errors;
  }
  errors += test_looped_trigger_auto_connect_internal(noise);

  // now, let's do the same for groups - this will be fun!
  std::cout << "Testing trigger-auto-connect for a group" << std::endl;
  cedar::proc::GroupPtr childish_group(new cedar::proc::Group());
  childish_group->setIsLooped(true);
  if (noise->getLoopedTrigger())
  {
    std::cout << "error: looped element has a parent trigger outside of a group" << std::endl;
    ++errors;
  }

  errors += test_looped_trigger_auto_connect_internal(childish_group);

  return errors;
}

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
  network->readJson("test://unit/processing/Group/Sample.json");
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
    std::cout << "ERROR: Somehow got past a call that was not supposed to work." << std::endl;
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
    std::string new_name = network->duplicate("stepB");
    network->getElement(new_name);
  }
  catch(cedar::aux::ExceptionBase& e) // simple copy did not work
  {
    std::cout << "ERROR: simple duplication did not work" << std::endl;
    ++errors;
    std::cout << "Got an exception: " << e.exceptionInfo() << std::endl;
  }

  try
  {
    network->duplicate("stepB", "copied step B");
    network->getElement("copied step B");
  }
  catch(cedar::aux::ExceptionBase&) // named copy did not work
  {
    std::cout << "ERROR: named duplication did not work" << std::endl;
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

  std::cout << "Write nested group." << std::endl;
  network_parent->writeJson("test://unit/processing/Group/Nested.json");
  std::cout << "Read nested group." << std::endl;
  cedar::proc::GroupPtr network_nested(new cedar::proc::Group());
  network_nested->readJson("test://unit/processing/Group/Nested.json");

  std::cout << "testing Group::getElement for nested networks" << std::endl;
  network_nested->getElement<Step>("parent step");
  if (network_nested->getElement<Group>("network child")->getElement<Step>("child step")->getName() != "child step")
  {
    std::cout << "ERROR: child step was not found in nested network" << std::endl;
    ++errors;
  }
  if (network_nested->getElement<Step>("network child.child step")->getName() != "child step")
  {
    std::cout << "ERROR: child step was not found in nested network using dot notation" << std::endl;
    ++errors;
  }
  if (network_nested->getElement<Group>("network child.network grand child")->getName() != "network grand child")
  {
    std::cout << "ERROR: child step was not found in nested network using dot notation" << std::endl;
    ++errors;
  }

  std::cout << "testing Group::findPath" << std::endl;
  if (network_nested->findPath(network_nested->getElement<Step>("network child.child step")) != "network child.child step")
  {
    std::cout << "ERROR: path to existing element not recovered" << std::endl;
    ++errors;
  }

  TestModulePtr step_schmild (new TestModule());
  step_schmild->setName("schmild step");
  if (network_nested->findPath(step_schmild) != "")
  {
    std::cout << "ERROR: path to non-existing element not empty" << std::endl;
    ++errors;
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
  cedar::proc::GroupPtr group_importing(new cedar::proc::Group());
  group_importing->importGroupFromFile("network child", "test://unit/processing/Group/Nested.json");

  cedar::proc::GroupDeclarationPtr group_declaration
                                 (
                                   new cedar::proc::GroupDeclaration
                                   (
                                     "template",
                                     "test://unit/processing/Group/Template.json",
                                     "template",
                                     "test"
                                   )
                                 );
  group_declaration->declare();

  cedar::proc::GroupDeclarationManagerSingleton::getInstance()->addGroupTemplateToGroup("template", group_importing);
  //!@todo check, if the template was loaded correctly

  // test connecting triggers to groups
  std::cout << "testing connecting triggers to groups" << std::endl;
  cedar::proc::GroupPtr group_trigger(new cedar::proc::Group());
  cedar::proc::GroupPtr group_triggered(new cedar::proc::Group());
  group_triggered->setIsLooped(true);
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

  errors += test_looped_group_cycle();
  errors += test_input_revalidation();
  errors += test_connector_renaming();
  errors += test_name_exists();
  errors += test_looped_trigger_auto_connect();

  // return
  std::cout << "Done. There were " << errors << " errors." << std::endl;

  QApplication::exit(errors);
}

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  cedar::aux::CallFunctionInThread caller(boost::bind(&run_test));

  caller.start();

  app.exec();

  return errors;
}
