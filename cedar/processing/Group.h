/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Group.h

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

#ifndef CEDAR_PROC_GROUP_H
#define CEDAR_PROC_GROUP_H

// CEDAR INCLUDES
#include "cedar/processing/Connectable.h"
#include "cedar/processing/NetworkPath.h"
#include "cedar/processing/Triggerable.h"
#include "cedar/auxiliaries/MapParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/boostSignalsHelper.h"
#include "cedar/units/Time.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/StringVectorParameter.fwd.h"
#include "cedar/auxiliaries/ParameterLink.fwd.h"
#include "cedar/processing/LoopedTrigger.fwd.h"
#include "cedar/processing/Group.fwd.h"
#include "cedar/processing/Trigger.fwd.h"
#include "cedar/processing/TriggerConnection.fwd.h"
#include "cedar/processing/GroupFileFormatV1.fwd.h"
#include "cedar/processing/consistency/ConsistencyIssue.fwd.h"
#include "cedar/processing/sinks/GroupSink.fwd.h"

// SYSTEM INCLUDES
#include <QThread>
#include <vector>
#ifndef Q_MOC_RUN
  #include <boost/signals2/signal.hpp>
  #include <boost/signals2/connection.hpp>
#endif
#include <map>
#include <set>
#include <list>
#include <string>

/*!@brief A collection of cedar::proc::Elements forming some logical unit.
 *
 *        The group is at the heart of the processing framework. It is used for loading and writing architectures, and
 *        holds a list of elements (e.g., steps, triggers) and the connections between them.
 *
 *        Whenever you want to connect up a number of processing steps, you should first add them to a group and then
 *        use the connect functions of this group. This ensures proper management of storage and deletion of all the
 *        elements in the group.
 */
class cedar::proc::Group : public QThread, public cedar::proc::Connectable, public cedar::proc::Triggerable
{
  Q_OBJECT

  friend class cedar::proc::GroupFileFormatV1;
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  struct ParameterLinkInfo
  {
    std::string getSourceElementPath() const;
    std::string getTargetElementPath() const;

    std::string getSourceParameterPath() const;
    std::string getTargetParameterPath() const;

    cedar::aux::ParameterLinkPtr mParameterLink;

    cedar::proc::ElementPtr mSourceElement;
    cedar::proc::ElementPtr mTargetElement;

    cedar::proc::GroupWeakPtr mGroup;
  };

  //! Enum that represents the change of a given connection.
  enum ConnectionChange
  {
    //! The connection was added.
    CONNECTION_ADDED,
    //! The was removed.
    CONNECTION_REMOVED,
  };

 signals:
  void stepNameChanged(const std::string& from, const std::string& to);
  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! Type of the trigger connection list.
  typedef std::vector<cedar::proc::TriggerConnectionPtr> TriggerConnectionVector;

  typedef std::vector<cedar::proc::TriggerablePtr> TriggerableVector;

public:
  //! Type of the data connection list.
  typedef std::vector<cedar::proc::DataConnectionPtr> DataConnectionVector;

  //! Type of the map of elements.
  typedef std::map<std::string, cedar::proc::ElementPtr> ElementMap;

  //! Iterator type of the element map.
  typedef ElementMap::iterator ElementMapIterator;

  //! Const iterator type of the element map.
  typedef ElementMap::const_iterator ElementMapConstIterator;

  typedef std::map<std::string, bool> ConnectorMap;
  typedef cedar::aux::MapParameter<bool> ConnectorMapParameter;
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(ConnectorMapParameter);


  friend class cedar::proc::sinks::GroupSink;
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Group();

  //!@brief The destructor.
  ~Group();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Reads the group from a configuration node.
   */
  void readConfiguration(const cedar::aux::ConfigurationNode& root);

  /*!@deprecated Use readConfiguration instead.
   */
  CEDAR_DECLARE_DEPRECATED(void readFrom(const cedar::aux::ConfigurationNode& root))
  {
    this->readConfiguration(root);
  }

  /*!@brief Writes the group to a configuration node.
   */
  void writeConfiguration(cedar::aux::ConfigurationNode& root) const;

  /*!@deprecated Use writeConfiguration instead.
   */
  CEDAR_DECLARE_DEPRECATED(void writeTo(cedar::aux::ConfigurationNode& root))
  {
    this->writeConfiguration(root);
  }

