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

#ifndef CEDAR_PROC_GUI_GROUP_H
#define CEDAR_PROC_GUI_GROUP_H

// CEDAR INCLUDES
#include "cedar/processing/gui/Connectable.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/Group.h"
#include "cedar/auxiliaries/LockableMember.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/DataSlotItem.fwd.h"
#include "cedar/processing/gui/Group.fwd.h"
#include "cedar/processing/gui/GroupWidget.fwd.h"
#include "cedar/auxiliaries/Configurable.fwd.h"

// SYSTEM INCLUDES
#include <QObject>
#include <QWeakPointer>
#ifndef Q_MOC_RUN
  #include <boost/signals2/signal.hpp>
  #include <boost/signals2/connection.hpp>
#endif // Q_MOC_RUN
#include <map>
#include <vector>
#include <string>
#include <list>
#include <unordered_set>

#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/devices/gui/KinematicChainWidget.h"

/*!@brief The representation of a cedar::proc::Group in a cedar::proc::gui::Scene.
 *
 *        This class takes care of loading cedar::proc::Groups in a manner that allows them to be added into
 *        cedar::proc::gui::Scenes as either the root group or a subgroup.
 */
class cedar::proc::gui::Group : public cedar::proc::gui::Connectable
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::proc::gui::GroupWidget;
  friend class cedar::proc::gui::Scene;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Group
  (
    QMainWindow *pMainWindow,
    cedar::proc::gui::Scene* scene,
    qreal width = static_cast<qreal>(250),
    qreal height = static_cast<qreal>(250),
    cedar::proc::GroupPtr group = cedar::proc::GroupPtr()
  );

  //!@brief Destructor
  ~Group();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief write group to file
  void write() const;
  void writeTo(std::string file) const;

  //!@brief write configuration to path
  void writeJson(const cedar::aux::Path& filename) const;

  //!@brief read configuration from path
  void readJson(const cedar::aux::Path& filename);

  //!@brief called after the architecture has been loaded
  void afterArchitectureLoaded();

  //! Checks if any connectables in the given list can be added to this group. Non-connectables are ignored.
  bool canAddAny(const QList<QGraphicsItem*>& items) const;

  /*!@brief access the underlying cedar::proc::Group
   */
  cedar::proc::GroupPtr getGroup();

  /*!@brief access the underlying cedar::proc::Group
   */
  cedar::proc::ConstGroupPtr getGroup() const;

  /*!@brief access the underlying cedar::proc::Group
   */
  void setGroup(cedar::proc::GroupPtr group);

  //!@brief get the current file, to which the group configuration can be saved
  const std::string& getFileName() const;

  /*!@brief Resizes the group to exactly fit all its contents.
   *
   * @param grow If true, the item will only get larger, but not smaller.
   */
  void fitToContents(bool grow = false);

  //! Returns the GUI data slot item for the given connector slot.
  cedar::proc::gui::DataSlotItem* getSourceConnectorItem(cedar::proc::DataSlotPtr slot) const;

  //! Returns the GUI data slot item for the given connector slot.
  cedar::proc::gui::DataSlotItem* getSinkConnectorItem(cedar::proc::DataSlotPtr slot) const;

  //!@brief Adds an element to the group.
  void addElement(cedar::proc::gui::GraphicsBase *pElement);

  //!@brief Adds a list of elements to the group efficiently.
  void addElements(const std::list<QGraphicsItem*>& elements);

  //! Duplicates an element and places it at the given position.
  cedar::proc::gui::Element* duplicate(const QPointF& scenePos, const std::string& elementName, const std::string& newName = "");

  //!@brief Sets the scene containing this item.
  void setScene(cedar::proc::gui::Scene* pScene);

  //!@brief reads a configuration from a node
  void readConfiguration(const cedar::aux::ConfigurationNode& node);

  /*!@brief Reads the defined robots from a configuration node and initializes them in the RobotManager
   */
  void readRobots(const cedar::aux::ConfigurationNode &root);

  //!@brief saves a configuration to a node
  void writeConfiguration(cedar::aux::ConfigurationNode& root) const;

  //! Disconnects the group.
  void disconnect();

  //! deals with changes to the group gui item
  QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant & value);

  //! deals with a mouse release event
  bool sceneEventFilter(QGraphicsItem* pWatched, QEvent* pEvent);

  //! get the scene in which this group is embedded
  cedar::proc::gui::Scene* getScene()
  {
    return this->mpScene;
  }

  //! get the scene in which this group is embedded
  cedar::proc::gui::Scene* getScene() const
  {
    return this->mpScene;
  }

  //! Opens the kinematic chain control widget from saved configuration file
  void openKinematicChainWidget(const aux::ConfigurationNode &node);

  void insertKinematicChainWidget(cedar::dev::gui::KinematicChainWidget* kinematicChainWidget) const;

  void insertViewer(cedar::aux::gui::Viewer* viewer) const;

  //! Opens a view on the simulated scene
  void openSceneViewer();
  void openSceneViewer(const cedar::aux::ConfigurationNode& node);

  /*!@brief Sets the ui configuration for the element when it is added to the group.
   */
  inline void setNextElementUiConfiguration
  (
    cedar::proc::ElementPtr element, const cedar::aux::ConfigurationNode& uiDescription
  )
  {
    this->mNextElementUiConfigurations[element.get()] = uiDescription;
  }

  //! Sets the smart connection mode for all elements in this group.
  void toggleSmartConnectionMode(bool smart)
  {
    this->_mSmartMode->setValue(smart);
  }

  //! Returns whether smart connection mode is used for all elements in this group.
  bool getSmartConnection() const
  {
    return this->_mSmartMode->getValue();
  }

  //! creates plot group of provided name containing all currently opened plots
  void addPlotGroup(std::string plotGroupName);

  //! creates plot group of provided name containing all currently opened plots
  void editPlotGroup(std::string plotGroupName);

  //! removes plot group of given name
  void removePlotGroup(std::string plotGroupName);

  //! renames plot group of given name (from) to given name (to)
  void renamePlotGroup(std::string from, std::string to);

  //! returns the name of every plot group of this group
  std::list<std::string> getPlotGroupNames();

  //! returns whether a name already exists in plot group list
  bool plotGroupNameExists(const std::string& newName) const;

  //! opens the given plot group
  void displayPlotGroup(std::string plotGroupName);

  //! determines if the specific group is currently visible
  bool isPlotGroupVisible(std::string plotGroupName);

  //!@brief handles events in the context menu
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

  //! Returns whether or not this group is collapsed.
  bool isCollapsed() const
  {
    return this->_mIsCollapsed->getValue();
  }

  //! search and replace every occurance of 'from' with 'to' in the plot groups node
  void changeStepName(const std::string& from, const std::string& to);

  //! Returns the architecture plots for this group.
  const std::map<std::string, cedar::aux::Path>& getArchitectureWidgets() const;

  //! Sets the architecture widgets for this group.
  void setArchitectureWidgets(const std::map<std::string, cedar::aux::Path>& newWidgets);

  //! Displays the architecture plot with the given name.
  void showArchitectureWidget(const std::string& name);

  //! Changes the visibility of all open architecture widgets
  void toggleVisibilityOfOpenArchitectureWidgets(bool visible);

  void toggleVisibilityOfOpenKinematicChainWidgets(bool visible);

  void toggleVisibilityOfOpenSceneViewers(bool visible);

  //! Closes all open architecture widgets
  void closeOpenArchitectureWidgets();

  //! Returns a color for a given looped trigger
  QBrush getColorFor(cedar::proc::LoopedTriggerPtr trigger) const;

  void toggleTriggerColors(bool show);

  //! Returns whether or not the trigger colors of elements in this group should be shown.
  bool showsTriggerColors() const;

  void updateTriggerColorState();

  //! Returns the slot item used for the given group source.
  cedar::proc::gui::DataSlotItem* getSlotItemFor(cedar::proc::sources::GroupSourcePtr source) const;

  //! Returns true in this case, as deleting groups requires confirmation.
  bool manualDeletionRequiresConfirmation() const;

  //! Implements supported display modes for groups.
  bool supportsDisplayMode(cedar::proc::gui::Connectable::DisplayMode::Id id) const;

  //! Defines draggability of groups.
  bool canBeDragged() const;

  //! Inherited, see super class.
  bool canShowTriggerChains() const;

