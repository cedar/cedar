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

    File:        ControlThread.cpp

    Maintainer:  Bjoern Weghenkel
    Email:       bjoern.weghenkel@ini.ruhr-uni-bochum.de
    Date:        2011 05 10

    Description: Example of speed control of an single joint.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES

#include "ControlThread.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

ControlThread::ControlThread(
                              const cedar::dev::KinematicChainPtr kinematicChain,
                              double stepSize,
                              double idleTime
                            )
:
cedar::aux::LoopedThread(stepSize, idleTime)
{
  mpKinematicChain = kinematicChain;
  return;
}


void ControlThread::step(double)
{
  double current_pos = mpKinematicChain->getJointAngle(JOINT);
  double current_vel = mpKinematicChain->getJointVelocity(JOINT);

  double rate_of_change = 1.0 * (TARGET - current_pos);
  rate_of_change = std::min<double>(rate_of_change, current_vel + 0.4);

  //todo: use a log instead of the std::cout
  std::cout << "setting speed " << rate_of_change << std::endl;
  mpKinematicChain->setJointVelocity(JOINT, rate_of_change);
}

