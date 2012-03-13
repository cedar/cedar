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
#include "cedar/processing/gui/namespace.h"
#include "cedar/processing/namespace.h"
#include "cedar/auxiliaries/gui/namespace.h"

// SYSTEM INCLUDES
#include <QGraphicsScene>
#include <QMainWindow>


/*!@brief This is a QGraphicsScene specifically designed for drawing cedar::proc::Networks.
 */
class cedar::proc::gui::Scene : public QGraphicsScene
{
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

  //! Type for associating cedar::proc::Steps to cedar::proc::gui::StepItems.
  typedef std::map<cedar::proc::Step*, cedar::proc::gui::StepItem*> StepMap;

  //! Type for associating cedar::proc::Triggers to cedar::proc::gui::TriggerItem.
  typedef std::map<cedar::proc::Trigger*, cedar::proc::gui::TriggerItem*> TriggerMap;

  //! Type for associating cedar::proc::Networks to cedar::proc::gui::Networks.
  typedef std::map<cedar::proc::Network*, cedar::proc::gui::Network*> NetworkMap;

  //! Type for associating cedar::proc::Elements to cedar::proc::gui::GraphicsBase.
  typedef std::map<cedar::proc::Element*, cedar::proc::gui::GraphicsBase*> ElementMap;

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
  /*!@brief Handles the drop event of the scene.
   *
   *        This method mainly instantiates elements that are dropped from the Element toolbar to create new items in
   *        the scene.
   */
  void dropEvent(QGraphicsSceneDragDropEvent *pEvent);

  /*!@brief Handles the dragEnter event of the scene.
   *
   *        This method determines whether the contents of the drop can be handled by
   *        cedar::proc::gui::Scene::dropEvent.
   */
  void dragEnterEvent(QGraphicsSceneDragDropEvent *pEvent);

  /*!@brief Handles the dragMove event of the scene.
   *
   *        This method determines whether the contents of the drop can be handled by
   *        cedar::proc::gui::Scene::dropEvent.
   */
  void dragMoveEvent(QGraphicsSceneDragDropEvent *pEvent);

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
  void addElement(const std::string& classId, QPointF position);

  /*!@brief Adds a cedar::proc::gui::StepItem for the given cedar::proc::Step to the scene at the given position.
   */
  void addProcessingStep(cedar::proc::StepPtr step, QPointF position);

  /*!@brief Adds a cedar::proc::gui::StepItem to the scene.
   */
  void addStepItem(cedar::proc::gui::StepItem *pStep);

  /*!@brief Removes a cedar::proc::gui::StepItem from the scene.
   */
  void removeStepItem(cedar::proc::gui::StepItem *pStep);

  /*!@brief Adds a cedar::proc::gui::TriggerItem for the given cedar::proc::Trigger to the scene at the given position.
   */
  void addTrigger(cedar::proc::TriggerPtr trigger, QPointF position);

  /*!@brief Adds a cedar::proc::gui::TriggerItem to the scene.
   */
  void addTriggerItem(cedar::proc::gui::TriggerItem *pTrigger);

  /*!@brief Removes a cedar::proc::gui::TriggerItem from the scene.
   */
  void removeTriggerItem(cedar::proc::gui::TriggerItem *pTrigger);

  /*!@brief Adds a given network item to the scene.
   */
  cedar::proc::gui::Network* addNetwork
  (
    const QPointF& position,
    cedar::proc::NetworkPtr network = cedar::proc::NetworkPtr()
  );

  /*!@brief Removes a given network item from the scene.
   */
  void removeNetworkItem(cedar::proc::gui::Network *pNetwork);

  /*!@brief Adds a given network item to the scene.
   */
  void addNetworkItem(cedar::proc::gui::Network *pNetwork);

  /*!@brief Sets the current mode, i.e., selection, connecion etc.
   */
  void setMode(MODE mode, const QString& param = "");

