/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
#include "cedar/devices/namespace.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/MapParameter.h"
#include "cedar/auxiliaries/StringParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/FactoryManager.fwd.h"

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
    return mRobot;
  }

  /*!@brief Returns the component currently docked to this component slot.
   *
   * @returns pointer to the component
   */
  cedar::dev::ComponentPtr getComponent();

  void readConfiguration(const cedar::aux::ConfigurationNode& node);

  //!@brief Lists all channels in this component.
  std::vector<std::string> listChannels() const;

  //!@brief Checks if a channel of the given name exists.
  bool hasChannel(const std::string& name) const;

  //!@brief Sets the channel for this component.
  void setChannel(const std::string& channel);

  //! Returns a path that identifies this component.
  std::string getPath() const;

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

  //! Name of the component.
  std::string mName;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Type of the channel stored in this component.
  cedar::aux::StringParameterPtr _mChannelType;

  //! mapping of channel types to class names of components
  std::map<std::string, std::string> mComponentTypeIds;

  ComponentConfigurations _mComponentConfigurations;

  cedar::aux::ConfigurationNode mCommonParameters;

}; // class cedar::dev::ComponentSlot
#endif // CEDAR_DEV_COMPONENT_SLOT_H
