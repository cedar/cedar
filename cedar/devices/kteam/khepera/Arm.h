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

    File:        Arm.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 07 30

    Description: Header file for the class cedar::dev::kteam::khepera::Arm.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_KTEAM_KHEPERA_ARM_H
#define CEDAR_DEV_KTEAM_KHEPERA_ARM_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/Component.h"
#include "cedar/auxiliaries/math/UIntLimitsParameter.h"

// FORWARD DECLARATIONS
#include "cedar/devices/kteam/khepera/Arm.fwd.h"

// SYSTEM INCLUDES


/*!@brief Implementation of an arm component for Khepera robots.
 */
class cedar::dev::kteam::khepera::Arm : public cedar::dev::Component
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Arm();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Sets the arm position.
  virtual void setArmPosition(unsigned int position) = 0;

  //!@brief Returns the current arm position.
  virtual unsigned int getArmPosition() = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  virtual bool applyBrakeSlowlyController();
  virtual bool applyBrakeNowController();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  static unsigned int POSITION;
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! limits for the arm position
  cedar::aux::math::UIntLimitsParameterPtr _mArmPositionLimits;

private:
  // none yet

}; // class cedar::dev::kteam::khepera::Arm

#endif // CEDAR_DEV_KTEAM_KHEPERA_ARM_H