  /*!@brief Reads the group from a given json file.
   *
   *        This file must have the same format as the one output by cedar::proc::Group::writeFile.
   *
   * @deprecated Use readJson (from cedar::aux::Configurable)
   */
  CEDAR_DECLARE_DEPRECATED(void readFile(const std::string& filename))
  {
    this->readJson(filename);
  }

  /*!@brief Writes the group to a given json file.
   *
   * @deprecated Use writeJson (from cedar::aux::Configurable)
   */
  CEDAR_DECLARE_DEPRECATED(void writeFile(const std::string& filename))
  {
    this->writeJson(filename);
  }

  /*!@brief Removes an element from the group.
   *
   * @remark Before calling this function, you should remove all connections to the element.
   */
  void remove(cedar::proc::ConstElementPtr element);

  /*!@brief calls remove() for every element of the group
   */
  void removeAll();

  /*!@brief Creates a new element with the type given by className and the name instanceName.
   *
   * @param className    Identifier of the type registered at cedar::aux::DeclarationManager.
   * @param instanceName Name to be given to the new element.
   */
  void create(std::string className, std::string instanceName);

  /*!@brief Adds a given element to the group under the instanceName.
   *
   * @param element      Pointer to the element to be added.
   * @param instanceName Name to be given to the instance.
   */
  void add(cedar::proc::ElementPtr element, std::string instanceName);

  /*!@brief    Adds a given element to the group.
   *
   * @param    element The element to add to the group.
   *
   * @remarks  The element is assumed to have a valid name, and this name is used to identify it within the group.
   */
  void add(cedar::proc::ElementPtr element);

  /*!@brief adds a list of elements and takes care of existing connections between those elements
   */
  void add(std::list<cedar::proc::ElementPtr> elements);

  void addConnector(const std::string& name, bool input);

  void removeConnector(const std::string& name, bool input);

  /*!@brief Duplicates an existing element.
   *
   * @param elementName Identifier of the existing element.
   * @param newName Name to be given to the new element.
   * @returns The new name of the element.
   */
  std::string duplicate(const std::string& elementName, const std::string& newName = "");

  /*!@brief unmodifiedName unmodified name, possibly non-unique in group
   * @return unique name created by attaching a number if name is already taken
   */
  CEDAR_DECLARE_DEPRECATED(std::string getUniqueName(const std::string& unmodifiedName) const);

  /*!@brief Returns the element with the given name as a pointer of the specified type.
   */
  template <class T>
  boost::shared_ptr<T> getElement(const cedar::proc::NetworkPath& name)
  {
    return boost::dynamic_pointer_cast<T>(this->getElement(name));
  }

  /*!@brief Returns the element with the given name as a const pointer of the specified type.
   */
  template <class T>
  boost::shared_ptr<const T> getElement(const cedar::proc::NetworkPath& name) const
  {
    return boost::dynamic_pointer_cast<const T>(this->getElement(name));
  }

  /*!@brief  Returns a pointer to the element with the given name. Uses the const function getElement.
   *
   * @throws cedar::aux::InvalidNameException if no element is found with the given name.
   */
  cedar::proc::ElementPtr getElement(const cedar::proc::NetworkPath& name);

  /*!@brief  Returns a const pointer to the element with the given name.
   *
   * @throws cedar::aux::InvalidNameException if no element is found with the given name.
   */
  cedar::proc::ConstElementPtr getElement(const cedar::proc::NetworkPath& name) const;

  /*!@brief Connects data slots of two cedar::proc::Connectable elements.
   *
   * @param source Identifier of the source data slot. This must be of the form "elementName.outputSlotName".
   * @param target Identifier of the target data slot. This must be of the form "elementName.inputSlotName".
   */
  void connectSlots(const std::string& source, const std::string& target);

  /*!@brief Connects data slots of two cedar::proc::Connectable elements.
   *
   * @param source Source data slot.
   * @param target Target data slot.
   */
  void connectSlots(cedar::proc::ConstDataSlotPtr source, cedar::proc::ConstDataSlotPtr target);

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
  bool isConnected(const std::string& source, const std::string& target) const;

  /*!@brief Returns, whether target receives trigger signals from the source trigger.
   */
  bool isConnected(cedar::proc::TriggerPtr source, cedar::proc::TriggerablePtr target) const;

  /*!@brief Deletes the connection between the data slots.
   *
   * @param source Identifier of the source data slot. This must be of the form "elementName.outputSlotName".
   * @param target Identifier of the target data slot. This must be of the form "elementName.inputSlotName".
   */
  void disconnectSlots(const std::string& source, const std::string& target);

