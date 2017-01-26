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

    File:        MainApplication.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 01 20

    Description: Source file for the class cedar::processingCL::MainApplication.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Group.h"
#include "cedar/auxiliaries/Settings.h"

// LOCAL INCLUDES
#include "MainApplication.h"

// SYSTEM INCLUDES
#include <boost/make_shared.hpp>
#include <QTime>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::processingCL::MainApplication::MainApplication(int argc, char** argv)
{
  mParser.defineFlag("run", "Run the architecture after loading it.", 'r');
  mParser.defineFlag("no-plugins", "Do not load default plugins.", 'p');
  mParser.defineValue("load", "Load an architecture.", 'l');
  mParser.parse(argc, argv, true);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::processingCL::MainApplication::exec()
{
  // load default plugins
  if (!this->mParser.hasParsedFlag("no-plugins"))
  {
    std::cout << "Loading PLUGINS" << std::endl;
    cedar::aux::SettingsSingleton::getInstance()->loadDefaultPlugins();
  }

  // command line input processing
  if (this->mParser.hasParsedValue("load"))
  {
    this->loadArchitecture(this->mParser.getValue<std::string>("load"));
  }

  if (this->mParser.hasParsedFlag("run"))
  {
    this->startTriggers();
  }

  // user input processing
  while (this->waitForUserInput())
  {
    // infinite loop until the function returns false.
  }
  emit quit();
}

bool cedar::processingCL::MainApplication::waitForUserInput()
{
  std::cout << ">";
  std::string command;
  std::cin >> command;

  //!@todo In the long run, we probably need better parsing of these commands.
  if (command == "quit()")
  {
    return false;
  }
  else if (command == "help")
  {
    std::cout << "Available commands:" << std::endl;
    std::cout << "-------------------" << std::endl << std::endl;
    std::cout << "startTriggers()    Starts all triggers of the architecture." << std::endl;
    std::cout << "quit()             Exit application." << std::endl;
  }
  else if (command == "startTriggers()")
  {
    this->startTriggers();
  }
  else
  {
    std::cout << "Unrecognized command \"" << command << "\". Type \"help\" for a list of available commands." << std::endl;
  }

  return true;
}

void cedar::processingCL::MainApplication::startTriggers()
{
  if (!this->mArchitecture)
  {
    std::cout << "Cannot start triggers: no architecture loaded." << std::endl;
    return;
  }
  this->mArchitecture->startTriggers();
  std::cout << "Triggers started." << std::endl;
}

void cedar::processingCL::MainApplication::loadArchitecture(const std::string& path)
{
  std::cout << "Loading architecture \"" << path << "\"" << std::endl;
  std::cout << "This may take a while, please be patient." << std::endl;
  std::cout << std::endl;

  this->mArchitecture = boost::make_shared<cedar::proc::Group>();
  QTime timer;
  timer.start();
  this->mArchitecture->readJson(path);
  std::cout << "Loading done, it took " << timer.elapsed() << " ms." << std::endl;
}
