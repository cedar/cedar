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

    File:        YarpVehicle.h

    Maintainer:  Jan Tekülve
    Email:       jan.tekuelve@ini.rub.de
    Date:        2016 06 27

    Description: Header file for the class cedar::dev::YarpVehicle.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_YARP_VEHICLE_H
#define CEDAR_DEV_YARP_VEHICLE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"
#include "cedar/devices/Vehicle.h"
#include "cedar/devices/YarpChannel.h"

// CEDAR INCLUDES

#ifdef CEDAR_USE_YARP
// FORWARD DECLARATIONS
#include "cedar/devices/YarpVehicle.fwd.h"

// SYSTEM INCLUDES


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::dev::YarpVehicle : public cedar::dev::Vehicle
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
  YarpVehicle();

  //!@brief Destructor
  virtual ~YarpVehicle();

  //--------------------------------------------------------------------------------------------------------------------
    // public methods
    //--------------------------------------------------------------------------------------------------------------------
  public:
    bool applyCrashbrake() override;
    void readConfiguration(const cedar::aux::ConfigurationNode& node) override;

    //--------------------------------------------------------------------------------------------------------------------
    // private methods
    //--------------------------------------------------------------------------------------------------------------------
  private:

    void registerPortNames();
    void sendVelocities(cv::Mat mat);
    cv::Mat retrieveVelocities();
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
 cedar::aux::StringParameterPtr mReadPort;
 cedar::aux::StringParameterPtr mWritePort;

}; // class cedar::dev::YarpVehicle

#endif //CEDAR_USE_YARP

#endif // CEDAR_DEV_YARP_VEHICLE_H