  /*!@brief Deletes the connection between the data slots.
   *
   * @param sourceSlot The source slot.
   * @param targetSlot The target slot.
   */
  void disconnectSlots(cedar::proc::ConstDataSlotPtr sourceSlot, cedar::proc::ConstDataSlotPtr targetSlot);

  /*!@brief Deletes the connections in the list.
   */
  void disconnectSlots(const std::vector<cedar::proc::DataConnectionPtr>& connections);

  /*!@brief Deletes all connections to the given data slot.
   *
   * @param role Role of the slot  (input, output etc.)
   * @param connectable The slot's parent.
   * @param slot Identifier of the data slot.
   */
  void disconnectSlot(cedar::proc::DataRole::Id role, cedar::proc::ConnectablePtr connectable, const std::string& slot);

  /*!@brief Deletes all connections from a given data slot.
   * @param connectable The slot's parent.
   * @param slot Identifier of the data slot.
   *
   * @todo check for duplicate functions
   */
  void disconnectOutputSlot(cedar::proc::ConnectablePtr connectable, const std::string& slot);

  /*!@brief Deletes all connections from a given data slot.
   * @param connectable The slot's parent.
   * @param slot Identifier of the data slot.
   */
  //void disconnectSlot(cedar::proc::ConnectablePtr connectable, const std::string& slot);

  /*!@brief Deletes all connections to the given data slot.
   * @param connectable The slot's parent.
   * @param slot Identifier of the data slot.
   */
  void disconnectInputSlot(cedar::proc::ConnectablePtr connectable, const std::string& slot);

  /*!@brief Deletes the connection between source and target.
   */
  void disconnectTrigger(cedar::proc::TriggerPtr source, cedar::proc::TriggerablePtr target);

  /*!@brief Writes all the connections originating from a source connectable into a vector.
   *
   * @param source         The connectable source.
   * @param sourceDataName The name of the slot from which to look list outgoing connections.
   * @param connections    Vector into which the connections are written.
   * @todo there is a const version of this function with duplicated code - not trivial since vector is const as well
   */
  void getDataConnections(
                           cedar::proc::ConnectablePtr source,
                           const std::string& sourceDataName,
                           std::vector<cedar::proc::DataConnectionPtr>& connections
                         );
                         
  void getDataConnections(
                           cedar::proc::ConstConnectablePtr source,
                           const std::string& sourceDataName,
                           std::vector<cedar::proc::ConstDataConnectionPtr>& connections
                         ) const;

  /*!@brief Writes all the connections originating from a source connectable into a vector.
   *
   * @param source             The connectable source.
   * @param sourceDataSlotName The name of the slot from which to look list outgoing connections.
   * @param connections        Vector into which the connections are written.
   */
  void getDataConnectionsFrom(
                           cedar::proc::ConnectablePtr source,
                           const std::string& sourceDataSlotName,
                           std::vector<cedar::proc::DataConnectionPtr>& connections
                         );

  /*!@brief Writes all the connections ending at a target connectable into a vector.
   *
   * @param source             The connectable target.
   * @param sourceDataSlotName The name of the slot at which connections end.
   * @param connections        Vector into which the connections are written.
   */
  void getDataConnectionsTo(
                             cedar::proc::ConnectablePtr target,
                             const std::string& targetDataSlotName,
                             std::vector<cedar::proc::DataConnectionPtr>& connections
                           );

  /*!@brief access the vector of data connections
   */
  const cedar::proc::Group::DataConnectionVector& getDataConnections() const;

  /*!@brief Returns a const reference to the map of names to elements stored in the group.
   */
  const ElementMap& getElements() const;

  //!@deprecated Use getElements instead.
  CEDAR_DECLARE_DEPRECATED(const ElementMap& elements() const)
  {
    return this->getElements();
  }

  /*!@brief Updates the name stored for the object.
   */
  void updateObjectName(cedar::proc::Element* object);

  /*!@brief Sends a reset command to all connectable elements in the group.
   */
  void reset();

  /*!@brief Find the complete path of an element, if it exists in the tree structure
   * @returns returns the dot-separated path to the element, or empty string if element is not found in tree
   */
  std::string findPath(cedar::proc::ConstElementPtr findMe) const;

