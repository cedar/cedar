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
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/MapParameter.h"
#include "cedar/auxiliaries/StringParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/FactoryManager.fwd.h"
#include "cedar/devices/Component.fwd.h"
#include "cedar/devices/ComponentSlot.fwd.h"
#include "cedar/devices/Robot.fwd.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/enable_shared_from_this.hpp>
#endif // Q_MOC_RUN
#include <vector>
#include <string>
#include <map>

/*!@brief Slot for a single robot component.
 *
 * @todo More detailed description of the class.
 */
class cedar::dev::ComponentSlot : public cedar::aux::Configurable, public boost::enable_shared_from_this<cedar::dev::ComponentSlot>
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend std::ostream& operator<<(std::ostream& stream, const cedar::dev::ComponentSlot& slot);

  friend std::ostream& operator<<(std::ostream& stream, cedar::dev::ConstComponentSlotPtr slot);

  friend std::ostream& operator<<(std::ostream& stream, cedar::dev::ComponentSlotPtr slot);

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  //!@brief a singleton factory manager for components
  typedef cedar::aux::Singleton<cedar::aux::FactoryManager<cedar::dev::ComponentPtr> > FactorySingleton;

  typedef std::map<std::string, cedar::aux::ConfigurationNode> ComponentConfigurations;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  ComponentSlot(cedar::dev::RobotPtr robot, const std::string& name);

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
    auto robot = mRobot.lock();
    CEDAR_ASSERT(robot);
    return robot;
  }

  /*!@brief Returns the component currently docked to this component slot.
   *
   * @returns pointer to the component
   */
  cedar::dev::ComponentPtr getComponent();

  void readConfiguration(const cedar::aux::ConfigurationNode& node);

  //!@brief Lists all channels in this component.
  std::vector<std::string> listConfigurations() const;

  //!@brief Checks if a channel of the given name exists.
  bool hasConfiguration(const std::string& name) const;

  //!@brief Sets the channel for this component.
  void instantiateConfiguration(const std::string& configurationName);

  //! Returns a path that identifies this component.
  std::string getPath() const;

  //! Return the name of the slot
  std::string getName() const;

  //! Return the name of the configuration
  std::string getConfigurationName() const;

  //! returns the component icon path
  QString getIconPath() const;

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
  cedar::dev::RobotWeakPtr mRobot;

  //! component that is currently docked to this slot
  cedar::dev::ComponentPtr mComponent;

  //! Name of the component.
  std::string mName;

  //! Path of the component icon
  QString mIconPath;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Type of the channel stored in this component.
  cedar::aux::StringParameterPtr _mConfigurationName;

  //! mapping of channel types to class names of components
  std::map<std::string, std::string> mComponentTypeIds;

  ComponentConfigurations _mComponentConfigurations;

  cedar::aux::ConfigurationNode mCommonParameters;

}; // class cedar::dev::ComponentSlot
#endif // CEDAR_DEV_COMPONENT_SLOT_H
