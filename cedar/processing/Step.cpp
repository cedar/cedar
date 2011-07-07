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

    File:        Step.cpp

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
#include "processing/Step.h"
#include "processing/Arguments.h"
#include "processing/Data.h"
#include "auxiliaries/macros.h"
#include "processing/exceptions.h"
#include "processing/Manager.h"
#include "auxiliaries/Parameter.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Step::Step(bool runInThread, bool autoConnectTriggers)
:
mFinished(new cedar::proc::Trigger()),
mAutoConnectTriggers (autoConnectTriggers),
mBusy(false),
mMandatoryConnectionsAreSet (true),
mName (new cedar::aux::StringParameter("name", "")),
mRunInThread(runInThread)
{
  this->registerParameter(mName);
}

cedar::proc::Step::DataEntry::DataEntry(bool isMandatory)
:
mMandatory (isMandatory)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::Step::DataEntry::setData(DataPtr data)
{
  this->mData = data;
}

cedar::proc::DataPtr cedar::proc::Step::DataEntry::getData()
{
  return this->mData;
}

boost::shared_ptr<const cedar::proc::Data> cedar::proc::Step::DataEntry::getData() const
{
  return this->mData;
}

bool cedar::proc::Step::DataEntry::isMandatory() const
{
  return this->mMandatory;
}

const cedar::proc::Step::SlotMap& cedar::proc::Step::getDataSlots(DataRole::Id role) const
{
  std::map<DataRole::Id, SlotMap>::const_iterator iter = this->mDataConnections.find(role);
  if (iter == this->mDataConnections.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException, "Role "
                                                   + DataRole::type().get(role).name()
                                                   + " not found in cedar::proc::Step::getData(DataRole::Id).");
  }
  return iter->second;
}

void cedar::proc::Step::setName(const std::string& name)
{
  this->mName->set(name);
}

const std::string& cedar::proc::Step::getName() const
{
  return this->mName->get();
}

const cedar::proc::Step::ParameterMap& cedar::proc::Step::getParameters() const
{
  return this->mParameters;
}

cedar::proc::Step::ParameterMap& cedar::proc::Step::getParameters()
{
  return this->mParameters;
}

void cedar::proc::Step::registerParameter(cedar::aux::ParameterBasePtr parameter)
{
  //! @todo check for duplicate names
  //! @todo make sure there are no dots in the name; make a global function for name checks.
  this->mParameters[parameter->getName()] = parameter;
}

void cedar::proc::Step::checkMandatoryConnections()
{
  mMandatoryConnectionsAreSet = true;
  for (std::map<DataRole::Id, SlotMap>::iterator slot = this->mDataConnections.begin();
       slot != this->mDataConnections.end();
       ++slot)
  {
    for (SlotMap::iterator iter = slot->second.begin();
        iter != slot->second.end();
         ++iter)
    {
      if (iter->second.isMandatory() && iter->second.getData().get() == NULL)
      {
        this->mMandatoryConnectionsAreSet = false;
        return;
      }
    }
  }
}

void cedar::proc::Step::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  this->setName(node.get<std::string>("name"));
  this->setThreaded(node.get<bool>("threaded", false));

  for (ParameterMap::iterator iter = this->mParameters.begin(); iter != this->mParameters.end(); ++iter)
  {
    try
    {
      const cedar::aux::ConfigurationNode& value = node.get_child(iter->second->getName());
      iter->second->set(value);
    }
    catch (const boost::property_tree::ptree_bad_path&)
    {
      //!@todo handle with default value/exception
      std::cout << "Config node " << iter->second->getName() << " not found!" << std::endl;
    }
  }
}

void cedar::proc::Step::onStart()
{

}

void cedar::proc::Step::onTrigger()
{
  if (this->mMandatoryConnectionsAreSet)
  {
    if (!this->mBusy)
    {
      if (this->mRunInThread)
      {
        this->start();
      }
      else
      {
        this->run();
      }
    }
#ifdef DEBUG
    else
    {
      std::cout << "I'm busy" << std::endl;
    }
#endif // DEBUG
  } // this->mMandatoryConnectionsAreSet
  else
  {
    CEDAR_THROW(MissingConnectionException,
                "Some mandatory connections are not set for the processing step " + this->getName() + ".");
  }
}

void cedar::proc::Step::run()
{
  this->mBusy = true;

  // if no arguments have been set, create default ones.
  if (this->mNextArguments.get() == NULL)
  {
    this->mNextArguments = cedar::proc::ArgumentsPtr(new cedar::proc::Arguments());
  }

  this->compute(*(this->mNextArguments.get()));

  // remove the argumens, as they have been processed.
  this->mNextArguments.reset();

  this->mBusy = false;
  this->mFinished->trigger();
}

cedar::proc::TriggerPtr& cedar::proc::Step::getFinishedTrigger()
{
  return this->mFinished;
}

void cedar::proc::Step::setThreaded(bool isThreaded)
{
  this->mRunInThread = isThreaded;
}

