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
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::Settings::Settings()
:
mWritingDisabled(false),
_mMaximumNumberOfLogEntries(new cedar::aux::UIntParameter(this, "maximal number of log entries", 200))
{
  std::string default_plot = "cedar::aux::gui::";

#ifdef CEDAR_USE_QWTPLOT3D
  default_plot += "QwtSurfacePlot";
#elif defined CEDAR_USE_VTK
  default_plot += "VtkSurfacePlot";
#else
  default_plot += "ImagePlot";
#endif // CEDAR_USE_QWTPLOT3D

  this->_mDefaultMatDataPlot = new cedar::aux::StringParameter(this, "default 2d mat data plot", default_plot);
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

std::string cedar::aux::gui::Settings::getDefault2dMatDataPlot() const
{
  cedar::aux::Parameter::ReadLocker locker(this->_mDefaultMatDataPlot.get());
  std::string plot_class = this->_mDefaultMatDataPlot->getValue();
  return plot_class;
}

void cedar::aux::gui::Settings::setDefault2dMatDataPlot(const std::string& plotClass)
{
  cedar::aux::Parameter::WriteLocker locker(this->_mDefaultMatDataPlot.get());
  this->_mDefaultMatDataPlot->setValue(plotClass);
}

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
  }
}

unsigned int cedar::aux::gui::Settings::getMaximumNumberOfLogEntries() const
{
  return this->_mMaximumNumberOfLogEntries->getValue();
}

cedar::aux::UIntParameterPtr cedar::aux::gui::Settings::getMaximalNumberOfLogEntriesParameter() const
{
  return this->_mMaximumNumberOfLogEntries;
}


