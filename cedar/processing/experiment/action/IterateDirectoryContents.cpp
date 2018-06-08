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

    File:        IterateDirectoryContents.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 11 17

    Description: Source file for the class cedar::proc::experiment::action::IterateDirectoryContents.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/action/IterateDirectoryContents.h"
#include "cedar/processing/experiment/Supervisor.h"

// SYSTEM INCLUDES
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// register class
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool declared = cedar::proc::experiment::action::ActionManagerSingleton::getInstance()->
      registerType<cedar::proc::experiment::action::IterateDirectoryContentsPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::action::IterateDirectoryContents::IterateDirectoryContents()
:
_mFilter(new cedar::aux::StringParameter(this, "file filter regex", "")),
_mDirectory(new cedar::aux::DirectoryParameter(this, "directory", ""))
{
  this->getStepParameter()->setType(cedar::proc::experiment::StepPropertyParameter::PARAMETER);
  this->getStepParameter()->allowType("cedar.aux.FileParameter");
  QObject::connect(this->_mDirectory.get(), SIGNAL(valueChanged()), this, SLOT(updateDirectoryContents()));
  QObject::connect(this->_mFilter.get(), SIGNAL(valueChanged()), this, SLOT(updateDirectoryContents()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::experiment::action::IterateDirectoryContents::preExperiment()
{
  cedar::proc::experiment::action::SetParameterValue::preExperiment();

  size_t trial_count = static_cast<size_t>(cedar::proc::experiment::SupervisorSingleton::getInstance()->getExperiment()->getTrialCount());
  QReadLocker locker(this->mDirectoryContents.getLockPtr());
  size_t file_count = this->mDirectoryContents.member().size();
  locker.unlock();

  if (trial_count != file_count)
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Number of files in directory (" + cedar::aux::toString(file_count) + ") in IterateDirectoryContents action "
      "differs from the number of trials (" + cedar::aux::toString(trial_count) + "). Not all files/trials will be "
      "handled correctly.",
      CEDAR_CURRENT_FUNCTION_NAME
    );
  }

  mChangedParameterAsFileParameter
    = boost::dynamic_pointer_cast<cedar::aux::FileParameter>(this->getStepParameter()->getParameter());
}

void cedar::proc::experiment::action::IterateDirectoryContents::run()
{
  size_t trial = static_cast<size_t>(cedar::proc::experiment::SupervisorSingleton::getInstance()->getExperiment()->getCurrentTrial());
  QReadLocker locker(this->mDirectoryContents.getLockPtr());
  size_t index = trial;
  if (index >= this->mDirectoryContents.member().size())
  {
    index = this->mDirectoryContents.member().size() - 1;
  }
  auto path = this->mDirectoryContents.member().at(index);
  locker.unlock();

  if (this->mChangedParameterAsFileParameter)
  {
    QWriteLocker param_locker(this->mChangedParameterAsFileParameter->getLock());
    this->mChangedParameterAsFileParameter->setValue(path.absolute().toString());
    if(auto myExperiment = this->_mExperiment.lock())
    {
      //Hacky way to broadcast the current picture path to other actions!
      myExperiment->setExperimentInfo(path.absolute().toString());
    }
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Could not set parameter in IterateDirectoryContents action. Parameter type not handled.",
      CEDAR_CURRENT_FUNCTION_NAME
    );
  }
}

void cedar::proc::experiment::action::IterateDirectoryContents::updateDirectoryContents()
{
  this->lockParameters(cedar::aux::LOCK_TYPE_READ);
  cedar::aux::Path directory = this->_mDirectory->getValue().absolutePath().toStdString();
  std::string filter = this->_mFilter->getValue();
  this->unlockParameters(cedar::aux::LOCK_TYPE_READ);

  std::vector<cedar::aux::Path> files;

  if (filter.empty())
  {
    files = directory.listFiles();
  }
  else
  {
    files = directory.listFilesThatMatchRe(filter);
  }

  QWriteLocker locker(this->mDirectoryContents.getLockPtr());
  this->mDirectoryContents.member().clear();
  for (auto entry : files)
  {
    this->mDirectoryContents.member().push_back(entry.absolute());
  }
}
