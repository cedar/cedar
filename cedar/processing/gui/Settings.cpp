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
#include "auxiliaries/System.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/property_tree/json_parser.hpp>

cedar::proc::gui::Settings cedar::proc::gui::Settings::mInstance;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::Settings::Settings()
:
cedar::aux::Configurable()
{
  cedar::aux::ConfigurablePtr plugins(new cedar::aux::Configurable());
  this->addConfigurableChild("plugins", plugins);

  std::set<std::string> default_plugins;
  mPluginsToLoad = cedar::aux::StringSetParameterPtr
                   (
                     new cedar::aux::StringSetParameter
                     (
                       "loadOnStartup",
                       default_plugins
                     )
                   );
  plugins->registerParameter(mPluginsToLoad);

  this->load();
}

cedar::proc::gui::Settings::~Settings()
{
  this->save();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


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
