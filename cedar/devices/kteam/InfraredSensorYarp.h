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

    File:        InfraredSensorYarp.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2013 03 20

    Description: Infrared sensor, which communicates over a yarp channel.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_KTEAM_INFRARED_SENSOR_YARP_H
#define CEDAR_DEV_KTEAM_INFRARED_SENSOR_YARP_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_YARP

// CEDAR INCLUDES
#include "cedar/devices/Sensor.h"
#include "cedar/devices/kteam/namespace.h"

// FORWARD DECLARATIONS
#include "cedar/devices/YarpChannel.fwd.h"
#include "cedar/auxiliaries/MatData.fwd.h"

// SYSTEM INCLUDES
#include <string>


/*!@brief Infrared sensor, which communicates over a yarp channel.
 *
 */
class cedar::dev::kteam::InfraredSensorYarp : public cedar::dev::Sensor
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  typedef cedar::dev::YarpChannel<cv::Mat> YarpMatChannel;
  CEDAR_GENERATE_POINTER_TYPES(YarpMatChannel);
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  InfraredSensorYarp();

  //!@brief Constructor taking an externally created channel.
  InfraredSensorYarp(YarpMatChannelPtr channel);

  //!@brief Destructor
  virtual ~InfraredSensorYarp();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Returns the current readings of the sensor.
  virtual cv::Mat getData();

  /*!@brief   Overrides the default configuration reading.
   *
   * @remarks This method provides downwards-compatibility for reading fields that were written with a previous version.
   *          Currently, it takes care of reading the kernels and the sigmoid properly.
   */
  void readConfiguration(const cedar::aux::ConfigurationNode& node);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void open();

  void updateSensorValues();
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  std::string mSensorPortWithPrefix;

  //!@todo Should this be some kind of unit data?
  cedar::aux::MatDataPtr mValues;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::aux::StringParameterPtr _mSensorPort;

}; // class cedar::dev::kteam::InfraredSensorYarp
#endif // CEDAR_USE_YARP

#endif // CEDAR_DEV_KTEAM_INFRARED_SENSOR_YARP_H