  /*!@brief Sets the main window containing this scene.
   */
  void setMainWindow(QMainWindow *pMainWindow);

  /*!@brief Sets the network that is displayed by this scene.
   */
  void setNetwork(cedar::proc::gui::NetworkPtr network);

  /*!@brief Resets the network.
   */
  void reset();

  /*!@brief Returns the step map.
   */
  const StepMap& stepMap() const;

  /*!@brief Returns the trigger map.
   */
  const TriggerMap& triggerMap() const;

  /*!@brief Returns the step item that displays the given step.
   */
  cedar::proc::gui::StepItem* getStepItemFor(cedar::proc::Step* step);

  /*!@brief Returns the trigger item that displays the given trigger.
   */
  cedar::proc::gui::TriggerItem* getTriggerItemFor(cedar::proc::Trigger* trigger);

  /*!@brief Returns the cedar::proc::gui::GraphicsBase item corresponding to the given element.
   */
  cedar::proc::gui::GraphicsBase* getGraphicsItemFor(cedar::proc::Element* trigger);

  /*!@brief Returns, whether snap-to-grid is true.
   */
  bool getSnapToGrid() const;

  /*!@brief Enables or disables the snap-to-grid function.
   */
  void setSnapToGrid(bool snap);

  /*!@brief Adds actions to the menu that relate to network groups.
   */
  void networkGroupingContextMenuEvent(QMenu& menu);

  /*!@brief Access the root network
   */
  cedar::proc::gui::NetworkPtr getRootNetwork();
  
  /*!@brief Returns the current mode.
   */
  MODE getMode() const
  {
    return this->mMode;
  }

  /*!@brief Sets the widget used for displaying/editing the parameters of configurables.
   */
  void setConfigurableWidget(cedar::aux::gui::PropertyPane *pConfigurableWidget);

  //--------------------------------------------------------------------------------------------------------------------
  // signals
  //--------------------------------------------------------------------------------------------------------------------
signals:
  /*!@brief Signal that is emitted when an exception occurs.
   */
  void exception(const QString& message);
  /*!@brief Signal that is emitted when the current mode finishes, e.g., when a connection has been made.
   */
  void modeFinished();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief Displays the context menu for the scene (if no item accepts the event).
   */
  void contextMenuEvent(QGraphicsSceneContextMenuEvent* pContextMenuEvent);

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

  /*!@brief Adds the names of networks and their subnetworks to an action.
   */
  void addNetworkNames(QMenu* pMenu, cedar::proc::ConstNetworkPtr network, std::string path) const;

private slots:
  void promoteElementToExistingGroup();

  void promoteElementToNewGroup();

  /*!@brief Slot that is called whenever a different item is selected in the cedar::proc::gui::Scene.
   */
  void itemSelected();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! The current mode.
  MODE mMode;

  //! The parameter for the current mode.
  QString mModeParam;

  //! The network displayed by the scene.
  cedar::proc::gui::NetworkPtr mNetwork;

  //! The view displaying the scene.
  cedar::proc::gui::View *mpeParentView;

  //! The line indicating a new connection.
  QGraphicsLineItem *mpNewConnectionIndicator;

  //! The item from which a new connection is started.
  cedar::proc::gui::GraphicsBase *mpConnectionStart;

  //! The step map.
  StepMap mStepMap;

  //! The trigger map.
  TriggerMap mTriggerMap;

  //! The network map.
  NetworkMap mNetworkMap;

  //! Map of all the elements.
  ElementMap mElementMap;

  //! The main window containing the scene.
  QMainWindow *mpMainWindow;

  //! Bool representing whether the snap-to-grid function is active.
  bool mSnapToGrid;

  //! The widget used to display configurables when they are selected in the scene. May be null.
  cedar::aux::gui::PropertyPane *mpConfigurableWidget;

}; // class ProcessingScene

#endif // CEDAR_PROC_SCENE_H
