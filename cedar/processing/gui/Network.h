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

#ifndef CEDAR_PROC_GUI_NETWORK_H
#define CEDAR_PROC_GUI_NETWORK_H

// CEDAR INCLUDES
#include "cedar/processing/gui/GraphicsBase.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/Network.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/DataSlotItem.fwd.h"
#include "cedar/processing/gui/Network.fwd.h"

// SYSTEM INCLUDES
#include <QObject>
#ifndef Q_MOC_RUN
  #include <boost/signals2/signal.hpp>
  #include <boost/signals2/connection.hpp>
#endif // Q_MOC_RUN
#include <map>
#include <vector>
#include <string>
#include <list>

/*!@brief The representation of a cedar::proc::Network in a cedar::proc::gui::Scene.
 *
 *        This class takes care of loading cedar::proc::Networks in a manner that allows them to be added into
 *        cedar::proc::gui::Scenes as either the root network or a subnetwork.
 */
class cedar::proc::gui::Network : public QObject, public cedar::proc::gui::GraphicsBase
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief mapping from data slot names to their graphical representation
  typedef std::map<std::string, cedar::proc::gui::DataSlotItem*> DataSlotNameMap;
  //!@brief mapping from data role id to a map of all data slots fitting this id
  typedef std::map<cedar::proc::DataRole::Id, DataSlotNameMap> DataSlotMap;
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Network
  (
    QMainWindow *pMainWindow,
    cedar::proc::gui::Scene* scene,
    qreal width = 0,
    qreal height = 0,
    cedar::proc::NetworkPtr network = cedar::proc::NetworkPtr()
  );

  //!@brief Destructor
  ~Network();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief write network to file
  void write();
  //!@brief write network to file given by destination
  void write(const std::string& destination);
  //!@brief read network from given file
  void read(const std::string& source);

  /*!@brief access the underlying cedar::proc::Network
   */
  cedar::proc::NetworkPtr getNetwork();

  /*!@brief access the underlying cedar::proc::Network
   */
  CEDAR_DECLARE_DEPRECATED(inline cedar::proc::NetworkPtr network())
  {
    return this->getNetwork();
  }

  //!@brief add all elements contained in this network to the scene
  void addElementsToScene();

  //!@brief get the current file, to which the network configuration can be saved
  const std::string& getFileName() const;

  //!@brief Resizes the network to fit all its contents.
  void fitToContents();

  //!@brief Adds an element to the network.
  void addElement(cedar::proc::gui::GraphicsBase *pElement);

  //!@brief Adds a list of elements to the network efficiently.
  void addElements(const std::list<QGraphicsItem*>& elements);

  //! Duplicates an element and places it at the given position.
  void duplicate(const QPointF& scenePos, const std::string& elementName, const std::string& newName = "");

  //!@brief Sets the scene containing this item.
  void setScene(cedar::proc::gui::Scene* pScene);

  //!@brief reads a configuration from a node
  void readConfiguration(const cedar::aux::ConfigurationNode& node);

  //!@brief saves a configuration to a node
  void writeConfiguration(cedar::aux::ConfigurationNode& root) const;

  //! Returns the slot item of the given role and name.
  cedar::proc::gui::DataSlotItem* getSlotItem(cedar::proc::DataRole::Id role, const std::string& name);

  //!@brief returns a map of all data slots of the same id
  cedar::proc::gui::Network::DataSlotNameMap& getSlotItems(cedar::proc::DataRole::Id role);

  void disconnect();

  //! deals with changes to the network gui item
  QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant & value);

  //! deals with a mouse release event
  bool sceneEventFilter(QGraphicsItem* pWatched, QEvent* pEvent);

  //! get the scene in which this network is embedded
  cedar::proc::gui::Scene* getScene()
  {
    return this->mpScene;
  }

  /*!@brief Sets the ui configuration for the element when it is added to the network.
   */
  inline void setNextElementUiConfiguration
  (
    cedar::proc::ElementPtr element, const cedar::aux::ConfigurationNode& uiDescription
  )
  {
    this->mNextElementUiConfigurations[element.get()] = uiDescription;
  }

  //! Sets the smart connection mode for all elements in this network.
  void toggleSmartConnectionMode(bool smart)
  {
    this->_mSmartMode->setValue(smart);
  }

  //! Returns whether smart connection mode is used for all elements in this network.
  bool getSmartConnection() const
  {
    return this->_mSmartMode->getValue();
  }

  //! creates plot group of provided name containing all currently opened plots
  void addPlotGroup(std::string plotGroupName);

  //! removes plot group of given name
  void removePlotGroup(std::string plotGroupName);

  //! renames plot group of given name (from) to given name (to)
  void renamePlotGroup(std::string from, std::string to);
  
  //! returns the name of every plot group of this network
  std::list<std::string> getPlotGroupNames();

  //! opens the given plot group
  void displayPlotGroup(std::string plotGroupName);

  //! search and replace every occurance of 'from' with 'to' in the plot groups node
  void changeStepName(const std::string& from, const std::string& to);

