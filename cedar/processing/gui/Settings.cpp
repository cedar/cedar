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

    File:        FrameworkSettings.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 26

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "processing/gui/Settings.h"
#include "auxiliaries/Configurable.h"
#include "auxiliaries/SetParameter.h"
#include "auxiliaries/DirectoryParameter.h"
#include "auxiliaries/Parameter.h"
#include "auxiliaries/System.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/property_tree/json_parser.hpp>
#include <QMainWindow>

cedar::proc::gui::Settings cedar::proc::gui::Settings::mInstance;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::Settings::Settings()
:
cedar::aux::Configurable(),
mLog(new cedar::proc::gui::Settings::DockSettings()),
mSteps(new cedar::proc::gui::Settings::DockSettings()),
mTools(new cedar::proc::gui::Settings::DockSettings()),
mProperties(new cedar::proc::gui::Settings::DockSettings()),
mMainWindowGeometry(new cedar::aux::StringParameter(this, "mainWindowGeometry", "")),
mMainWindowState(new cedar::aux::StringParameter(this, "mainWindowState", ""))
{

  cedar::aux::ConfigurablePtr plugins(new cedar::aux::Configurable());
  this->addConfigurableChild("plugins", plugins);

  std::set<std::string> default_plugins;
  mPluginsToLoad = cedar::aux::StringSetParameterPtr
                   (
                     new cedar::aux::StringSetParameter
                     (
                       plugins.get(),
                       "loadOnStartup",
                       default_plugins
                     )
                   );

  cedar::aux::ConfigurablePtr ui_settings(new cedar::aux::Configurable());
  this->addConfigurableChild("ui", ui_settings);
  
  ui_settings->addConfigurableChild("log", mLog);
  ui_settings->addConfigurableChild("steps", mSteps);
  ui_settings->addConfigurableChild("tools", mTools);
  ui_settings->addConfigurableChild("properties", mProperties);

  cedar::aux::ConfigurablePtr recent_files(new cedar::aux::Configurable());
  this->addConfigurableChild("fileHistory", recent_files);
  this->mPluginLoadDialogLocation = cedar::aux::DirectoryParameterPtr(new cedar::aux::DirectoryParameter(recent_files.get(), "lastPluginLoadDialogLocation", ""));

  this->load();
}

cedar::proc::gui::Settings::DockSettings::DockSettings()
:
mVisible(new cedar::aux::BoolParameter(this, "visible", true)),
mFloating(new cedar::aux::BoolParameter(this, "floating", false))
{
}

cedar::proc::gui::Settings::~Settings()
{
  this->save();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::DirectoryParameterPtr cedar::proc::gui::Settings::lastPluginLoadDialogLocation()
{
  return this->mPluginLoadDialogLocation;
}

void cedar::proc::gui::Settings::storeMainWindow(QMainWindow *pWindow)
{
  QByteArray window_state = pWindow->saveState();
  QByteArray window_state_hex = window_state.toHex();
  mMainWindowState->set(window_state_hex.constData());
  
  QByteArray window_geometry = pWindow->saveGeometry();
  QByteArray window_geometry_hex = window_geometry.toHex();
  mMainWindowGeometry->set(window_geometry_hex.constData());
}

void cedar::proc::gui::Settings::restoreMainWindow(QMainWindow *pWindow)
{
  QByteArray window_state_hex(mMainWindowState->get().c_str());
  QByteArray window_state = QByteArray::fromHex(window_state_hex);
  if (!pWindow->restoreState(window_state))
  {
    std::cout << "Could not restore state of the main window." << std::endl;
  }
  
  QByteArray window_geometry_hex(mMainWindowGeometry->get().c_str());
  QByteArray window_geometry = QByteArray::fromHex(window_geometry_hex);
  if (!pWindow->restoreGeometry(window_geometry))
  {
    std::cout << "Could not restore geometry of the main window." << std::endl;
  }
}

void cedar::proc::gui::Settings::DockSettings::getFrom(QDockWidget *pDock)
{
  this->mVisible->set(pDock->isVisible());
  this->mFloating->set(pDock->isFloating());
}

void cedar::proc::gui::Settings::DockSettings::setTo(QDockWidget *pDock)
{
  pDock->setVisible(this->mVisible->get());
  pDock->setFloating(this->mFloating->get());
}

cedar::proc::gui::Settings::DockSettingsPtr cedar::proc::gui::Settings::logSettings()
{
  return this->mLog;
}

cedar::proc::gui::Settings::DockSettingsPtr cedar::proc::gui::Settings::toolsSettings()
{
  return this->mTools;
}

cedar::proc::gui::Settings::DockSettingsPtr cedar::proc::gui::Settings::propertiesSettings()
{
  return this->mProperties;
}

cedar::proc::gui::Settings::DockSettingsPtr cedar::proc::gui::Settings::stepsSettings()
{
  return this->mSteps;
}


cedar::proc::gui::Settings& cedar::proc::gui::Settings::instance()
{
  return cedar::proc::gui::Settings::mInstance;
}

void cedar::proc::gui::Settings::load()
{
  std::string path = cedar::aux::System::getUserApplicationDataDirectory() + "/.cedar/processingGui";
  try
  {
    this->readJson(path);
  }
  catch (const boost::property_tree::json_parser::json_parser_error& e)
  {
    //!@todo proper signaling(?) of this message to the gui.
    std::cout << "Error reading framework gui settings: " << e.what() << std::endl;
  }
}

void cedar::proc::gui::Settings::save()
{
  std::string path = cedar::aux::System::getUserApplicationDataDirectory() + "/.cedar/processingGui";
  try
  {
    this->saveJson(path);
  }
  catch (const boost::property_tree::json_parser::json_parser_error& e)
  {
    //!@todo proper signaling(?) of this message to the gui.
    std::cout << "Error saving framework gui settings: " << e.what() << std::endl;
  }
}

const std::set<std::string>& cedar::proc::gui::Settings::pluginsToLoad()
{
  return this->mPluginsToLoad->get();
}

void cedar::proc::gui::Settings::addPluginToLoad(const std::string& path)
{
  this->mPluginsToLoad->insert(path);
}

void cedar::proc::gui::Settings::removePluginToLoad(const std::string& path)
{
  std::set<std::string>::iterator pos = this->mPluginsToLoad->get().find(path);
  if (pos != this->mPluginsToLoad->get().end())
  {
    this->mPluginsToLoad->get().erase(pos);
  }
}

