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

    File:        Scene.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 05

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_SCENE_H
#define CEDAR_PROC_SCENE_H

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/processing/gui/DataSlotItem.fwd.h"
#include "cedar/processing/gui/Element.fwd.h"
#include "cedar/processing/gui/Scene.fwd.h"
#include "cedar/processing/gui/StepItem.fwd.h"
#include "cedar/processing/gui/View.fwd.h"
#include "cedar/processing/Element.fwd.h"
#include "cedar/processing/Group.fwd.h"
#include "cedar/processing/Step.fwd.h"
#include "cedar/processing/Trigger.fwd.h"
#include "cedar/processing/gui/GraphicsBase.fwd.h"
#include "cedar/processing/gui/Group.fwd.h"
#include "cedar/processing/gui/RecorderWidget.fwd.h"
#include "cedar/processing/gui/CommentWidget.fwd.h"
#include "cedar/processing/gui/StickyNote.fwd.h"
#include "cedar/processing/gui/TriggerItem.fwd.h"
#include "cedar/auxiliaries/gui/Configurable.fwd.h"
#include "cedar/auxiliaries/PluginDeclaration.fwd.h"

// SYSTEM INCLUDES
#include <QGraphicsScene>
#include <QMainWindow>
#include <map>
#include <vector>
#include <string>
#include <tuple>


/*!@brief This is a QGraphicsScene specifically designed for drawing cedar::proc::Groups.
 */
class cedar::proc::gui::Scene : public QGraphicsScene
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::proc::gui::StepItem;
  friend class cedar::proc::gui::TriggerItem;
  friend class cedar::proc::gui::Group;

  //--------------------------------------------------------------------------------------------------------------------
  // macros & types
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

