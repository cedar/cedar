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

    File:        Network.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 19

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_NETWORK_H
#define CEDAR_PROC_NETWORK_H

// LOCAL INCLUDES
#include "cedar/processing/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <vector>


/*!@brief A collection of cedar::proc::Elements forming some logical unit.
 *
 *        The network is at the heart of the processing framework. It is used for loading and writing architectures, and
 *        holds a list of elements (e.g., steps, triggers) and the connections between them.
 *
 *        Whenever you want to connect up a number of processing steps, you should first add them to a network and then
 *        use the connect functions of this network. This ensures proper management of storage and deletion of all the
 *        elements in the network.
 *
 * @todo Change the name of the class to Module?
 * @todo Add a slot which reacts to name changes of elements (update map of names to ptrs)
 */
class cedar::proc::Network
{
  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! Type of the data connection list.
  typedef std::vector<cedar::proc::DataConnectionPtr> DataConnectionVector;

  //! Type of the trigger connection list.
  typedef std::vector<cedar::proc::TriggerConnectionPtr> TriggerConnectionVector;

  //! Type of the map of elements.
  typedef std::map<std::string, cedar::proc::ElementPtr> ElementMap;
public:
  //! Iterator type of the element map.
  typedef ElementMap::iterator ElementMapIterator;

  //! Const iterator type of the element map.
  typedef ElementMap::const_iterator ElementMapConstIterator;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Network();

  //!@brief The destructor.
  ~Network();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Reads the network from a configuration node.
   */
  void readFrom(const cedar::aux::ConfigurationNode& root);

  /*!@brief Writes the network to a configuration node.
   */
  void writeTo(cedar::aux::ConfigurationNode& root);

  /*!@brief Reads the network from a given json file.
   *
   *        This file must have the same format as the one output by cedar::proc::Network::writeFile.
   */
  void readFile(const std::string& filename);

  /*!@brief Writes the network to a given json file.
   */
  void writeFile(const std::string& filename);

  /*!@brief Removes an element from the network.
   *
   * @remark Before calling this function, you should remove all connection to the element.
   */
  void remove(cedar::proc::ElementPtr element);

  /*!@brief Adds a new element with the type given by className and the name instanceName.
   *
   * @param className    Identifier of the type registered at cedar::proc::DeclarationRegistry.
   * @param instanceName Name to be given to the new element.
   */
  void add(std::string className, std::string instanceName);

  /*!@brief Adds a given element to the network under the instanceName.
   *
   * @param element      Pointer to the element to be added.
   * @param instanceName Name to be given to the instance.
   */
  void add(cedar::proc::ElementPtr element, std::string instanceName);

  /*!@param    Adds a given element to the network.
   *
   * @param    element The element to add to the network.
   *
   * @remarks  The element is assumed to have a valid name, and this name is used to identify it within the network.
   */
  void add(cedar::proc::ElementPtr element);

  /*!@brief Returns the element with the given name as a pointer of the specified type.
   */
  template <class T>
  boost::shared_ptr<T> getElement(const std::string& name)
  {
    return boost::shared_dynamic_cast<T>(this->getElement(name));
  }

  /*!@brief  Returns a pointer to the element with the given name.
   *
   * @throws cedar::proc::InvalidNameException if no element is found with the given name.
   */
  cedar::proc::ElementPtr getElement(const std::string& name);

  /*!@brief Connects data slots of two cedar::proc::Connectable elements.
   *
   * @param source Identifier of the source data slot. This must be of the form "elementName.outputSlotName".
   * @param target Identifier of the target data slot. This must be of the form "elementName.inputSlotName".
   */
  void connectSlots(const std::string& source, const std::string& target);

  /*!@brief Connects a cedar::proc::Trigger to a cedar::proc::Triggerable.
   *
   *        When the two elements are connected successfully, then target's onTrigger method is called every time source
   *        is triggered.
   */
  void connectTrigger(cedar::proc::TriggerPtr source, cedar::proc::TriggerablePtr target);

  /*!@brief Returns, whether source is connected to target.
   *
   * @param source Identifier of the source data slot. This must be of the form "elementName.outputSlotName".
   * @param target Identifier of the target data slot. This must be of the form "elementName.inputSlotName".
   */
  bool isConnected(const std::string& source, const std::string& target);

  /*!@brief Returns, whether target receives trigger signals from the source trigger.
   */
  bool isConnected(cedar::proc::TriggerPtr source, cedar::proc::TriggerablePtr target);

  /*!@brief Deletes the connection between the data slots.
   *
   * @param source Identifier of the source data slot. This must be of the form "elementName.outputSlotName".
   * @param target Identifier of the target data slot. This must be of the form "elementName.inputSlotName".
   */
  void disconnectSlots(const std::string& source, const std::string& target);

  /*!@brief Deletes the connection between source and target.
   */
  void disconnectTrigger(cedar::proc::TriggerPtr source, cedar::proc::TriggerablePtr target);

  /*!@brief Writes all the connections originating from a source connectable into a vector.
   *
   * @param source         The connectable source.
   * @param sourceDataName The name of the slot from which to look list outgoing connections.
   * @param connections    Vector into which the connections are written.
   */
  void getDataConnections(
                           cedar::proc::ConnectablePtr source,
                           const std::string& sourceDataName,
                           std::vector<cedar::proc::DataConnectionPtr>& connections
                         );

  /*!@brief Returns a const reference to the map of names to elements stored in the network.
   */
  const ElementMap& elements() const;

  /*!@brief Updates the name stored for the object.
   */
  void updateObjectName(cedar::proc::Element* object);

  /*!@brief Sends a reset command to all connectable elements in the network.
   */
  void reset();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Reads the network from a configuration node using the first version of the format.
   */
  void readFromV1(const cedar::aux::ConfigurationNode& root);

  /*!@brief Writes the meta data to the configuration.
   */
  void writeMetaData(cedar::aux::ConfigurationNode& root);

  /*!@brief Reads steps from the configuration node and adds them to the network.
   */
  void readSteps(const cedar::aux::ConfigurationNode& root);

  /*!@brief Writes the steps in the network to the configuration node.
   */
  void writeSteps(cedar::aux::ConfigurationNode& root);

  /*!@brief Reads triggers from a configuration node and adds them to the network.
   */
  void readTriggers(const cedar::aux::ConfigurationNode& root);

  /*!@brief Writes the triggers in the network to the configuration node.
   */
  void writeTriggers(cedar::aux::ConfigurationNode& root);

  /*!@brief Reads a data connection from a configuration node and adds it to the network.
   */
  void readDataConnection(const cedar::aux::ConfigurationNode& root);

  /*!@brief Writes a data connection to the configuration node.
   */
  void writeDataConnection(cedar::aux::ConfigurationNode& root, const cedar::proc::DataConnectionPtr connection);

  /*!@brief Reads data connections from a configuration node and adds them to the network.
   */
  void readDataConnections(const cedar::aux::ConfigurationNode& root);

  /*!@brief Writes the data connections in the network to the configuration node.
   */
  void writeDataConnections(cedar::aux::ConfigurationNode& root);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Map associating element names to elements.
  ElementMap mElements;

  //! List of data connections in the network.
  DataConnectionVector mDataConnections;

  //! List of trigger connections in the network.
  TriggerConnectionVector mTriggerConnections;

}; // class cedar::proc::Network

#endif // CEDAR_PROC_NETWORK_H

