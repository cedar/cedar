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

    File:        KinematicChainWidget.cpp

    Maintainer:  Bjoern Weghenkel
    Email:       bjoern.weghenkel@ini.rub.de
    Date:        2011 01 06

    Description: Example for an @em cedar::dev::KinematicChainWidget.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "ControlThread.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/auxiliaries/systemFunctions.h"

// SYSTEM INCLUDES
#include "cedar/devices/amtec/KinematicChain.h"

//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

int main(int /* argc */, char ** /* argv[] */)
{
  std::string configuration_file = cedar::aux::locateResource("robots/cora_arm.json");

  try
  {
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
//    ControlThread thread(p_kinematic_chain, configuration_file);
    ControlThread thread(p_kinematic_chain, 100, 0.01);
    std::cout << "moving arm for 5s just by controling velocity..." << std::endl;
    thread.start();
    cedar::aux::sleep(cedar::unit::Time(5.0 * cedar::unit::seconds));
    thread.stop();
    thread.wait();
  }
  catch (std::exception e)
  {
    std::cout << "Exception: " << e.what() << std::endl;
  }
}