public:
  //! Current tool mode
  enum MODE
  {
    //! Selection mode, i.e., items can be selected, moved etc.
    MODE_SELECT,
    //! Connection mode, i.e., connections can be created.
    MODE_CONNECT
  };

  //! Current trigger connection mode
  enum TRIGGER_CONNECTION_MODE
  {
    //!
    MODE_HIDE_ALL,
    //!
    MODE_SHOW_ALL,
    //!
    MODE_SMART
  };

  //!@todo Can StepMap, TriggerMap, GroupMap be removed now that the ElementMap has a proc::gui::Element pointer?
  //! Type for associating cedar::proc::Steps to cedar::proc::gui::StepItems.
  typedef std::map<const cedar::proc::Step*, cedar::proc::gui::StepItem*> StepMap;

  //! Type for associating cedar::proc::Triggers to cedar::proc::gui::TriggerItem.
  typedef std::map<const cedar::proc::Trigger*, cedar::proc::gui::TriggerItem*> TriggerMap;

  //! Type for associating cedar::proc::Groups to cedar::proc::gui::Groups.
  typedef std::map<const cedar::proc::Group*, cedar::proc::gui::Group*> GroupMap;

  //! Type for associating cedar::proc::Elements to cedar::proc::gui::GraphicsBase.
  typedef std::map<const cedar::proc::Element*, cedar::proc::gui::Element*> ElementMap;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The scene's constructor.
   *
   * @param peParentView  The view currently displaying the scene.
   * @param pParent       The parent of the QObject.
   * @param pMainWindow   The main window containing the scene.
   */
  Scene(cedar::proc::gui::View* peParentView, QObject *pParent = NULL, QMainWindow *pMainWindow = NULL);

  //!@brief Destructor.
  ~Scene();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Handler for mouse press events that happen within the bounds of the scene.
   */
  void mousePressEvent(QGraphicsSceneMouseEvent *pMouseEvent);

  /*!@brief Handler for mouse move events that happen within the bounds of the scene.
   */
  void mouseMoveEvent(QGraphicsSceneMouseEvent *pMouseEvent);

  /*!@brief Handler for mouse release events that happen within the bounds of the scene.
   */
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *pMouseEvent);

  /*!@brief Creates an element of the given classId at the specified position and adds it to the scene.
   */
  cedar::proc::ElementPtr createElement(cedar::proc::GroupPtr group, const std::string& classId, QPointF position);

  /*!@brief Creates an element of the given classId with the given name at the specified position and adds it to the scene.
   */
  cedar::proc::ElementPtr createElement
  (
    cedar::proc::GroupPtr group,
    const std::string& classId,
    const std::string& desiredName,
    QPointF position
  );

  /*!@brief Adds a cedar::proc::gui::StepItem for the given cedar::proc::Step to the scene at the given position.
   */
  void addProcessingStep(cedar::proc::StepPtr step, QPointF position);

  /*!@brief Adds a cedar::proc::gui::StepItem to the scene.
   */
  void addStepItem(cedar::proc::gui::StepItem* pStep);


  /*!@brief Adds a cedar::proc::gui::TriggerItem for the given cedar::proc::Trigger to the scene at the given position.
   */
  void addTrigger(cedar::proc::TriggerPtr trigger, QPointF position);

  /*!@brief Adds a cedar::proc::gui::TriggerItem to the scene.
   */
  void addTriggerItem(cedar::proc::gui::TriggerItem* pTrigger);


  /*!@brief Adds a given group item to the scene.
   */
  cedar::proc::gui::Group* addGroup
  (
    const QPointF& position,
    cedar::proc::GroupPtr group = cedar::proc::GroupPtr()
  );


  /*!@brief Adds a given group item to the scene.
   */
  void addGroupItem(cedar::proc::gui::Group* pGroup);

  /*!@brief Sets the current mode, i.e., selection, connecion etc.
   */
  void setMode(MODE mode, const QString& param = "");

  /*!@brief Sets the main window containing this scene.
   */
  void setMainWindow(QMainWindow *pMainWindow);

  /*!@brief Sets the group that is displayed by this scene.
   */
  void setGroup(cedar::proc::gui::GroupPtr group);

  /*!@brief Resets the group.
   */
  void reset();

  /*!@brief Returns the step map.
   */
  const StepMap& getStepMap() const;

  /*!@brief Returns the group map.
   */
  const GroupMap& getGroupMap() const;

  /*!@brief Returns the trigger map.
   */
  const TriggerMap& getTriggerMap() const;

  /*!@brief Returns the gui::group that displays the given group.
   */
  cedar::proc::gui::Group* getGroupFor(cedar::proc::ConstGroup* group);

  /*!@brief Returns the step item that displays the given step.
   */
  cedar::proc::gui::StepItem* getStepItemFor(cedar::proc::Step* step);

  /*!@brief Returns the trigger item that displays the given trigger.
   */
  cedar::proc::gui::TriggerItem* getTriggerItemFor(cedar::proc::Trigger* trigger);

  /*!@brief Returns the cedar::proc::gui::GraphicsBase item corresponding to the given element.
   */
  cedar::proc::gui::Element* getGraphicsItemFor(cedar::proc::ConstElement* element);

  //! Returns the graphics item corresponding to the given element.
  cedar::proc::gui::Element* getGraphicsItemFor(cedar::proc::ConstElementPtr element);

  /*!@brief Returns, whether snap-to-grid is true.
   */
  bool getSnapToGrid() const;

  /*!@brief Enables or disables the snap-to-grid function.
   */
  void setSnapToGrid(bool snap);

  /*!@brief Access the root group
   */
  cedar::proc::gui::GroupPtr getRootGroup();

  /*!@brief Access the root group.
   */
  cedar::proc::gui::ConstGroupPtr getRootGroup() const;
  
  /*!@brief Returns the current mode.
   */
  MODE getMode() const
  {
    return this->mMode;
  }

  /*!@brief Sets the widget used for displaying/editing the parameters of configurables.
   */
  void setConfigurableWidget(cedar::aux::gui::Configurable* pConfigurableWidget);

  /*!@brief Sets the widget used for displaying/editing the record parameters.
   */
  void setRecorderWidget(cedar::proc::gui::RecorderWidget* pRecorderWidget);

  void setCommentWidget(cedar::proc::gui::CommentWidget* pCommentWidget);

  /*!@brief Exports the scene to an svg file
   */
  void exportSvg(const QString& file);

  /*! Sets the display mode for triggers.
   */
  void setTriggerDisplayMode(TRIGGER_CONNECTION_MODE mode)
  {
    this->mTriggerMode = mode;
    this->handleTriggerModeChange();
  }

  /*! Returns the current display mode for triggers.
   */
  TRIGGER_CONNECTION_MODE getTriggerDisplayMode() const
  {
    return this->mTriggerMode;
  }

  //! select all items
  void selectAll();

  //! deselect all items
  void selectNone();

  /*!brief Adds a sticky note to the current scene
   */
  void addStickyNote();

  //! adds a sticky note of given size at given position
  cedar::proc::gui::StickyNote* addStickyNote(float x, float y, float witdh, float height, std::string text, int fontSize = 10, QColor color = QColor(255,255,110));

  //! Removes a sticky note
  void removeStickyNote(StickyNote* note);

  //! Gets all sticky notes
  const std::vector<cedar::proc::gui::StickyNote* > getStickyNotes() const;

  //! emits a scene changed signal
  void emitSceneChanged();

  //! Returns the configurable widget of the scene.
  cedar::aux::gui::Configurable* getConfigurableWidget() const;

  //! Returns the configurable widget of the scene.
  cedar::proc::gui::RecorderWidget* getRecorderWidget() const;

  cedar::proc::gui::CommentWidget* getCommentWidget() const;

  /*!@brief sort two QGraphicsItems measuring their depth in relation to the root network.
   */
  static bool sortElements(QGraphicsItem* pFirstItem, QGraphicsItem* pSecondItem);

  /*! Returns a list of selected items where those items whose parents are in the selection are removed.
   */
  QList<QGraphicsItem*> getSelectedParents() const;

  bool isDragging();

  cedar::proc::gui::View* getParentView();
  //--------------------------------------------------------------------------------------------------------------------
  // signals
  //--------------------------------------------------------------------------------------------------------------------
