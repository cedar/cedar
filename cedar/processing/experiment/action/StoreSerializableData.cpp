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

    File:        StoreSerializableData.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 11 17

    Description: Source file for the class cedar::proc::experiment::action::StoreSerializableData.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/action/StoreSerializableData.h"
#include "cedar/processing/experiment/Supervisor.h"
#include "cedar/auxiliaries/LockerBase.h"

// SYSTEM INCLUDES
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// register class
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool declared = cedar::proc::experiment::action::ActionManagerSingleton::getInstance()->
      registerType<cedar::proc::experiment::action::StoreSerializableDataPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::action::StoreSerializableData::StoreSerializableData()
:
_mOutputFile(new cedar::aux::FileParameter(this, "filename", cedar::aux::FileParameter::WRITE, "")),
_mAppendTime(new cedar::aux::BoolParameter(this, "append time to filename", true))
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::experiment::action::StoreSerializableData::run()
{
  //!@todo There should be a Configurable::Read/WriteLocker instead of this
  cedar::aux::LockerBase locker
  (
    boost::bind(&cedar::aux::Configurable::lockParameters, this, cedar::aux::LOCK_TYPE_READ),
    boost::bind(&cedar::aux::Configurable::unlockParameters, this, cedar::aux::LOCK_TYPE_READ)
  );

  cedar::aux::Path path = this->_mOutputFile->getPath(true);

  if (this->_mAppendTime->getValue())
  {
    std::string filename_no_ext = path.getFileNameWithoutExtension();
    std::string extension;
    if (path.hasExtension())
    {
      extension = "." + path.getExtension();
    }
    std::string date = cedar::aux::Path::getTimestampForFileName();
    std::string new_name = filename_no_ext + "." + date + extension;
    path.setFileName(new_name);
  }

  auto group = cedar::proc::experiment::SupervisorSingleton::getInstance()->getExperiment()->getGroup();
  group->writeDataFile(path);
  cedar::aux::LogSingleton::getInstance()->message
  (
    "Serializable data stored in \"" + path.absolute().toString() + "\".",
    CEDAR_CURRENT_FUNCTION_NAME
  );
}
