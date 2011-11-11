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

    File:        Group.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 13
    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/processing/Group.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/processing/Manager.h"
#include "cedar/processing/Step.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::Group::Group(const std::string& name)
:
mGroupTrigger(new cedar::proc::LoopedTrigger())
{
  this->setName(name);
}

cedar::proc::Group::~Group()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::Group::addStep(cedar::proc::StepPtr step)
{
  mSteps.insert(step);
  cedar::proc::Manager::getInstance().connect(mGroupTrigger, step);
}

void cedar::proc::Group::removeStep(cedar::proc::StepPtr step)
{
  mSteps.erase(step);
}

void cedar::proc::Group::addGroup(cedar::proc::GroupPtr group)
{
  this->mGroups.insert(group);
}

void cedar::proc::Group::removeGroup(cedar::proc::GroupPtr group)
{
  this->mGroups.erase(group);
}

void cedar::proc::Group::start()
{
  mGroupTrigger->start();
  for (ChildGroups::iterator iter = mGroups.begin(); iter != mGroups.end(); iter++)
  {
    (*iter)->start();
  }
}

void cedar::proc::Group::stop()
{
  mGroupTrigger->stop();
  for (ChildGroups::iterator iter = mGroups.begin(); iter != mGroups.end(); iter++)
  {
    (*iter)->stop();
  }
}

void cedar::proc::Group::wait()
{
  mGroupTrigger->wait();
  for (ChildGroups::iterator iter = mGroups.begin(); iter != mGroups.end(); iter++)
  {
    (*iter)->wait();
  }
}

cedar::proc::Group::ChildSteps& cedar::proc::Group::steps()
{
  return this->mSteps;
}

void cedar::proc::Group::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  this->cedar::aux::Configurable::readConfiguration(node);

  try
  {
    cedar::aux::ConfigurationNode step_children = node.get_child("steps");
    for (cedar::aux::ConfigurationNode::iterator iter = step_children.begin(); iter != step_children.end(); ++iter)
    {
      std::string step_name = iter->second.get_value<std::string>();
      //!@todo should this step pointer come from a network?
      cedar::proc::StepPtr step = cedar::proc::Manager::getInstance().steps().get(step_name);
      this->addStep(step);
    }
  }
  catch (const boost::property_tree::ptree_bad_path& e)
  {
    // this is ok -- a group may have no steps in it
  }
}

void cedar::proc::Group::saveConfiguration(cedar::aux::ConfigurationNode& root)
{
  this->cedar::aux::Configurable::writeConfiguration(root);

  cedar::aux::ConfigurationNode step_children;
  for (ChildSteps::const_iterator iter = this->mSteps.begin(); iter != this->mSteps.end(); ++iter)
  {
    step_children.push_back(cedar::aux::ConfigurationNode::value_type("",
                                                                      cedar::aux::ConfigurationNode((*iter)->getName())
                                                                      )
                            );
  }
  root.push_back(cedar::aux::ConfigurationNode::value_type("steps", step_children));
}
