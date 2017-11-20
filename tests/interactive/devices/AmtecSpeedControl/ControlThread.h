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

    File:        ControlThread.h

    Maintainer:  Bjoern Weghenkel
    Email:       bjoern.weghenkel@ini.ruhr-uni-bochum.de
    Date:        2011 05 10

    Description: Example of speed control of an single joint.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_CONTROL_THREAD_H
#define CEDAR_CONTROL_THREAD_H


// CEDAR INCLUDES
#include "cedar/auxiliaries/LoopedThread.h"
#include "cedar/devices/KinematicChain.h"

// SYSTEM INCLUDES


const unsigned int JOINT = 4;
const double TARGET = 1.5;

/*!@brief Example of a thread controlling a joint by setting its velocity
 */
class ControlThread : public cedar::aux::LoopedThread
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------

  /*!@brief Constructor
   *
   *@param kinematicChain pointer to the chain
   *@param configFileName path to a configuration file
   */
  ControlThread
  (
    const cedar::dev::KinematicChainPtr kinematicChain,
    double stepSize = 1.0,
    double idleTime = 0.001
  );

//--------------------------------------------------------------------------------------------------------------------
// public methods
//--------------------------------------------------------------------------------------------------------------------
public:

//--------------------------------------------------------------------------------------------------------------------
// protected methods
//--------------------------------------------------------------------------------------------------------------------
protected:

//--------------------------------------------------------------------------------------------------------------------
// private methods
//--------------------------------------------------------------------------------------------------------------------
private:
  void step(cedar::unit::Time);

//--------------------------------------------------------------------------------------------------------------------
// members
//--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::dev::KinematicChainPtr mpKinematicChain;

}; // class ControlThread

#endif /* CEDAR_CONTROL_THREAD_H */
