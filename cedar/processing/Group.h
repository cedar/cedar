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
#include "cedar/processing/GroupPath.h"
#include "cedar/processing/Triggerable.h"
#include "cedar/auxiliaries/MapParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/Path.h"
#include "cedar/auxiliaries/boostSignalsHelper.h"
#include "cedar/units/Time.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/StringVectorParameter.fwd.h"
#include "cedar/auxiliaries/ParameterLink.fwd.h"
#include "cedar/processing/LoopedTrigger.fwd.h"
#include "cedar/processing/Group.fwd.h"
#include "cedar/processing/CppScript.fwd.h"
#include "cedar/processing/Trigger.fwd.h"
#include "cedar/processing/TriggerConnection.fwd.h"
#include "cedar/processing/GroupFileFormatV1.fwd.h"
#include "cedar/processing/consistency/ConsistencyIssue.fwd.h"
#include "cedar/processing/sinks/GroupSink.fwd.h"

// SYSTEM INCLUDES
#include <QThread>
#include <QMetaType>
#include <vector>
#ifndef Q_MOC_RUN
  #include <boost/signals2/signal.hpp>
  #include <boost/signals2/connection.hpp>
#endif
#include <map>
#include <set>
#include <list>
#include <string>
#include <functional>

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
  /*!@brief A structure that holds information about parameter links.
   *
   * This structure is responsible for storing the source- and target element of parameter links, as well as the link
   * between them.
   *
   * Its main purpose is to remember what kinds of element parameters are linked together for later storing these links.
   */
  struct ParameterLinkInfo
  {
    //! returns the path to the source element of this parameter link
    std::string getSourceElementPath() const;

    //! returns the path to the target element of this parameter link
    std::string getTargetElementPath() const;

    //! returns the path to the source parameter of this parameter link
    std::string getSourceParameterPath() const;

    //! returns the path to the target parameter of this parameter link
    std::string getTargetParameterPath() const;

    //! the link parameter
    cedar::aux::ParameterLinkPtr mParameterLink;

    //! the source element
    cedar::proc::ElementPtr mSourceElement;

    //! the target element
    cedar::proc::ElementPtr mTargetElement;

    //! the group
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
  //! Signals when a step name changes.
  void stepNameChanged(const std::string& from, const std::string& to);

  //! Emitted whenever a trigger in this group or any of its subgroups is started.
  void triggerStarted();

  //! Emitted whenever all trigger in this group and its subgroups are stopped.
  void allTriggersStopped();

  //! Sent when the number of triggerables in warning/error states is changed
  void triggerableStateCountsChanged();

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

  //! A map of external connectors and their roles (input/output)
  typedef std::map<std::string, bool> ConnectorMap;

  //! A map parameter of external connectors and their roles (input/output)
  typedef cedar::aux::MapParameter<bool> ConnectorMapParameter;

  //!@cond SKIPPED_DOCUMENTATION
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(ConnectorMapParameter);
  //!@endcond


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


  void readData(const cedar::aux::ConfigurationNode& root);

  /*!@brief Writes the group to a configuration node.
   */
  void writeConfiguration(cedar::aux::ConfigurationNode& root) const;

  /*!@brief Writes data marked as serializable to the given configuration node.
   */
  void writeData(cedar::aux::ConfigurationNode& root) const;

  /*!@brief Removes an element from the group.
   *
   * @remark Before calling this function, you should remove all connections to the element.
   */
  void remove(cedar::proc::ConstElementPtr element, bool destructing = false);

  /*!@brief calls remove() for every element of the group
   */
  void removeAll(bool destructing = false);

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

  //!@brief adds an external connector of the specified type to the group
  void addConnector(const std::string& name, bool input);

  //!@brief renames an external connector of the specified type
  void renameConnector(const std::string& oldName, const std::string& newName, bool input);

  //!@brief checks if a connector can be renamed
  bool canRenameConnector(const std::string& oldName, const std::string& newName, bool input, std::string& error) const;

  //!@brief removes an external connector of the specified type from the group
  void removeConnector(const std::string& name, bool input);

  /*!@brief Duplicates an existing element.
   *
   * @param elementName Identifier of the existing element.
   * @param newName Name to be given to the new element.
   * @returns The new name of the element.
   */
  std::string duplicate(const std::string& elementName, const std::string& newName = "");

  /*!@brief Returns the element with the given name as a pointer of the specified type.
   */
  template <class T>
  boost::shared_ptr<T> getElement(const cedar::proc::GroupPath& name)
  {
    return boost::dynamic_pointer_cast<T>(this->getElement(name));
  }

  /*!@brief Returns the element with the given name as a const pointer of the specified type.
   */
  template <class T>
  boost::shared_ptr<const T> getElement(const cedar::proc::GroupPath& name) const
  {
    return boost::dynamic_pointer_cast<const T>(this->getElement(name));
  }

  /*!@brief  Returns a pointer to the element with the given name. Uses the const function getElement.
   *
   * @throws cedar::aux::InvalidNameException if no element is found with the given name.
   */
  cedar::proc::ElementPtr getElement(const cedar::proc::GroupPath& name);

  /*!@brief  Returns a const pointer to the element with the given name.
   *
   * @throws cedar::aux::InvalidNameException if no element is found with the given name.
   */
  cedar::proc::ConstElementPtr getElement(const cedar::proc::GroupPath& name) const;

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
  void connectSlots(cedar::proc::OwnedDataPtr source, cedar::proc::ExternalDataPtr target);

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
  void disconnectSlots(cedar::proc::ConstOwnedDataPtr sourceSlot, cedar::proc::ConstExternalDataPtr targetSlot);

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
   */
  void disconnectOutputSlot(cedar::proc::ConnectablePtr connectable, const std::string& slot);

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

  //!@brief const version of getDataConnections
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
   * @param target             The connectable target.
   * @param targetDataSlotName The name of the slot at which connections end.
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

  //! Recursively lists all elements in the group and all its subgroups.
  std::vector<cedar::proc::GroupPath> listAllElementPaths(const cedar::proc::GroupPath& base_path = cedar::proc::GroupPath()) const;

  /*! Recursively lists elements in the group and all its subgroups if they fit the function fit.
   */
  std::vector<cedar::proc::GroupPath> listElementPaths(std::function<bool(cedar::proc::ConstElementPtr)> fit, const cedar::proc::GroupPath& base_path = cedar::proc::GroupPath()) const;

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

  /*!@brief Find the complete path of an element, if it exists in the tree structure
   * @returns returns the dot-separated path to the element, or empty string if element is not found in tree
   */
  std::string findPath(const cedar::proc::Element* pFindMe) const;

  /*!@brief Finds all elements of the given type.
   *
   * @param findRecursive Whether child groups of this group should be searched as well.
   */
  template <typename T>
  std::set<boost::shared_ptr<T> > findAll(bool findRecursive = false) const
  {
    std::set<boost::shared_ptr<T> > elements;

    for (auto name_element_pair : this->getElements())
    {
      cedar::proc::ElementPtr element = name_element_pair.second;
      if (auto t_ptr = boost::dynamic_pointer_cast<T>(element))
      {
        elements.insert(t_ptr);
      }

      if (findRecursive)
      {
        if (auto group_ptr = boost::dynamic_pointer_cast<cedar::proc::Group>(element))
        {
          auto sub_items = group_ptr->findAll<T>(findRecursive);
          elements.insert(sub_items.begin(), sub_items.end());
        }
      }
    }

    return elements;
  }

  /*!@brief This method lists all groups that are children of this group (const).
   */
  void listSubgroups(std::set<cedar::proc::ConstGroupPtr>& subgroups) const;

  /*!@brief This method lists all groups that are children of this group.
   */
  void listSubgroups(std::set<cedar::proc::GroupPtr>& subgroups);

  /*!@brief Returns a unique identifier containing the given string.
   *
   *        If there is no element by the given name, then the identifier is returned, otherwise, it is extended by
   *        appending a number.
   */
  std::string getUniqueIdentifier(const std::string& identifier) const;

  //!@brief Checks whether a name exists in the group.
  bool nameExists(const cedar::proc::GroupPath& name) const;

  //!@brief Checks whether a name exists in this group or any of its children or parents.
  bool nameExistsInAnyGroup(const cedar::proc::GroupPath& name) const;

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
  std::vector<cedar::proc::LoopedTriggerPtr> listLoopedTriggers(bool recursive = false) const;

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

  //!@brief returns the map of all external connectors
  const ConnectorMap& getConnectorMap();

  //!@brief checks if a connector name already exists
  bool hasConnector(const std::string& name) const;

  //!@brief returns a generic, unused connector name
  std::string getNewConnectorName(bool inputConnector) const;

  //!@brief checks if group itself or any child group contains the given element
  bool contains(cedar::proc::ConstElementPtr element) const;

  //!@brief returns if this is the root group
  bool isRoot() const;

  //!@brief imports a given group from a given configuration file
  cedar::proc::ElementPtr importGroupFromFile(const std::string& groupName, const cedar::aux::Path& fileName);

  //!@brief imports a given group from a given configuration file and links it to said file
  cedar::proc::ElementPtr createLinkedGroup(const std::string& groupName, const std::string& fileName);

  //!@brief imports a given group template from a given configuration file and links it to said file
  cedar::proc::ElementPtr createLinkedTemplate(const std::string& groupName, const std::string& fileName, const std::string& templateName);

  //!@brief read linked content from a group
  void readLinkedGroup(const std::string& groupName, const cedar::aux::Path& fileName);

  //!@brief read linked content from a group
  void readLinkedTemplate(const std::string& templateName);

  //!@brief imports a given step from a given configuration file
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

  //!@brief set loopiness of this group
  void setIsLooped(bool looped);

  // make sure to create a list of all looped triggerables on start
  void onStart();

  // onStop enables the isLooped parameter
  void onStop();

  //!@brief this function removes all unused connectors, i.e., connectors that have zero incoming or outgoing connections
  void pruneUnusedConnectors();

  //!@brief adds a custom parameter for this group
  cedar::aux::ParameterPtr addCustomParameter(const std::string& type, const std::string& name);

  //!@brief removes a custom parameter from this group
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

  /*! @brief Returns a list of all steps that are in an invalid state.
   */
  std::vector<std::string> listInvalidSteps() const;

  /*! Returns whether or not this group is linked, i.e., read from a file.
   */
  inline bool isLinked() const
  {
    return !this->mLinkedGroupFile.empty() && !this->mLinkedGroupName.empty();
  }

  //!@brief finds all elements in this group and child groups that match the given name
  std::vector<cedar::proc::ConstElementPtr> findElementsAcrossGroupsFullName(const std::string& fullName) const;

  //!@brief finds all elements in this group and child groups that partially match the given string
  std::vector<cedar::proc::ConstElementPtr> findElementsAcrossGroupsContainsString(const std::string& string) const;

  //! If set to true, trigger chains will not be updated.
  bool holdTriggerChainUpdates() const;

  //! If set to true, trigger chains will not be updated.
  void setHoldTriggerChainUpdates(bool hold);

  //! Updates the trigger chains of all steps.
  void updateTriggerChains(std::set<cedar::proc::Trigger*>& visited);

  /*! This function lists the required plugins for all the elements in this group and any of its subgroups.
   */
  std::set<std::string> listRequiredPlugins() const;

  /*! Creates a new cedar::proc::CppScript of the given type and adds it to the group.
   *
   *  @remarks the name for the script will be determined automatically.
   */
  void createScript(const std::string& type);

  //! Adds the given script to the group.
  void addScript(cedar::proc::CppScriptPtr script);

  //! Retrieves the script with the given name.
  cedar::proc::CppScriptPtr getScript(const std::string& name) const;

  //! Removes the script with the given name.
  void removeScript(const std::string& name);

  //! Lists all scripts in this group.
  std::set<cedar::proc::CppScriptPtr> getScripts() const;

  //! Lists all scripts in this group in an ordered manner.
  std::vector<cedar::proc::CppScriptPtr> getOrderedScripts() const;

  //! Checks if a script with the given name exists in this group.
  bool checkScriptNameExists(const std::string& name) const;

  //! Sets the time factor to be used for simulating this group. Only applied by the root group.
  void setTimeFactor(double factor);

  //! Returns the time factor set for this architecture.
  double getTimeFactor() const;

  //! Applies the group's time factor, i.e., sets it at the cedar::aux::SettingsSingleton.
  void applyTimeFactor();

  //! Returns the number of triggerables in this group that are in a warning state
  unsigned int getTriggerablesInWarningStateCount() const;

  //! Returns the number of triggerables in this group that are in a warning state
  unsigned int getTriggerablesInErrorStateCount() const;

  //!@brief connects two slots across groups, allocating connectors if necessary
  static void connectAcrossGroups(cedar::proc::OwnedDataPtr source, cedar::proc::ExternalDataPtr target);

  //!@brief disconnects two slots across groups, removing/merging connectors if necessary
  static bool disconnectAcrossGroups(cedar::proc::OwnedDataPtr source, cedar::proc::ExternalDataPtr target);

  //! Returns if this group is marked as being recorded.
  bool isRecorded() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief Redetermines the validity for an input slot.
   *
   * @param slot The slot to revalidate.
   */
  void revalidateInputSlot(const std::string& slot);

  signals:
    //!@brief notify others of loopiness change
    void loopedChanged();

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

  //!@brief adds all connectors from the list of stored connectors
  void processConnectors();

  //!@brief removes all connectors from this group
  void removeAllConnectors();

  //!@brief Reacts to a change in the input connection.
  void inputConnectionChanged(const std::string& inputName);

  //!@brief searches for non-group targets of a data slot, traversing all outgoing connections across groups
  std::vector<cedar::proc::ExternalDataPtr> getRealTargets
                                            (
                                              cedar::proc::DataSlotPtr slot,
                                              cedar::proc::ConstGroupPtr targetGroup
                                            );

  //!@brief searches for non-group sources of a data slot, traversing all incoming connections across groups
  std::vector<cedar::proc::OwnedDataPtr> getRealSources
                                         (
                                           cedar::proc::DataSlotPtr slot,
                                           cedar::proc::ConstGroupPtr targetGroup
                                         );

  //!@brief searches for elements specified by a matcher function
  std::vector<cedar::proc::ConstElementPtr> findElementsAcrossGroups(boost::function<bool(cedar::proc::ConstElementPtr)> matcher) const;

  /*!@brief A function that adds a connector even if the name alread exists. Used during loading
   */
  void addConnectorInternal(const std::string& name, bool input);

  //! Finds an identifier for which the @em checker function returns false.
  static std::string findNewIdentifier(const std::string& basis, boost::function<bool(const std::string&)> checker);

  //! if the parent group changes (i.e., this group looses its 'rootness'), the default trigger is removed if it exists
  void onParentGroupChanged();

  void disconnectTriggerInternal(cedar::proc::TriggerPtr source, cedar::proc::TriggerablePtr target);

  void outputConnectionRemoved(cedar::proc::DataSlotPtr slot);

  void triggerableStateChanged(cedar::proc::TriggerableWeakPtr triggerable);

  void uncountTriggerableState(cedar::proc::ConstTriggerablePtr triggerable);

  void registerLoopedTriggerable(cedar::proc::TriggerablePtr triggerable);

  void unregisterLoopedTriggerable(cedar::proc::TriggerablePtr triggerable);

private slots:
  //!@brief Takes care of updating the group's name in the parent's map.
  void onNameChanged();

  //!@brief Takes care of updating the list of looped triggerables if any of them change their state.
  void onLoopedChanged();

  void triggerStopped();

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

public:
  //!@brief Signal that is emitted whenever a custom parameter is removed.
  CEDAR_DECLARE_SIGNAL(LinkedChanged, void (bool));

public:
  //!@brief Signal that is emitted whenever a custom parameter is removed.
  CEDAR_DECLARE_SIGNAL(LastReadConfigurationChanged, void ());

public:
  //!@brief Signal that is emitted whenever a custom parameter is removed.
  CEDAR_DECLARE_SIGNAL(ScriptAdded, void (const std::string&));

public:
  //!@brief Signal that is emitted whenever a custom parameter is removed.
  CEDAR_DECLARE_SIGNAL(ScriptRemoved, void (const std::string&));

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
  cedar::aux::LockableMember<TriggerableVector> mLoopedTriggerables;

  //! List of all the custom parameters that were added to this group.
  std::vector<cedar::aux::ParameterPtr> mCustomParameters;

  //! If non-empty, specifies which file the group was imported from.
  cedar::aux::Path mLinkedGroupFile;

  //! If non-empty, the name of the group that was imported from a file.
  std::string mLinkedGroupName;

  //! If non-empty, the name of the group template that was used to create this group.
  std::string mLinkedTemplateName;

  //! Flag if trigger chain updates should be executed (during connecting/loading)
  bool mHoldTriggerChainUpdates;

  //! Map of scripts present in this architecture
  cedar::aux::LockableMember<std::set<cedar::proc::CppScriptPtr>> mScripts;

  //! a connection to the groupChanged signal of element
  boost::signals2::scoped_connection mParentGroupChangedConnection;

  std::map<cedar::proc::ConstTriggerablePtr, boost::shared_ptr<boost::signals2::scoped_connection> > mTriggerableStateChangedConnections;

  std::map<cedar::proc::ConstTriggerablePtr, cedar::proc::Triggerable::State> mPreviousTriggerableStates;

  cedar::aux::LockableMember<unsigned int> mTriggerablesInWarningStates;

  cedar::aux::LockableMember<unsigned int> mTriggerablesInErrorStates;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! map of all external connectors of this group
  ConnectorMapParameterPtr _mConnectors;

  /*! this parameter is kept to maintain compatibility with older cedar versions - it is no longer needed
   *  because groups now automatically determine if they are looped
   */
  cedar::aux::BoolParameterPtr _mIsLooped;

  cedar::aux::DoubleParameterPtr _mTimeFactor;
}; // class cedar::proc::Group

Q_DECLARE_METATYPE(cedar::proc::Group::ConnectionChange)

#endif // CEDAR_PROC_GROUP_H
