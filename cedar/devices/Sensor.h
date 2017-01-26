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

    File:        Sensor.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 26

    Description: Abstract sensor of a robot (e.g., an infrared sensor).

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_SENSOR_H
#define CEDAR_DEV_SENSOR_H

// CEDAR INCLUDES
#include "cedar/devices/Component.h"

// FORWARD DECLARATIONS
#include "cedar/devices/Sensor.fwd.h"

// SYSTEM INCLUDES

/*!@brief Base class for sensors of robots.
 *
 * @todo More detailed description of the class.
 */
class cedar::dev::Sensor : public cedar::dev::Component
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor
  Sensor();

  //!@brief Constructor taking an externally created channel
  Sensor(cedar::dev::ChannelPtr channel);

  //!@brief Destructor
  virtual ~Sensor();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Returns the current readings of the sensor.
  virtual cv::Mat getData() = 0;

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

  bool applyBrakeSlowlyController();
  bool applyBrakeNowController();
  bool applyCrashbrake();

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
  // none yet
}; // class cedar::dev::Sensor
#endif // CEDAR_DEV_SENSOR_H
