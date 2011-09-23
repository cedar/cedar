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

    File:        KinematicChainWidget.cpp

    Maintainer:  Bjoern Weghenkel
    Email:       bjoern.weghenkel@ini.rub.de
    Date:        2011 01 06

    Description: Example for an @em cedar::dev::robot::KinematicChainWidget.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES

#include "ControlThread.h"

// PROJECT INCLUDES

#include "devices/robot/AmtecKinematicChain.h"

// SYSTEM INCLUDES


using namespace std;
using namespace cedar::dev::robot;


//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

int main(int /* argc */, char ** /* argv[] */) {

  string config_file("../../tests/interactive/devices/AmtecSpeedControl/cora_arm.conf");

  try
  {
    KinematicChainPtr p_kinematic_chain(new AmtecKinematicChain(config_file));
    ControlThread thread(p_kinematic_chain, config_file);
    cout << "moving arm for 15s just by controling velocity..." << endl;
    thread.start();
    thread.wait(15000);
    thread.stop();
  }
  catch(exception e)
  {
    cout << "Exception: " << e.what() << endl;
  }
}