public slots:
  /*! sets the recording state of all steps
   * @todo why is this done here? why is this done for all steps if one changes??
   */
  void stepRecordStateChanged();

  //! set collapsedness of this group
  void setCollapsed(bool collapsed);

  //! handes a change in step name
  void handleStepNameChanged(const std::string& from, const std::string& to);

  //! Enables/disables resizing and moving of the group.
  void setLockGeometry(bool lock = true);

  //! Calls reset on the underlying group, i.e., resets all elements in the group displayed by this item.
  void reset();

  //! Opens a container that displays this group.
  void openGroupContainer();

  void setAllPlotGroupsVisibility(bool visibility);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! handles removal of a slot
  void slotRemoved(cedar::proc::DataRole::Id role, const std::string& name);

  /*!@brief Handles the drop event of the scene.
   *
   *        This method mainly instantiates elements that are dropped from the Element toolbar to create new items in
   *        the scene.
   */
  void dropEvent(QGraphicsSceneDragDropEvent *pEvent);

  //! Overrides Qt's dragEnterEvent.
  void dragEnterEvent(QGraphicsSceneDragDropEvent *pEvent);

  /*!@brief Handles the dragLeave event of the scene.
   */
  void dragLeaveEvent(QGraphicsSceneDragDropEvent *pEvent);

  /*!@brief Handles the dragMove event of the scene.
   *
   *        This method determines whether the contents of the drop can be handled by
   *        cedar::proc::gui::Scene::dropEvent.
   */
  void dragMoveEvent(QGraphicsSceneDragDropEvent *pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  std::string getStringForElementType(cedar::proc::ConstElementPtr element) const;

  void tryToRestoreUIConfiguration
       (
         cedar::aux::ConfigurationNode& conf,
         cedar::proc::ElementPtr element,
         cedar::proc::gui::GraphicsBase* pSceneElement
       );

  void tryToRestoreGroupUIConfiguration
       (
         cedar::aux::ConfigurationNode& conf,
         cedar::proc::gui::GraphicsBase* pSceneElement
       );

  //! Restores the UI configurations for any elements that are in the scene.
  void tryRestoreUIConfigurationsOfElements(cedar::aux::ConfigurationNode& conf);

  //! Adds all the steps already in the group
  void addGuiItemsForGroup();

  //!@brief write scene to a node
  void writeScene(cedar::aux::ConfigurationNode& root) const;
  void writeView(cedar::aux::ConfigurationNode& root) const;

  //!@brief Determines whether the group is the root group.
  bool isRootGroup();

  void checkSlots();

  void checkDataItems();

  void updateConnectorPositions();

  //!@brief Transforms the coordinates of a newly added child into the group's coordinate system.
  void transformChildCoordinates(cedar::proc::gui::GraphicsBase* pItem);

  //!@brief a function that translates a boost signal to check a data connection into a Qt signal
  void checkDataConnection
       (
         cedar::proc::ConstDataSlotPtr source,
         cedar::proc::ConstDataSlotPtr target,
         cedar::proc::Group::ConnectionChange change
       );

  void checkTriggerConnection(cedar::proc::TriggerPtr, cedar::proc::TriggerablePtr, bool added);

  void processElementAddedSignal(cedar::proc::ElementPtr);

  void processElementRemovedSignal(cedar::proc::ConstElementPtr);

//  void readPlotList(const std::string& plotGroupName, const cedar::aux::ConfigurationNode& node);

  void writeOpenPlotsTo(cedar::aux::ConfigurationNode& node, bool onlyVisiblePlots = false) const;

  void sizeChanged();

  void itemSceneHasChanged();

  void removeConnectorItem(bool isSource, const std::string& name);

  qreal getIconSizeForCurrentMode() const;

  cedar::proc::gui::Element* getUiElementFor(cedar::proc::ElementPtr element) const;

  void readStickyNotes(const cedar::aux::ConfigurationNode& node);
  void readView(const cedar::aux::ConfigurationNode& node);

  /*!@brief this function emits a connection added signal for all connections in the underlying network.
   * It can be used to restore connections after moving or duplicating groups
   */
  void restoreConnections();

  void linkedChanged(bool readOnly);

  void lastReadConfigurationChanged();

  bool canResize() const;

  void clearTriggerColorCache() const;

  void updateAllElementsTriggerColorState() const;

  void addElementsToGroup();

  void internalWriteJson(const cedar::aux::Path& filename) const;

signals:
  //!@brief signal that is emitted when a boost signal is received
  void signalDataConnectionChange(QString, QString, QString, QString, cedar::proc::Group::ConnectionChange);

  //! Emitted whenever trigger colors need updating.
  void triggerColorsChanged() const;

private slots:
  //!@brief Updates the label of the group.
  void groupNameChanged();

  void toggleSmartConnectionMode();

  //!@brief handle an internal signal to create or remove gui connections
  void dataConnectionChanged
       (
         QString sourceName,
         QString sourceSlot,
         QString targetName,
         QString targetSlot,
         cedar::proc::Group::ConnectionChange change
       );

  void updateCollapsedness();

  //! Updates the position(s) and size(s) of the texts attached to this group.
  void updateTextBounds();

  //! Updates the position and size of the icon.
  void updateIconBounds();

  void loopedChanged();

  void removeElementFromPlotGroup(const std::string& plotGroupname, const std::string& elementName);

  void openParameterEditor();

  void backgroundColorActionTriggered();

  void elementNameChanged(const std::string&, const std::string&);

  void geometryLockChanged();

  void togglePlotGroupVisibility(bool visible, cedar::aux::ConfigurationNode& node, std::string plotGroupName);

  void removeViewer();

  void removeKinematicChainWidget();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief represented group
  cedar::proc::GroupPtr mGroup;

  //!@brief a scene, which displays the elements contained in this group
  cedar::proc::gui::Scene* mpScene;

  //!@brief a filename from which to load a group configuration, or to which to save a configuration
  mutable std::string mFileName;

  //!@brief a vector containing all the viewers
  mutable std::vector<cedar::aux::gui::Viewer*> mViewers;

  //!@brief a vector containing all the KinematicChainWidget for savin purposes...
  mutable std::vector<cedar::dev::gui::KinematicChainWidget*> mKinematicChainWidgets;

  //!@brief a vector of all source connectors
  std::vector<cedar::proc::gui::DataSlotItem*> mConnectorSources;

  //!@brief a vector of all sink connectors
  std::vector<cedar::proc::gui::DataSlotItem*> mConnectorSinks;

  //!@brief a vector of steps, which contains all steps that should be added to the scene after reading a configuration
  std::vector<cedar::proc::gui::StepItem*> mpStepsToAdd;

  //!@brief a vector of triggers, which contains all steps that should be added to the scene
  //        after reading a configuration
  std::vector<cedar::proc::gui::TriggerItem*> mpTriggersToAdd;

  //!@brief a vector of steps, which contains all steps that should be added to the scene after reading a configuration
  std::vector<cedar::proc::gui::Group*> mpGroupsToAdd;

  //! Map assigning colors to looped triggers. This is a cache to make calculations faster. The real assignment is determined algorithmically.
  mutable std::map<cedar::proc::TriggerPtr, QBrush> mTriggerColors;

  boost::signals2::scoped_connection mNewElementAddedConnection;
  boost::signals2::scoped_connection mElementRemovedConnection;
  boost::signals2::scoped_connection mTriggerConnectionChangedConnection;
  boost::signals2::scoped_connection mDataConnectionChangedConnection;
  boost::signals2::scoped_connection mLinkedChangedConnection;
  boost::signals2::scoped_connection mLastReadConfigurationChangedConnection;

  //! Fit to contents-calls are temporarily disabled if this is set to true.
  bool mHoldFitToContents;

  //! Text item used for displaying the name of the group.
  QGraphicsTextItem* mpNameDisplay;

  //! Configuration of the next element that is added to the scene.
  std::map<cedar::proc::Element*, cedar::aux::ConfigurationNode> mNextElementUiConfigurations;

  cedar::proc::gui::Connectable::DecorationPtr mpLinkedDecoration;

  std::vector<QWeakPointer<QWidget>> mArchitectureWidgetDocks;

  bool mShowTriggerColors;

  //! The vertical offset for data slots in the group used when the group is expanded.
  static const qreal M_EXPANDED_SLOT_OFFSET;

  //! The size of the icon in the expanded mode.
  static const qreal M_EXPANDED_ICON_SIZE;

  //! The size of the icon in the collapsed mode.
  static const qreal M_COLLAPSED_ICON_SIZE;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
private:
  cedar::aux::BoolParameterPtr _mSmartMode;

  cedar::aux::ConfigurationNode mPlotGroupsNode;

  cedar::aux::BoolParameterPtr _mIsCollapsed;

  //! Disables moving/resizing the group
  cedar::aux::BoolParameterPtr _mGeometryLocked;

  //! Width of the group in its uncollapsed state.
  cedar::aux::DoubleParameterPtr _mUncollapsedWidth;

  //! Height of the group in its uncollapsed state.
  cedar::aux::DoubleParameterPtr _mUncollapsedHeight;

  //! Map containing all the architecture plots. Keys are the names of the plots, values the paths to the files defining them.
  std::map<std::string, cedar::aux::Path> _mArchitectureWidgets;

}; // class cedar::proc::gui::GroupFile

#endif // CEDAR_PROC_GUI_GROUP_H
