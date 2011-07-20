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

    File:        NetworkFile.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 19

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "processing/gui/NetworkFile.h"
#include "processing/gui/Connection.h"
#include "processing/gui/StepItem.h"
#include "processing/Step.h"
#include "processing/Data.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/property_tree/json_parser.hpp>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::NetworkFile::NetworkFile(cedar::proc::gui::Scene* pScene)
:
mNetwork(new cedar::proc::Network()),
mpScene(pScene)
{
}

cedar::proc::gui::NetworkFile::~NetworkFile()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::NetworkFile::addToScene()
{
  std::vector<cedar::proc::StepPtr> steps_to_connect;
  // add step-items for steps that don't have one yet (i.e., for which none was present in the configuration tree)
  for (size_t i = 0; i < this->mNetwork->steps().size(); ++i)
  {
    cedar::proc::StepPtr& step = this->mNetwork->steps().at(i);
    if (this->mpScene->stepMap().find(step.get()) == this->mpScene->stepMap().end())
    {
      this->mpScene->addProcessingStep(step, QPointF(0, 0));
      steps_to_connect.push_back(step);
    }
  }

  for (size_t i = 0; i < steps_to_connect.size(); ++i)
  {
    cedar::proc::StepPtr& step = steps_to_connect.at(i);
    cedar::proc::Step::SlotMap& slot_map = step->getDataSlots(cedar::proc::DataRole::INPUT);
    for (cedar::proc::Step::SlotMap::iterator iter = slot_map.begin(); iter != slot_map.end(); ++iter)
    {
      cedar::proc::Step *p_owner = iter->second.getData()->getOwner();
      if (!p_owner)
      {
        std::cout << "warning: during loading, the owner of a data connection was NULL." << std::endl;
//          continue;
      }
      cedar::proc::gui::StepItem* p_source = this->mpScene->getStepItemFor(p_owner);
      if (!p_source)
      {
        std::cout << "warning: during loading, the source of a data connection was NULL." << std::endl;
//          continue;
      }
      cedar::proc::gui::StepItem* p_target = this->mpScene->getStepItemFor(step.get());
      if (!p_target)
      {
        std::cout << "warning: during loading, the target of a data connection was NULL." << std::endl;
//          continue;
      }
      Connection *p_connection = new Connection(p_source, p_target);
      this->mpScene->addItem(p_connection);
    }
  }
}

cedar::proc::NetworkPtr cedar::proc::gui::NetworkFile::network()
{
  return this->mNetwork;
}

void cedar::proc::gui::NetworkFile::save(const std::string& destination)
{
  cedar::aux::ConfigurationNode root;

  this->mNetwork->saveTo(root);
  this->saveScene(root);

  write_json(destination, root);
}

void cedar::proc::gui::NetworkFile::load(const std::string& source)
{
  cedar::aux::ConfigurationNode root;
  read_json(source, root);

  this->mNetwork->readFrom(root);
}

void cedar::proc::gui::NetworkFile::saveScene(cedar::aux::ConfigurationNode /* root */)
{
  // TODO implement me
}


void cedar::proc::gui::NetworkFile::loadScene(cedar::aux::ConfigurationNode /* root */)
{
  // TODO implement me
}