public slots:
  void stepRecordStateChanged();

  void handleStepNameChanged(const std::string& from, const std::string& to);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  //!@brief write scene to a node
  void writeScene(cedar::aux::ConfigurationNode& root, cedar::aux::ConfigurationNode& scene);

  //!@brief Determines whether the network is the root network.
  bool isRootNetwork();

  void checkSlots();

  void checkDataItems();

  //!@brief Transforms the coordinates of a newly added child into the network's coordinate system.
  void transformChildCoordinates(cedar::proc::gui::GraphicsBase* pItem);

  //!@brief a function that translates a boost signal to check a data connection into a Qt signal
  void checkDataConnection
       (
         cedar::proc::ConstDataSlotPtr source,
         cedar::proc::ConstDataSlotPtr target,
         cedar::proc::Network::ConnectionChange change
       );

  void checkTriggerConnection(cedar::proc::TriggerPtr, cedar::proc::TriggerablePtr, bool added);

  void processElementAddedSignal(cedar::proc::ElementPtr);

  void processElementRemovedSignal(cedar::proc::ConstElementPtr);

  void readPlotList(const std::string& plotGroupName, const cedar::aux::ConfigurationNode& node);

  void writeOpenPlotsTo(cedar::aux::ConfigurationNode& node) const;

  void readStickyNotes(cedar::aux::ConfigurationNode& node);

signals:
  //!@brief signal that is emitted when a boost signal is received
  void signalDataConnectionChange(QString, QString, QString, QString, cedar::proc::Network::ConnectionChange);

private slots:
  //!@brief Updates the label of the network.
  void networkNameChanged();

  void toggleSmartConnectionMode();

  //!@brief handle an internal signal to create or remove gui connections
  void dataConnectionChanged
       (
         QString sourceName,
         QString sourceSlot,
         QString targetName,
         QString targetSlot,
         cedar::proc::Network::ConnectionChange change
       );

  void removeElementFromPlotGroup(const std::string& plotGroupname, const std::string& elementName);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief represented network
  cedar::proc::NetworkPtr mNetwork;

  //!@brief a scene, which displays the elements contained in this network
  cedar::proc::gui::Scene* mpScene;

  //!@brief a filename from which to load a network configuration, or to which to save a configuration
  std::string mFileName;

  //!@brief a main window
  QMainWindow *mpMainWindow;

  //!@brief a map of all data slots of the current step
  DataSlotMap mSlotMap;

  //!@brief a vector of steps, which contains all steps that should be added to the scene after reading a configuration
  std::vector<cedar::proc::gui::StepItem*> mpStepsToAdd;

  //!@brief a vector of triggers, which contains all steps that should be added to the scene
  //        after reading a configuration
  std::vector<cedar::proc::gui::TriggerItem*> mpTriggersToAdd;

  //!@brief a vector of steps, which contains all steps that should be added to the scene after reading a configuration
  std::vector<cedar::proc::gui::Network*> mpNetworksToAdd;

  //! Connection to Network's slot changed signal.
  boost::signals2::connection mSlotConnection;
  boost::signals2::connection mNewElementAddedConnection;
  boost::signals2::connection mElementRemovedConnection;
  boost::signals2::connection mTriggerConnectionChangedConnection;
  boost::signals2::connection mDataConnectionChangedConnection;

  //! Fit to contents-calls are temporarily disabled if this is set to true.
  bool mHoldFitToContents;

  //! Text item used for displaying the name of the network.
  QGraphicsTextItem* mpNameDisplay;

  //! Configuration of the next element that is added to the scene.
  std::map<cedar::proc::Element*, cedar::aux::ConfigurationNode> mNextElementUiConfigurations;

  cedar::aux::BoolParameterPtr _mSmartMode;

  cedar::aux::ConfigurationNode mPlotGroupsNode;

}; // class cedar::proc::gui::NetworkFile

#endif // CEDAR_PROC_GUI_NETWORK_FILE_H
