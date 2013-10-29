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

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 09 14

    Description: Source file for the gui settings in cedar's auxiliaries library.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/Settings.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <QMainWindow>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::Settings::Settings()
:
mWritingDisabled(false)
{
  this->_mPluginLoadDialogLocation = new cedar::aux::DirectoryParameter
                                     (
                                       this,
                                       "last plugin load dialog location",
                                       ""
                                     );

  this->load();
}


cedar::aux::gui::Settings::~Settings()
{
  this->save();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::DirectoryParameterPtr cedar::aux::gui::Settings::lastPluginLoadDialogLocation() const
{
  return this->_mPluginLoadDialogLocation;
}

void cedar::aux::gui::Settings::setLastPluginDialogLocation(const std::string& location)
{
  this->_mPluginLoadDialogLocation->setValue(location);
}

void cedar::aux::gui::Settings::save()
{
  if (!mWritingDisabled)
  {
    std::string path = cedar::aux::getUserApplicationDataDirectory() + "/.cedar/auxiliaries_gui";
    try
    {
      this->writeJson(path);
    }
    catch (const boost::property_tree::json_parser::json_parser_error& e)
    {
      cedar::aux::LogSingleton::getInstance()->warning
      (
        std::string("Error saving aux gui settings: ") + e.what(),
        "void cedar::aux::gui::Settings::load()"
      );
    }
  }
}

void cedar::aux::gui::Settings::load()
{
  std::string path = cedar::aux::getUserApplicationDataDirectory() + "/.cedar/auxiliaries_gui";
  try
  {
    this->readJson(path);
  }
  catch (const boost::property_tree::json_parser::json_parser_error& e)
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      std::string("Error reading aux gui settings: ") + e.what(),
      "void cedar::aux::gui::Settings::load()"
    );
  }
}