signals:
  /*!@brief Signal that is emitted when the current mode finishes, e.g., when a connection has been made.
   */
  void modeFinished();

  //! Emitted whenever something in the scene changes (e.g., when an item is moved)
  void sceneChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief Displays the context menu for the scene (if no item accepts the event).
   */
  void contextMenuEvent(QGraphicsSceneContextMenuEvent* pContextMenuEvent);

  /*! Overrides the default help event to display tooltips for elements in the scene.
   */
  void helpEvent(QGraphicsSceneHelpEvent* pHelpEvent);

  void keyPressEvent(QKeyEvent* pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Mouse press event for the connect mode.
   */
  void connectModeProcessMousePress(QGraphicsSceneMouseEvent *pMouseEvent);

  /*!@brief Mouse move event for the connect mode.
   */
  void connectModeProcessMouseMove(QGraphicsSceneMouseEvent *pMouseEvent);

  /*!@brief Mouse release event for the connect mode.
   */
  void connectModeProcessMouseRelease(QGraphicsSceneMouseEvent *pMouseEvent);

  /*!@brief Adds the names of groups and their subgroups to an action.
   */
  void addGroupNames(QMenu* pMenu, cedar::proc::ConstGroupPtr group, std::string path) const;

  /*!@brief Removes a cedar::proc::gui::TriggerItem from the scene.
   */
  void removeTriggerItem(cedar::proc::gui::TriggerItem* pTrigger);

  /*!@brief Removes a cedar::proc::gui::StepItem from the scene.
   */
  void removeStepItem(cedar::proc::gui::StepItem* pStep);

  /*!@brief Removes a given group item from the scene.
   */
  void removeGroupItem(cedar::proc::gui::Group* pGroup);

  void handleTriggerModeChange();

  //! Responsible for highlighting group targets when the mouse is dragging items around.
  void highlightTargetGroups(const QPointF& mousePosition);

  void resetBackgroundColor();

  cedar::proc::gui::GraphicsBase* findConnectableItem(const QList<QGraphicsItem*>& items);

  cedar::proc::gui::Group* findFirstGroupItem(const QList<QGraphicsItem*>& items);

  void multiItemContextMenuEvent(QGraphicsSceneContextMenuEvent* pContextMenuEvent);

  void connectSlots(cedar::proc::gui::DataSlotItem* pSource, cedar::proc::gui::DataSlotItem* pTarget, bool addConnectorGroup);

  /*!@brief Deletes the list of graphics items.
   */
  void deleteElements(QList<QGraphicsItem*>& items, bool skipConfirmation = false);

  /*!@brief Delete a single graphics item.
   */
  void deleteElement(QGraphicsItem* pItem);

  /*!@brief Deletes the elements currently selected in the scene.
   */
  void deleteSelectedElements(bool skipConfirmation = false);

  void dragEnterEvent(QGraphicsSceneDragDropEvent *pEvent);

  void dragLeaveEvent(QGraphicsSceneDragDropEvent *pEvent);

  void dragMoveEvent(QGraphicsSceneDragDropEvent *pEvent);

  void dropEvent(QGraphicsSceneDragDropEvent *pEvent);

private slots:
  void promoteElementToExistingGroup();

  /*!@brief Slot that is called whenever a different item is selected in the cedar::proc::gui::Scene.
   */
  void itemSelected();

  //!@todo importGroup and importStep share a lot of code
  void importGroup(bool link);
  void importStep();

  void assignSelectedToTrigger();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! The current mode.
  MODE mMode;

  //! The current trigger mode.
  TRIGGER_CONNECTION_MODE mTriggerMode;

  //! The parameter for the current mode.
  QString mModeParam;

  //! The group displayed by the scene.
  cedar::proc::gui::GroupPtr mGroup;

  //! Stores what graphics item receives a drop, if any.
  QGraphicsItem* mpDropTarget;

  //! Group into which the items currently being moved would be added
  cedar::proc::GroupPtr mTargetGroup;

  //! The view displaying the scene.
  cedar::proc::gui::View* mpeParentView;

  //! The line indicating a new connection.
  QGraphicsLineItem* mpNewConnectionIndicator;

  //! The item from which a new connection is started.
  cedar::proc::gui::GraphicsBase* mpConnectionStart;

  //! The step map.
  StepMap mStepMap;

  //! The trigger map.
  TriggerMap mTriggerMap;

  //! The group map.
  GroupMap mGroupMap;

  //! Map of all the elements.
  ElementMap mElementMap;

  //! List of all sticky notes
  std::vector<cedar::proc::gui::StickyNote*> mStickyNotes;

  //! The main window containing the scene.
  QMainWindow* mpMainWindow;

  //! Bool representing whether the snap-to-grid function is active.
  bool mSnapToGrid;

  //! The widget used to display configurables when they are selected in the scene. May be null.
  cedar::aux::gui::Configurable* mpConfigurableWidget;

  //! The widget used to display record settings of steps when they are selected in the scene. May be null.
  cedar::proc::gui::RecorderWidget* mpRecorderWidget;

  cedar::proc::gui::CommentWidget* mpCommentWidget;

  //! Saves the mouse x position in the scene
  int mMousePosX;

  //! Saves the mouse y position in the scene
  int mMousePosY;

  //! Whether or not the mouse is currently dragging items in the scene
  bool mDraggingItems;

}; // class ProcessingScene

#endif // CEDAR_PROC_SCENE_H
