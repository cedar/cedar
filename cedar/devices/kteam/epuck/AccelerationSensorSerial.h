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

    File:        AccelerationSensorSerial.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 26

    Description: Acceleration sensor of the e-puck, which communicates over a serial channel.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_KTEAM_EPUCK_ACCELERATION_SENSOR_SERIAL_H
#define CEDAR_DEV_KTEAM_EPUCK_ACCELERATION_SENSOR_SERIAL_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/Sensor.h"
#include "cedar/devices/kteam/namespace.h"
#include "cedar/devices/kteam/epuck/namespace.h"

// SYSTEM INCLUDES


/*!@brief Acceleration sensor of the e-puck, which communicates over a serial channel..
 *
 * @todo describe more.
 */
class cedar::dev::kteam::epuck::AccelerationSensorSerial : public cedar::dev::Sensor
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  AccelerationSensorSerial();

  //!@brief Constructor taking an externally created channel.
  AccelerationSensorSerial(cedar::dev::kteam::SerialChannelPtr channel);

  //!@brief Destructor
  virtual ~AccelerationSensorSerial();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Returns the current readings of the sensor.
  virtual cv::Mat getData();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::aux::StringParameterPtr _mCommandGetAcceleration;

}; // class cedar::dev::kteam::epuck::AccelerationSensorSerial

#endif // CEDAR_DEV_KTEAM_EPUCK_ACCELERATION_SENSOR_SERIAL_H