void cedar::proc::Step::setNextArguments(cedar::proc::ArgumentsPtr arguments)
{
#ifdef DEBUG
  if (this->mNextArguments.get() != NULL)
  {
    std::cout << "Warning from ProcessingStep " << this->getName()
              << ": Overwriting arguments." << std::endl;
  }
#endif // DEBUG
  this->mNextArguments = arguments;
}

void cedar::proc::Step::declareData(DataRole::Id role, const std::string& name, bool mandatory)
{
  std::map<DataRole::Id, SlotMap>::iterator iter = this->mDataConnections.find(role);
  if (iter == this->mDataConnections.end())
  {
    this->mDataConnections[role] = SlotMap();
    iter = this->mDataConnections.find(role);

    CEDAR_DEBUG_ASSERT(iter != this->mDataConnections.end());
  }

  SlotMap::iterator map_iter = iter->second.find(name);
  if (map_iter != iter->second.end())
  {
    CEDAR_THROW(cedar::proc::DuplicateNameException, "There is already a " +
                 cedar::proc::DataRole::type().get(role).prettyString()
                 + " data-declaration with the name " + name + ".");
    return;
  }

  if (name.find('.') != std::string::npos)
  {
    CEDAR_THROW(cedar::proc::InvalidNameException, "Buffer names may not contain the character \".\". \""
                                                   + name + "\" in step \"" + this->getName()
                                                   + "\" violates this rule.");
  }

  iter->second[name] = DataEntry(mandatory);

  this->checkMandatoryConnections();
}

void cedar::proc::Step::declareInput(const std::string& name, bool mandatory)
{
  this->declareData(DataRole::INPUT, name, mandatory);
}

void cedar::proc::Step::declareBuffer(const std::string& name, bool mandatory)
{
  this->declareData(DataRole::BUFFER, name, mandatory);
}

void cedar::proc::Step::declareOutput(const std::string& name, bool mandatory)
{
  this->declareData(DataRole::OUTPUT, name, mandatory);
}


void cedar::proc::Step::setData(DataRole::Id role, const std::string& name, cedar::proc::DataPtr data)
{
  std::map<DataRole::Id, SlotMap>::iterator iter = this->mDataConnections.find(role);
  if (iter == this->mDataConnections.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException,
                "The requested role " +
                cedar::proc::DataRole::type().get(role).prettyString() +
                " does not exist.");
    return;
  }
  SlotMap::iterator map_iterator = iter->second.find(name);
  if (map_iterator == iter->second.end())
  {
    CEDAR_THROW(cedar::proc::InvalidNameException,
                "The requested " +
                cedar::proc::DataRole::type().get(role).prettyString() +
                " name does not exist.");
    return;
  }
  map_iterator->second.setData(data);
  this->checkMandatoryConnections();
}

void cedar::proc::Step::setInput(const std::string& name, cedar::proc::DataPtr data)
{
  this->setData(DataRole::INPUT, name, data);
}

void cedar::proc::Step::setBuffer(const std::string& name, cedar::proc::DataPtr data)
{
  this->setData(DataRole::BUFFER, name, data);
}

void cedar::proc::Step::setOutput(const std::string& name, cedar::proc::DataPtr data)
{
  this->setData(DataRole::OUTPUT, name, data);
}

cedar::proc::DataPtr cedar::proc::Step::getInput(const std::string& name)
{
  return this->getData(DataRole::INPUT, name);
}

cedar::proc::DataPtr cedar::proc::Step::getBuffer(const std::string& name)
{
  return this->getData(DataRole::BUFFER, name);
}

cedar::proc::DataPtr cedar::proc::Step::getOutput(const std::string& name)
{
  return this->getData(DataRole::OUTPUT, name);
}

cedar::proc::DataPtr cedar::proc::Step::getData(DataRole::Id role, const std::string& name)
{
  std::map<DataRole::Id, SlotMap>::iterator iter = this->mDataConnections.find(role);
  if (iter == this->mDataConnections.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException,
                "The requested role " +
                cedar::proc::DataRole::type().get(role).prettyString() +
                + " does not exist.");
    return cedar::proc::DataPtr();
  }
  SlotMap::iterator map_iterator = iter->second.find(name);
  if (map_iterator == iter->second.end())
  {
    CEDAR_THROW(cedar::proc::InvalidNameException, "The requested "
                                                   + cedar::proc::DataRole::type().get(role).prettyString()
                                                   + " name \"" + name + "\" does not exist in step \""
                                                   + this->getName() + "\".");
    return cedar::proc::DataPtr();
  }
  return map_iterator->second.getData();
}


void cedar::proc::Step::connect(
                                 cedar::proc::StepPtr source,
                                 const std::string& sourceName,
                                 cedar::proc::StepPtr target,
                                 const std::string& targetName
                               )
{
  target->setInput(targetName, source->getOutput(sourceName));
  /*
   * if the target is set to be auto-connected to triggers, do so. It makes sense to check this only for the target;
   * the source is not relevant, because the target may need to be triggered by the source, even if the source may not
   * want to be triggered. E.g., a dynamical system (not auto-connected) may trigger further processing during each
   * iteration.
   */
  if (target->mAutoConnectTriggers)
  {
    source->getFinishedTrigger()->addListener(target);
  }
}
