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

 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        SimulatedKinematicChain.h
 
 ----- Maintainer:  Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 06
 
 ----- Description: Header for the \em cedar::dev::robot::SimulatedKinematicChain class.
 
 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_DEV_SIMULATED_KINEMATIC_CHAIN_H
#define CEDAR_DEV_SIMULATED_KINEMATIC_CHAIN_H

// CEDAR INCLUDES
#include "cedar/devices/namespace.h"
#include "cedar/devices/SimulatedKinematicChain.fwd.h"
#include "cedar/devices/KinematicChain.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>

/*!@brief simulation of a kinematic chain
 *
 * This class provides a simulation of a kinematic chain determined by the specified reference geometry.
 * The current configuration state of the kinematic chain is represented by a vector of joint angles
 */
class cedar::dev::SimulatedKinematicChain : public cedar::dev::KinematicChain
{
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  SimulatedKinematicChain();
  //!@brief destructor
  ~SimulatedKinematicChain();
  
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  bool applyCrashbrake() override;
  void applyInitialConfigurationController(cv::Mat targetConfig) override;

public slots:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  void sendSimulatedAngles(cv::Mat mat);
  cv::Mat retrieveSimulatedAngles();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------

  // none yet
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
  std::map< ComponentDataType, cv::Mat > mSimulation;
  mutable QReadWriteLock mSimulationLock;

}; // class cedar::dev::SimulatedKinematicChain
#endif // CEDAR_DEV_SIMULATED_KINEMATIC_CHAIN_H
