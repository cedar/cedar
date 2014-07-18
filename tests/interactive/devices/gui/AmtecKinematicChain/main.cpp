/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        KinematicChainWidget.cpp

    Maintainer:  Bjoern Weghenkel
    Email:       bjoern.weghenkel@ini.rub.de
    Date:        2011 01 06

    Description: Example for an @em cedar::dev::KinematicChainWidget.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/gui/KinematicChainWidget.h"
#include "cedar/devices/amtec/KinematicChain.h"
#include "cedar/auxiliaries/systemFunctions.h"

// SYSTEM INCLUDES
#include <iostream>
#include <QApplication>

//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

int main(int argc, char *argv[]) {
  try
  {
//    std::string configuration_file_old = cedar::aux::System::locateResource("configs/cora_arm.conf");
    std::string configuration_file = cedar::aux::locateResource("configs/cora_arm.json");
    cedar::dev::amtec::KinematicChainPtr p_kinematic_chain
    (
      new cedar::dev::amtec::KinematicChain()
    );
    p_kinematic_chain->readJson(configuration_file);

    if(!p_kinematic_chain->initDevice())
    {
      std::cout << "Error initializing the Amtec module!" << std::endl;
      CEDAR_THROW(cedar::aux::InitializationException, "Error initializing the Amtec module!");
    }


    //p_kinematic_chain->useCurrentHardwareValues(true);
    QApplication app(argc, argv);
    cedar::dev::gui::KinematicChainWidget widget(p_kinematic_chain);
    widget.show();
    return app.exec();
  }
  catch(std::exception e)
  {
    std::cout << "Exception: " << e.what() << std::endl;
  }
}
