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

    File:        Locomotion.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: An object of this class represents the drive of a mobile robot.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_LOCOMOTION_H
#define CEDAR_DEV_LOCOMOTION_H

// CEDAR INCLUDES
#include "cedar/devices/namespace.h"
#include "cedar/devices/Component.h"
#include "cedar/auxiliaries/math/VelocityLimitsParameter.h"
#include "cedar/auxiliaries/math/AngularVelocityLimitsParameter.h"
#include "cedar/units/Velocity.h"
#include "cedar/units/AngularVelocity.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/UnitDataTemplate.fwd.h"

// SYSTEM INCLUDES

/*!@brief An object of this class represents the locomotion of a mobile robot.
 *
 * This is an abstract class with functions and attributes common to drives of mobile robots. Mobile robots are e.g
 * robots with differential drives or walking robots.
 */
class cedar::dev::Locomotion : public cedar::dev::Component
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor
  Locomotion();

  Locomotion(cedar::dev::ChannelPtr channel);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Returns the forward velocity [in m/s] of the robot.
  cedar::unit::Velocity getForwardVelocity() const;

  //!@brief Returns the turning rate [in rad/s] of the robot.
  cedar::unit::AngularVelocity getTurningRate() const;

  //!@brief Stops the robot.
  //@todo: change to a new braking mechanisms
  CEDAR_DECLARE_DEPRECATED( virtual void stop(); );

  //!@brief Sets the current forward velocity [in m/s] of the robot.
  void setForwardVelocity(cedar::unit::Velocity forwardVelocity);

  //!@brief Sets the turning rate [in rad/s] of the robot.
  void setTurningRate(cedar::unit::AngularVelocity turningRate);

  //!@brief Sets both the forward velocity [in m/s] and turning rate [in rad/s] of the robot.
  void setForwardVelocityAndTurningRate
               (
                 cedar::unit::Velocity forwardVelocity,
                 cedar::unit::AngularVelocity turningRate
               );

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief Checks the given forward velocity against the configured limits and thresholds it if necessary.
   * @param[in,out] forwardVelocity forward velocity to be thresholded [m/s]
   */
  virtual void thresholdForwardVelocity(cedar::unit::Velocity& forwardVelocity) const;

  /*!@brief Checks the given turning rate against the configured limits and thresholds it if necessary.
   * @param[in,out] turningRate turning rate to be thresholded [m/s]
   */
  virtual void thresholdTurningRate(cedar::unit::AngularVelocity& turningRate) const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void init();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:

public:
  static const cedar::dev::Component::ComponentDataType FORWARD_VELOCITY;
  static const cedar::dev::Component::ComponentDataType TURNING_RATE;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! limits for the forward velocity of the robot [m/s]
  cedar::aux::math::VelocityLimitsParameterPtr _mForwardVelocityLimits;
  //! limits for the turning rate of the robot [rad/s]
  cedar::aux::math::AngularVelocityLimitsParameterPtr _mTurningRateLimits;

}; // class cedar::dev::Locomotion
#endif // CEDAR_DEV_LOCOMOTION_H
