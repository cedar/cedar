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

    File:        Drive.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 04 12

    Description: The differential drive of a pulse-width-modulation (PWM) driven robot.

    Credits:     Original design by Andre Bartel (2011)

======================================================================================================================*/

#ifndef CEDAR_DEV_KTEAM_DRIVE_H
#define CEDAR_DEV_KTEAM_DRIVE_H

// CEDAR INCLUDES
#include "cedar/devices/namespace.h"
#include "cedar/devices/kteam/namespace.h"
#include "cedar/devices/DifferentialDrive.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/units/Length.h"
#include "cedar/units/Frequency.h"

// FORWARD DELCARATIONS
#include "cedar/auxiliaries/math/IntLimitsParameter.fwd.h"

// SYSTEM INCLUDES
#include <vector>

/*!@brief The differential drive of a pulse-width-modulation (PWM) driven robot.
 *
 * This is an abstract class with functions and attributes common to differential drive robots with
 * pulse-width-modulation driven wheels. These are, for instance, the mobile robots E-Puck, Khepera and Koala.
 */
class cedar::dev::kteam::Drive : public cedar::dev::DifferentialDrive
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor
  Drive();

  //!@brief Constructor taking an externally created channel
  Drive(cedar::dev::ChannelPtr channel);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Returns the number of pulses per wheel-revolution.
  float getNumberOfPulsesPerRevolution() const;

  //!@brief Returns the distance a wheel moves for a single pulse [in m].
  cedar::unit::Length getDistancePerPulse() const;

  //!@brief Returns the limits for the encoder values.
  cedar::aux::math::IntLimitsParameterPtr getEncoderLimits() const;

  //!@brief Returns the current encoder value of the left and right wheel.
  std::vector<int> getEncoders() const;

  /*!@brief Sets the encoder values of both wheels.
   * @param[in] encoders encoder value for the left and right wheel
   */
  void setEncoders(const std::vector<int>& encoders);

  //! @see base class.
  CEDAR_DECLARE_DEPRECATED( void reset(); );

  /*!@brief Sets the speed of the left and right wheel.
   * @param[in] wheelSpeedPulses The wheel speed of the left and right wheel to be set [in pulses/s].
   */
  virtual void setWheelSpeedPulses(const std::vector<cedar::unit::Frequency>& wheelSpeedPulses);

  cv::Mat pulsesToWheelSpeed(cedar::unit::Time dt, cv::Mat input, ComponentDataType type);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  std::vector<cedar::unit::Frequency> convertWheelSpeedToPulses
                   (
                     const std::vector<cedar::unit::Velocity>& wheelSpeed
                   ) const;

  std::vector<cedar::unit::Velocity> convertPulsesToWheelSpeed
                   (
                     const std::vector<cedar::unit::Frequency>& wheelSpeed
                   ) const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet
  void init();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  static const cedar::dev::Component::ComponentDataType ENCODERS;
  static const cedar::dev::Component::ComponentDataType ENCODERS_CHANGE;

protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! number of pulses per revolution of wheel
  cedar::aux::DoubleParameterPtr _mNumberOfPulsesPerRevolution;
  //! limits for the encoder value
  cedar::aux::math::IntLimitsParameterPtr _mEncoderLimits;
}; // class cedar::dev::kteam::Drive

#endif // CEDAR_DEV_KTEAM_DRIVE_H