  /*!@brief This method lists all groups that are children of this group.
   */
  void listSubgroups(std::set<cedar::proc::ConstGroupPtr>& subgroups) const;

  /*!@brief Returns a unique identifier containing the given string.
   *
   *        If there is no element by the given name, then the identifier is returned, otherwise, it is extended by
   *        appending a number.
   */
  std::string getUniqueIdentifier(const std::string& identifier) const;

  //!@brief Checks whether a name exists in the group.
  bool nameExists(const std::string& name) const;

  //!@brief returns the last ui node that was read
  cedar::aux::ConfigurationNode& getLastReadConfiguration()
  {
    return this->mLastReadConfiguration;
  }

  /*!@brief Remove all connections that connect up to a specified slot */
//  void removeAllConnectionsFromSlot(cedar::proc::ConstDataSlotPtr slot);

  //!@brief Starts all triggers in this group (that haven't been started yet).
  void startTriggers(bool wait = false);

  //!@brief Stops all running triggers in this group.
  void stopTriggers(bool wait = false);

  /*!@brief   Single-steps all triggers in this group.
   *
   *          The step time is in this case automatically determined by using the smallest step time of all triggers in
   *          the group.
   *
   * @see     stepTriggers(double).
   */
  void stepTriggers();

  /*!@brief   Single-steps all triggers in this group with the given time step.
   *
   * @see     stepTriggers(double).
   */
  void stepTriggers(cedar::unit::Time stepTime);

  //! Returns a list of issues in the group.
  std::vector<cedar::proc::ConsistencyIssuePtr> checkConsistency() const;

  //! Returns a list of all the looped triggers in this group.
  std::vector<cedar::proc::LoopedTriggerPtr> listLoopedTriggers() const;

  //! Reads the meta information from the given file and extracts the plugins required by the architecture.
  static std::set<std::string> getRequiredPlugins(const std::string& architectureFile);

  void onTrigger
       (
         cedar::proc::ArgumentsPtr args = cedar::proc::ArgumentsPtr(),
         cedar::proc::TriggerPtr = cedar::proc::TriggerPtr()
       );

  /*!@brief The wait method.
   */
  void waitForProcessing();

  const ConnectorMap& getConnectorMap();

  bool hasConnector(const std::string& name) const;

  std::string getNewConnectorName(bool inputConnector) const;

  bool contains(cedar::proc::ConstElementPtr element) const;

  bool isRoot() const;

  cedar::proc::ElementPtr importGroupFromFile(const std::string& groupName, const std::string& fileName);

  cedar::proc::ElementPtr importStepFromFile(const std::string& stepName, const std::string& fileName);

  //! Adds a parameter link to the list of links in this group. Does NOT link the parameters.
  void addParameterLink
  (
    cedar::proc::ElementPtr sourceElement,
    cedar::proc::ElementPtr targetElement,
    cedar::aux::ParameterLinkPtr link
  );
  
  //! Removes the parameter link and from this group.
  void removeParameterLink(cedar::aux::ParameterLinkPtr link);

  //!@brief
  void setIsLooped(bool looped);

  // make sure to create a list of all looped triggerables on start
  void onStart();

  // onStop enables the isLooped parameter
  void onStop();

  //!@brief Returns whether this step should automatically be connected to done triggers when data is connected.
  bool isLooped() const;

  //!@brief this function removes all unused connectors, i.e., connectors that have zero incoming or outgoing connections
  void pruneUnusedConnectors();

  cedar::aux::ParameterPtr addCustomParameter(const std::string& type, const std::string& name);

  void removeCustomParameter(const std::string& name);

  /*! Returns the list of custom parameters added to this group. Custom parameters are those that have been added by the
   *  user.
   */
  inline std::vector<cedar::aux::ParameterPtr> getCustomParameters() const
  {
    return this->mCustomParameters;
  }
  
  /*! Returns a list of all the parameter links stored in this group.
   */
  inline const std::vector<ParameterLinkInfo>& getParameterLinks() const
  {
    return this->mParameterLinks;
  }

