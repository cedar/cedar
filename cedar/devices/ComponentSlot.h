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

    File:        ComponentSlot.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 23

    Description: Slot for components of a robot (e.g., a kinematic chain).

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_COMPONENT_SLOT_H
#define CEDAR_DEV_COMPONENT_SLOT_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/devices/namespace.h"

// SYSTEM INCLUDES

/*!@brief Slot for a single robot component.
 *
 * @todo More detailed description of the class.
 */
class cedar::dev::ComponentSlot
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  //!@brief a singleton factory manager for components
  typedef cedar::aux::Singleton<cedar::aux::FactoryManager<cedar::dev::ComponentPtr> > FactorySingleton;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Returns a pointer to the robot this component slot belongs to.
   *
   * @returns pointer to this slot's robot
   */
  inline cedar::dev::RobotPtr getRobot() const
  {
    return mRobot;
  }

  /*!@brief Returns the component currently docked to this component slot.
   *
   * @returns pointer to the component
   */
  cedar::dev::ComponentPtr getComponent();

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
  //! robot the component slot belongs to
  cedar::dev::RobotPtr mRobot;

  //! component that is currently docked to this slot
  cedar::dev::ComponentPtr mComponent;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! name of the component slot
  cedar::aux::StringParameterPtr _mName;

  cedar::aux::StringParameterPtr _mChannelType;

  //! mapping of channel types to class names of components
  cedar::aux::StringMapParameterPtr _mComponentTypeIds;
}; // class cedar::dev::ComponentSlot
#endif // CEDAR_DEV_COMPONENT_SLOT_H
