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

    File:        YarpDrive.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2013 03 06

    Description: Yarp implementation of an epuck drive.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_KTEAM_EPUCK_YARP_DRIVE_H
#define CEDAR_DEV_KTEAM_EPUCK_YARP_DRIVE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_YARP

// CEDAR INCLUDES
#include "cedar/devices/kteam/namespace.h"
#include "cedar/devices/kteam/Drive.h"
#include "cedar/devices/kteam/epuck/namespace.h"
#include "cedar/devices/YarpChannel.h"
#include "cedar/auxiliaries/StringParameter.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>


/*!@brief Yarp implementation of an epuck drive.
 *
 * @todo describe more.
 */
class cedar::dev::kteam::epuck::YarpDrive : public cedar::dev::kteam::Drive
{
public:
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
  YarpDrive();

  //!@brief Constructor taking an externally created channel.
  YarpDrive(YarpMatChannelPtr channel);

  //!@brief Destructor
  virtual ~YarpDrive();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Returns the current encoder value of the left and right wheel.
  std::vector<int> getEncoders() const;

  /*!@brief Sets the encoder values of both wheels.
   * @param[in] encoders encoder value for the left and right wheel
   */
  void setEncoders(const std::vector<int>& encoders);

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
  //! @see cedar::dev::Locomotion.
  void sendMovementCommand();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void registerChannels();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  YarpMatChannelPtr mYarpChannel;
  std::string mMotorCommandPortWithPrefix;
  std::string mEncoderValuesPortWithPrefix;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::aux::StringParameterPtr _mMotorCommandsPort;
  cedar::aux::StringParameterPtr _mEncoderValuesPort;

}; // class cedar::dev::kteam::epuck::YarpDrive

#endif // CEDAR_USE_YARP

#endif // CEDAR_DEV_KTEAM_EPUCK_YARP_DRIVE_H