  /*! Returns a list of all the parameter links stored in this group.
   */
  inline std::vector<ParameterLinkInfo>& getParameterLinks()
  {
    return this->mParameterLinks;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief Redetermines the validity for an input slot.
   *
   * @param slot The slot to revalidate.
   */
  void revalidateInputSlot(const std::string& slot);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Reads the group from a configuration node and writes all exceptions into the given vector.
   */
  void readConfiguration
  (
    const cedar::aux::ConfigurationNode& root,
    std::vector<std::string>& exceptions
  );

  /*!@brief remove a DataConnection and do a check, if any TriggerConnections must be deleted as well
   * @returns return the next iterator
   */
  cedar::proc::Group::DataConnectionVector::iterator removeDataConnection
                                                       (
                                                         cedar::proc::Group::DataConnectionVector::iterator it
                                                       );

  //!@brief revalidates all outgoing connections of a slot
  void revalidateConnections(const std::string& sender);

  void processConnectors();

  void removeAllConnectors();

  //!@brief Reacts to a change in the input connection.
  void inputConnectionChanged(const std::string& inputName);

  std::vector<cedar::proc::DataSlotPtr> getRealTargets
                                        (
                                          cedar::proc::DataSlotPtr slot,
                                          cedar::proc::ConstGroupPtr targetGroup
                                        );

  std::vector<cedar::proc::DataSlotPtr> getRealSources
                                        (
                                          cedar::proc::DataSlotPtr slot,
                                          cedar::proc::ConstGroupPtr targetGroup
                                        );

  static void connectAcrossGroups(cedar::proc::DataSlotPtr source, cedar::proc::DataSlotPtr target);
  static bool disconnectAcrossGroups(cedar::proc::DataSlotPtr source, cedar::proc::DataSlotPtr target);

private slots:
  //!@brief Takes care of updating the group's name in the parent's map.
  void onNameChanged();
  //--------------------------------------------------------------------------------------------------------------------
  // signals and slots
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief a boost signal that is emitted if a change in slot takes place
  CEDAR_DECLARE_SIGNAL(SlotChanged, void());

public:
  //!@brief a boost signal that is emitted if a trigger connection changes (added/removed)
  CEDAR_DECLARE_SIGNAL(TriggerConnectionChanged, void (cedar::proc::TriggerPtr, cedar::proc::TriggerablePtr, bool));

public:
  //!@brief a boost signal that is emitted if a trigger connection changes (added/removed)
  CEDAR_DECLARE_SIGNAL(DataConnectionChanged, void (cedar::proc::ConstDataSlotPtr, cedar::proc::ConstDataSlotPtr, cedar::proc::Group::ConnectionChange));

public:
  //!@brief a boost signal that is emitted if a trigger connection changes (added/removed)
  CEDAR_DECLARE_SIGNAL(NewElementAdded, void (cedar::proc::ElementPtr));

public:
  //!@brief a boost signal that is emitted if a trigger connection changes (added/removed)
  CEDAR_DECLARE_SIGNAL(ElementRemoved, void (cedar::proc::ConstElementPtr));

public:
  //!@brief Signal that is emitted whenever a custom parameter is added.
  CEDAR_DECLARE_SIGNAL(CustomParameterAdded, void (cedar::aux::ParameterPtr));

public:
  //!@brief Signal that is emitted whenever a custom parameter is removed.
  CEDAR_DECLARE_SIGNAL(CustomParameterRemoved, void (cedar::aux::ParameterPtr));

public:
  //!@brief Signal that is emitted whenever a custom parameter is removed.
  CEDAR_DECLARE_SIGNAL(ParameterLinkAdded, void (const ParameterLinkInfo&));

public:
  //!@brief Signal that is emitted whenever a custom parameter is removed.
  CEDAR_DECLARE_SIGNAL(ParameterLinkRemoved, void (cedar::aux::ParameterLinkPtr));

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! Map associating element names to elements.
  ElementMap mElements;

  //! List of data connections in the group.
  DataConnectionVector mDataConnections;

  //! List of trigger connections in the group.
  TriggerConnectionVector mTriggerConnections;

  cedar::aux::ConfigurationNode mLastReadConfiguration;

  //!@brief connection to state changed signal of step
  std::map<std::string, boost::signals2::connection> mRevalidateConnections;

  //! List of all the parameter links in this group.
  std::vector<ParameterLinkInfo> mParameterLinks;

  //! Map containing every looped thread
  TriggerableVector mLoopedTriggerables;

  //! List of all the custom parameters that were added to this group.
  std::vector<cedar::aux::ParameterPtr> mCustomParameters;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
   ConnectorMapParameterPtr _mConnectors;
   cedar::aux::BoolParameterPtr _mIsLooped;

}; // class cedar::proc::Group

#endif // CEDAR_PROC_GROUP_H
