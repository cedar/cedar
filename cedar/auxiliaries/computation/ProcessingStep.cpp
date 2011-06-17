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

    File:        ProcessingStep.cpp


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
#include "auxiliaries/computation/ProcessingStep.h"
#include "auxiliaries/computation/Arguments.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::comp::ProcessingStep::ProcessingStep(bool runInThread)
:
mFinished(new cedar::aux::comp::Trigger()),
mBusy(false),
mRunInThread(runInThread)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::comp::ProcessingStep::onTrigger()
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
  else
  {
    std::cout << "I'm busy" << std::endl;
  }
}

void cedar::aux::comp::ProcessingStep::run()
{
  this->mBusy = true;

  // if no arguments have been set, create default ones.
  if (this->mNextArguments.get() == NULL)
  {
    this->mNextArguments = cedar::aux::comp::ArgumentsPtr(new cedar::aux::comp::Arguments());
  }

  this->compute(*(this->mNextArguments.get()));

  // remove the argumens, as they have been processed.
  this->mNextArguments.reset();

  this->mBusy = false;
  this->mFinished->trigger();
}

cedar::aux::comp::TriggerPtr& cedar::aux::comp::ProcessingStep::getFinishedTrigger()
{
  return this->mFinished;
}

void cedar::aux::comp::ProcessingStep::setThreaded(bool isThreaded)
{
  this->mRunInThread = isThreaded;
}

void cedar::aux::comp::ProcessingStep::setNextArguments(cedar::aux::comp::ArgumentsPtr arguments)
{
#ifdef DEBUG
  if (this->mArguments.get() != NULL)
  {
    std::cout << "Warning from ProcessingStep " << this->getName()
              << ": Overwriting arguments." << std::endl;
  }
#endif // DEBUG
  this->mNextArguments = arguments;
}

void cedar::aux::comp::ProcessingStep::addInput(cedar::aux::comp::DataPtr input)
{
  this->addData(DATA_ROLE_INPUT, input);
}

void cedar::aux::comp::ProcessingStep::addBuffer(cedar::aux::comp::DataPtr buffer)
{
  this->addData(DATA_ROLE_BUFFER, buffer);
}

void cedar::aux::comp::ProcessingStep::addOutput(cedar::aux::comp::DataPtr output)
{
  this->addData(DATA_ROLE_OUTPUT, output);
}

void cedar::aux::comp::ProcessingStep::addData(DataRole role, cedar::aux::comp::DataPtr data)
{
	//! @todo Check that the name doesn't exist yet
  std::map<DataRole, std::vector<cedar::aux::comp::DataPtr> >::iterator vector_iter = this->mDataConnections.find(role);
  if (vector_iter == this->mDataConnections.end())
  {
    this->mDataConnections[role] = std::vector<cedar::aux::comp::DataPtr>();
    vector_iter = this->mDataConnections.find(role);
  }
  vector_iter->second.push_back(data);
}

cedar::aux::comp::DataPtr cedar::aux::comp::ProcessingStep::getInputByName(const std::string& name) const
{
  return this->getDataByName(DATA_ROLE_INPUT, name);
}

cedar::aux::comp::DataPtr cedar::aux::comp::ProcessingStep::getBufferByName(const std::string& name) const
{
  return this->getDataByName(DATA_ROLE_BUFFER, name);
}

cedar::aux::comp::DataPtr cedar::aux::comp::ProcessingStep::getOutputByName(const std::string& name) const
{
  return this->getDataByName(DATA_ROLE_OUTPUT, name);
}

cedar::aux::comp::DataPtr cedar::aux::comp::ProcessingStep::getDataByName(DataRole role, const std::string& name) const
{
  std::map<DataRole, std::vector<cedar::aux::comp::DataPtr> >::const_iterator vector_iter = this->mDataConnections.find(role);
  if (vector_iter != this->mDataConnections.end())
  {
    const std::vector<cedar::aux::comp::DataPtr>& vector = vector_iter->second;
    for (size_t i = 0; i < vector.size(); ++i)
    {
      if (vector.at(i)->getName() == name)
      {
        return vector.at(i);
      }
    }
  }

  //!@todo throw not found exception

  return cedar::aux::comp::DataPtr();
}



void cedar::aux::comp::ProcessingStep::connect(
                                                cedar::aux::comp::ProcessingStepPtr source,
                                                const std::string& sourceName,
                                                cedar::aux::comp::ProcessingStepPtr target,
                                                const std::string& targetName
                                              )
{
  try
  {
    target->addInput(source->getOutputByName(sourceName)); //!\todo output not found exception
    source->getFinishedTrigger()->addListener(target); //!\todo conflict with smart ass pointers!
  }
  catch(...)
  {
    return;
  }
}
