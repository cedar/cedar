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

    File:        Settings.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2013 06 21

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/Settings.h"
#include "cedar/auxiliaries/SetParameter.h"
#include "cedar/auxiliaries/DirectoryParameter.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <boost/property_tree/json_parser.hpp>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::Settings::Settings()
:
cedar::aux::Configurable(),
_mMemoryDebugOutput(new cedar::aux::BoolParameter(this, "memory debug output", false))
{
  mRecorderWorkspace = new cedar::aux::DirectoryParameter
                       (
                         this,
                         "recorder output directory",
                         cedar::aux::getUserHomeDirectory() + "/cedarRecordings/"
                       );

  try
  {
    this->load();
  }
  catch (cedar::aux::ParseException& exc)
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "Error loading settings, a new file will be generated.",
      "cedar::aux::Settings::Settings()"
    );
  }
}

cedar::aux::Settings::~Settings()
{
  try
  {
    this->save();
  }
  catch(cedar::aux::ParseException& exc)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Error saving settings, please check file permissions in"
        + cedar::aux::getUserApplicationDataDirectory() + "/.cedar",
      "cedar::aux::Settings::~Settings()"
    );
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::DirectoryParameterPtr cedar::aux::Settings::getRecorderWorkspaceParameter() const
{
  return this->mRecorderWorkspace;
}

std::string cedar::aux::Settings::getRecorderWorkspace() const
{
  return this->mRecorderWorkspace->getValue().absolutePath().toStdString();
}

void cedar::aux::Settings::load()
{
  std::string path = cedar::aux::getUserApplicationDataDirectory() + "/.cedar/auxilariesSettings";
  try
  {
    this->readJson(path);
  }
  catch (const boost::property_tree::json_parser::json_parser_error& e)
  {
    CEDAR_THROW(cedar::aux::ParseException, "Error reading settings: " + std::string(e.what()));
  }
}

void cedar::aux::Settings::save()
{
  std::string path = cedar::aux::getUserApplicationDataDirectory() + "/.cedar/auxilariesSettings";
  try
  {
    this->writeJson(path);
  }
  catch (const boost::property_tree::json_parser::json_parser_error& e)
  {
    CEDAR_THROW(cedar::aux::ParseException, "Error saving settings: " + std::string(e.what()));
  }
}

bool cedar::aux::Settings::getMemoryDebugOutput()
{
  return this->_mMemoryDebugOutput->getValue();
}
