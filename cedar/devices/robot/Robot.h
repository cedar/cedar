/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
    Date:        2010 11 08

    Description: Manages all components of a robot and the communication with the hardware.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOT_ROBOT_H
#define CEDAR_DEV_ROBOT_ROBOT_H

// CEDAR INCLUDES
#include "cedar/devices/robot/namespace.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>
#include <set>
#include <map>

/*!@brief Base class for robots.
 *
 * @todo More detailed description of the class.
 */
class cedar::dev::robot::Robot
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  Robot();
  //!@brief destructor
  virtual ~Robot() = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! @brief Returns a pointer to the component with the name @em componentName.
   *
   * @return Pointer to the requested component.
   * @param[in] rComponentName Name of the component that is to be returned.
   */
  ComponentPtr& getComponent(const std::string& rComponentName);

  /*! @brief Creates a specified component.
   *
   * Abstract factory class, which will be implemented by the concrete subclass.
   */
  virtual ComponentPtr createComponent(const std::string& rComponentName) = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*! @brief Checks if a component with the supplied @em rComponentName exists within the robot.
   *
   * @param[in] rComponentName Name of the subcomponent we need to check.
   */
  bool isComponentAvailable(const std::string& rComponentName) const;

  /*! @brief Checks if a subcomponent with the supplied @em rComponentName exists for a parent component
   * with the name @em parentComponentName.
   *
   * @param[in] rComponentName Name of the subcomponent we need to check.
   * @param[in] rParentComponentName Name of the component, that might be the parent of @em componentName.
   */
  bool isComponentAvailable(
                             const std::string& rComponentName,
                             const std::string& rParentComponentName
                           ) const;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! map of pointers to all sub components
  std::map<std::string, ComponentPtr> mComponents;

  //! names of all components and their corresponding sub-components
  std::map<std::string, std::set<std::string> > _mSubComponentNames;

  //! Name of the robot.
  std::string _mName;

private:
  // none yet
}; // class cedar::dev::robot::Robot
#endif // CEDAR_DEV_ROBOT_ROBOT_H
