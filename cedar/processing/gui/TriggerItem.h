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

    File:        TriggerItem.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 11

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_TRIGGER_ITEM_H
#define CEDAR_PROC_GUI_TRIGGER_ITEM_H

// CEDAR INCLUDES
#include "cedar/processing/Trigger.h"
#include "cedar/processing/gui/Connection.h"
#include "cedar/processing/gui/Element.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/CallFunctionInThread.fwd.h"
#include "cedar/auxiliaries/PluginDeclaration.h"
#include "cedar/processing/gui/TriggerItem.fwd.h"
#include "cedar/processing/gui/Group.fwd.h"

// SYSTEM INCLUDES


/*!@brief Representation of a cedar::proc::Trigger in a cedar::proc::gui::Scene.
 */
class cedar::proc::gui::TriggerItem : public QObject, public cedar::proc::gui::Element
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::proc::gui::Group;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  TriggerItem();

  /*!@brief The constructor that directly ties to an instance of Trigger
   * @param trigger an instance of Trigger
   */
  TriggerItem(cedar::proc::TriggerPtr trigger);

  //!@brief Destructor
  ~TriggerItem();

private:
  //!@brief construct the graphical representation
  void construct();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief paint the graphical representation
  void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);

  //!@brief Let the trigger know whether it is docked (usually to a step).
  void isDocked(bool docked);

  /*!@brief returns the instance of Trigger
   * @return the associated instance of Trigger
   */
  cedar::proc::TriggerPtr getTrigger();

  /*!@brief returns the instance of Trigger
   * @return the associated instance of Trigger
   */
  cedar::proc::ConstTriggerPtr getTrigger() const;

  //!@brief create the context menu for TriggerItem
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

  /*!@brief graphically connect to suitable GraphicsItem
   * @param pTarget pointer to GraphicsItem
   */
  cedar::proc::gui::Connection* connectTo(cedar::proc::gui::GraphicsBase *pTarget);

  //!@brief read configuration from a node
  void readConfiguration(const cedar::aux::ConfigurationNode& node);

  //!@brief save configuration to a node
  void writeConfiguration(cedar::aux::ConfigurationNode& root) const;

  //!@brief checks for a target, if Trigger can connect to this target
  cedar::proc::gui::ConnectValidity canConnectTo(GraphicsBase* pTarget) const;

  //!@brief disconnect from a listener
  void disconnect(cedar::proc::gui::GraphicsBase* pListener);

  bool canDuplicate() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief sets the trigger associated with this graphical representation
  void setTrigger(cedar::proc::TriggerPtr trigger);

  //! Updates the display of the step's run time measurements.
  void updateToolTip();

private slots:
  void triggerStateChanging();

  void triggerStarted();

  void triggerStopped();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief the internal trigger
  cedar::proc::TriggerPtr mTrigger;

  //! Used to start the trigger in a separate thread
  cedar::aux::CallFunctionInThreadPtr mStartCaller;

  //! Used to stop the trigger in a separate thread
  cedar::aux::CallFunctionInThreadPtr mStopCaller;

  //! Tracks whether the trigger is starting/stopping
  bool mStartingStopping;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //!@brief the class id of the internal trigger, used for a tooltip
  cedar::aux::ConstPluginDeclarationPtr mClassId;

}; // class TriggerItem

#endif // CEDAR_PROC_GUI_TRIGGER_ITEM_H
