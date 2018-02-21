/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ArmSerial.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 07 30

    Description: Header file for the class cedar::dev::kteam::khepera::ArmSerial.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_KTEAM_KHEPERA_ARM_SERIAL_H
#define CEDAR_DEV_KTEAM_KHEPERA_ARM_SERIAL_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/kteam/khepera/Arm.h"

// FORWARD DECLARATIONS
#include "cedar/devices/kteam/khepera/ArmSerial.fwd.h"

// SYSTEM INCLUDES
#include "cedar/auxiliaries/StringParameter.h"


/*!@brief Component implementation of the Khepera arm using a serial channel.
 */
class cedar::dev::kteam::khepera::ArmSerial : public cedar::dev::kteam::khepera::Arm
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ArmSerial();

  //! Destructor.
  ~ArmSerial();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Sets the arm position.
  void setArmPosition(unsigned int position);

  //!@brief Returns the current arm position.
  unsigned int getArmPosition();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void applyArmPosition(cv::Mat angle);

  cv::Mat measureArmPosition();

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
  //! command used to set the arm position
  cedar::aux::StringParameterPtr _mCommandSetArmPosition;
  //! command used to get the arm position
  cedar::aux::StringParameterPtr _mCommandGetArmPosition;
  //! answer expected when getting the arm position
  cedar::aux::StringParameterPtr _mAnswerGetArmPosition;

}; // class cedar::dev::kteam::khepera::ArmSerial

#endif // CEDAR_DEV_KTEAM_KHEPERA_ARM_SERIAL_H

