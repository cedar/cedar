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

    File:        InfraredSensorSerial.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 12 03

    Description: Infrared sensor, which communicates over a serial channel.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_KTEAM_INFRARED_SENSOR_SERIAL_H
#define CEDAR_DEV_KTEAM_INFRARED_SENSOR_SERIAL_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/Sensor.h"
#include "cedar/devices/kteam/namespace.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>


/*!@brief Infrared sensor, which communicates over a serial channel.
 *
 * This class can provide either proximity or ambient light values, depending on the command string that
 * is sent to the robot.
 */
class cedar::dev::kteam::InfraredSensorSerial : public cedar::dev::Sensor
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  InfraredSensorSerial();

  //!@brief Destructor
  virtual ~InfraredSensorSerial();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Returns the current readings of the sensor.
  virtual cv::Mat getData();

  cv::Mat updateIrValues();

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
  static cedar::dev::Component::ComponentDataType M_IR_VALUES_ID;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::aux::StringParameterPtr _mCommandGetInfrared;

  //!@todo Should this be some kind of unit data?
  cedar::aux::MatDataPtr mValues;

}; // class cedar::dev::kteam::InfraredSensorSerial

#endif // CEDAR_DEV_KTEAM_INFRARED_SENSOR_SERIAL_H

