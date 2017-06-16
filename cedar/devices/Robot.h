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
#include "cedar/auxiliaries/FileParameter.h"
#include "cedar/auxiliaries/gl/GlobalScene.h"
#include "cedar/devices/gl/RobotVisualisation.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/ObjectMapParameterTemplate.fwd.h"
#include "cedar/devices/Channel.fwd.h"
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

/*!@brief Base class for robots.
 */
class cedar::dev::Robot : public cedar::aux::NamedConfigurable,
                          public boost::enable_shared_from_this<cedar::dev::Robot>
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend CEDAR_DEV_LIB_EXPORT std::ostream& operator<<(std::ostream& stream, const cedar::dev::Robot& robot);

  friend CEDAR_DEV_LIB_EXPORT std::ostream& operator<<(std::ostream& stream, cedar::dev::ConstRobotPtr robot);

  friend CEDAR_DEV_LIB_EXPORT std::ostream& operator<<(std::ostream& stream, cedar::dev::RobotPtr robot);

  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef std::map<std::string, cedar::dev::ComponentSlotPtr> ComponentSlotParameter;

  typedef cedar::aux::ObjectMapParameterTemplate<cedar::dev::Channel, cedar::aux::allocationPolicies::OnDemand<cedar::dev::Channel> > ChannelParameter;
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(ChannelParameter);

  typedef cedar::aux::NamedConfigurable Super;

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

  //!@brief Read a configuration for all registered parameters from a cedar::aux::ConfigurationNode.
  virtual void readConfiguration(const cedar::aux::ConfigurationNode& node);

  //!@returns A list of all components of this robot.
  std::vector<std::string> listComponentSlots() const;

  //!@returns A list of all channels of this robot.
  std::vector<std::string> listChannels() const;

  //!@brief Sets the configuration for all components in the robot.
  void instantiateComponentConfiguration(const std::string& configurationName);

  //!@brief Sets the channel for all components in the robot.
  cedar::dev::ConstChannelPtr getChannel(const std::string& channel) const;

  //!@brief Sets the channel for all components in the robot.
  cedar::dev::ChannelPtr getChannel(const std::string& channel);

  //!@brief Tests whether an instance has been created for the given channel.
  bool hasChannelInstance(const std::string& channel) const;

  template <typename T>
  inline boost::shared_ptr<T> getComponent(const std::string& slotName) const
  {
    cedar::dev::ComponentPtr component = this->getComponent(slotName);
    return boost::dynamic_pointer_cast<T>(component);
  }

  //! Allocates the channel of the given name.
  void allocateChannel(const std::string& channelName);

  //! start the hardware, start communication
  void startCommunicationOfComponents(bool suppressUserSideInteraction=false);
  //! stop the hardware
  void stopCommunicationOfComponents();
  //  HW is on?
  bool areSomeComponentsCommunicating() const;
  //  HW is on?
  bool areAllComponentsCommunicating() const;

  cedar::dev::gl::RobotVisualisationPtr getVisualisationPtr() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------  
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Opens all channels that are in use.
   */
  void openChannels();

  /*!@brief Closes all channels that are in use.
   */
  void closeChannels();

  /*!@brief Returns the number of channels that are open.
   */
  unsigned int countOpenChannels() const;

  /*!@brief Returns the overall number of channels.
   */
  unsigned int getNumberOfChannels() const;


  //!@brief Read a configuration for all registered parameters from a cedar::aux::ConfigurationNode.
  virtual void readDescription(const cedar::aux::ConfigurationNode& node);

  //!@brief Read out visualisation parameters, construct a RobotVisualisation and pass it to our simulated scene.
  void readVisualisation(const cedar::aux::ConfigurationNode& node);

  //!@brief Checks the robot description for consistency. Issues are written to the log as warnings and errors.
  void performConsistencyCheck() const;

  //! Reads and instantiates the slots given in the configuration.
  void readComponentSlotInstantiations(const cedar::aux::ConfigurationNode& node);

  //! Reads and stacks the component connections.
  void readComponentConnections(const cedar::aux::ConfigurationNode& node);

  //! Reads and stacks the component connections.
  void readSaveZone(const cedar::aux::ConfigurationNode& node);

  //! Reads the channels and their configurations and instantiates the selected channels.
  void readChannels(const cedar::aux::ConfigurationNode& node);

  //! Clears all components and channels.
  void clear();

  //! Merges channel parameters from the description and the instantiation
  void appendChannelConfiguration(const std::string& channelName, const cedar::aux::ConfigurationNode& node);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! Map of all the channel names to their type names.
  std::map<std::string, std::string> mChannelTypes;

  //! Map of all the channel names to channel configurations.
  std::map<std::string, cedar::aux::ConfigurationNode> mChannelConfigurations;

  //! Map from channel names to instances.
  std::map<std::string, cedar::dev::ChannelPtr> mChannelInstances;

  //! mapping of all slot names to their component slots
  ComponentSlotParameter mComponentSlots;

  //! Configurable object used for storing robot setup in a separate file.
  //cedar::aux::ConfigurablePtr mRobotDescription;

  //! a pointer to the actual visualisation of the robot
  cedar::dev::gl::RobotVisualisationPtr mpRobotVisualisation;

  //! safe zone coordinates
  std::vector<float> mLowerSafeZone;
  std::vector<float> mUpperSafeZone;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::Robot
#endif // CEDAR_DEV_ROBOT_H
