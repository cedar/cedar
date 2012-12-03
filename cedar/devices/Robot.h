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

    File:        Robot.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 23

    Description: Manages all components of a robot.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOT_H
#define CEDAR_DEV_ROBOT_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/NamedConfigurable.h"
#include "cedar/auxiliaries/namespace.h"
#include "cedar/devices/namespace.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>
#include <map>

/*!@brief Base class for robots.
 *
 * @todo More detailed description of the class.
 */
class cedar::dev::Robot : public cedar::aux::NamedConfigurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
  typedef cedar::aux::ObjectMapParameterTemplate<cedar::dev::ComponentSlot> ComponentSlotParameter;
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(ComponentSlotParameter);

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  Robot();
  //!@brief destructor
  virtual ~Robot();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! @brief Returns a pointer to the component docked to the component slot with the name @em componentSlotName.
   *
   * @return Pointer to the requested component.
   * @param[in] componentSlotName name of the component slot, whose component is to be returned.
   */
  ComponentPtr getComponent(const std::string& componentSlotName) const;

  /*! @brief Returns a vector containing all the available slot names of the robot.
   *
   * @return vector of available slot names
   */
  std::vector<std::string> getComponentSlotNames() const;

  /*! @brief Returns the component slot with the name @em componentSlotName.
   *
   * @return component slot
   * @param[in] componentSlotName name of the component slot that is to be returned
   */
  ComponentSlotPtr getComponentSlot(const std::string& componentSlotName) const;

  /*! @brief Returns the name of the robot.
   *
   * @return Name of the robot.
   */
  const std::string& getName() const;

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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! mapping of all slot names to their component slots
  ComponentSlotParameterPtr _mComponentSlots;

}; // class cedar::dev::Robot
#endif // CEDAR_DEV_ROBOT_H
