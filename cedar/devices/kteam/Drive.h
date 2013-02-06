/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

#ifndef CEDAR_DEV_ROBOT_KTEAM_DRIVE_H
#define CEDAR_DEV_ROBOT_KTEAM_DRIVE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/namespace.h"
#include "cedar/auxiliaries/math/IntLimitsParameter.h"
#include "cedar/devices/robot/DifferentialDrive.h"
#include "cedar/devices/kteam/namespace.h"
#include "cedar/devices/communication/namespace.h"

// SYSTEM INCLUDES
#include <vector>

/*!@brief The differential drive of a pulse-width-modulation (PWM) driven robot.
 *
 * This is an abstract class with functions and attributes common to differential drive robots with
 * pulse-width-modulation driven wheels. These are, for instance, the mobile robots E-Puck, Khepera and Koala.
 */
class cedar::dev::kteam::Drive : public cedar::dev::robot::DifferentialDrive
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The constructor
  Drive(cedar::dev::com::SerialCommunicationPtr communication);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Returns the number of pulses per wheel-revolution.
  double getNumberOfPulsesPerRevolution() const;

  //!@brief Returns the distance a wheel moves for a single pulse [in m].
  double getDistancePerPulse() const;

  //!@brief Returns the limits for the encoder values.
  cedar::aux::math::IntLimitsParameterPtr getEncoderLimits() const;

  //!@brief Returns the current encoder value of the left and right wheel.
  virtual std::vector<int> getEncoders() const;

  /*!@brief Sets the encoder values of both wheels.
   * @param[in] encoders encoder value for the left and right wheel
   */
  virtual void setEncoders(const std::vector<int>& encoders);

  //! @see Base class.
  virtual void reset();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! @see Base class.
  virtual void sendMovementCommand();

  //!@brief Returns the character used to start a command which sets the robots speed.
  virtual std::string getCommandSetSpeed() const;

  //!@brief Returns the character used to set the encoders of the robot.
  virtual std::string getCommandSetEncoder() const;

  //!@brief Returns the character used to get the encoders of the robot.
  virtual std::string getCommandGetEncoder() const;

  /*!@brief Determines the expected answer for a command with a given command string prefix.
   *
   * Command strings always begin with an upper-case character (e.g., 'D').
   * The correct answer to every command will begin with the lower-case version
   * of that character (e.g., 'd').
   *
   * @param[in] commandString the command prefix
   * @note The method makes a copy of the given string to not alter it.
   */
  std::string determineCorrectAnswer(std::string commandString) const;

  /*!@brief Checks whether the answer begins with the correct first character.
   * The method throws an exception when the answer is not correct.
   * @param[in] answer the answer to check
   * @param[in] command the command string producing the received answer
   * @param[in] expectedAnswer The answer expected from the robot.
   */
  void checkAnswer(const std::string& answer, const std::string& command, const std::string& expectedAnswer = "") const;

  /*!@brief Checks whether the stream is still valid.
   * @param[in] answerStream string stream to be checked
   * @param[in] atEndOfStream denotes whether the stream is believed to be at the end
   */
  void checkStream(const std::istringstream& answerStream, bool atEndOfStream) const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Returns the serial communication object.
  cedar::dev::com::SerialCommunicationPtr getSerialCommunication() const;

private:
  //! serial communication channel
  cedar::dev::com::SerialCommunicationPtr mSerialCommunication;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! number of pulses per revolution of wheel
  cedar::aux::DoubleParameterPtr _mNumberOfPulsesPerRevolution;
  //! limits for the encoder value
  cedar::aux::math::IntLimitsParameterPtr _mEncoderLimits;
}; // class cedar::dev::kteam::Drive

#endif // CEDAR_DEV_ROBOT_KTEAM_DRIVE_H
