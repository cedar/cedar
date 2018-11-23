/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        SimulatedVehicle.h

    Maintainer:  Jan Tekülve
    Email:       jan.tekuelve@ini.rub.de
    Date:        2016 07 21

    Description: Header file for the class cedar::dev::SimulatedVehicle.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SIMULATED_VEHICLE_H
#define CEDAR_DEV_SIMULATED_VEHICLE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/devices/SimulatedVehicle.fwd.h"

#include "cedar/devices/Vehicle.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>

// SYSTEM INCLUDES


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::dev::SimulatedVehicle : public cedar::dev::Vehicle
{
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  SimulatedVehicle();

  //!@brief Destructor
  virtual ~SimulatedVehicle();

  //--------------------------------------------------------------------------------------------------------------------
    // public methods
    //--------------------------------------------------------------------------------------------------------------------
  public:
    bool applyCrashbrake() override;

  public slots:
//    @brief reacts to a change in the number of joints and sets an almost-zero initial configuration
//    void updateInitialConfiguration();
    //--------------------------------------------------------------------------------------------------------------------
    // private methods
    //--------------------------------------------------------------------------------------------------------------------
  private:

    void sendSimulatedVelocities(cv::Mat mat);
    cv::Mat retrieveSimulatedVelocities();

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

}; // class cedar::dev::SimulatedVehicle

#endif // CEDAR_DEV_SIMULATED_VEHICLE_H

