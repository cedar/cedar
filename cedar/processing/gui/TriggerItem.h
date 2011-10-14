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

// LOCAL INCLUDES
#include "processing/Trigger.h"
#include "processing/gui/namespace.h"
#include "processing/gui/Connection.h"
#include "processing/gui/GraphicsBase.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::proc::gui::TriggerItem : public cedar::proc::gui::GraphicsBase
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

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

  //!@brief create the context menu for TriggerItem
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

  /*!@brief graphically connect to a StepItem
   * @param pTarget pointer to StepItem
   */
  void connectTo(cedar::proc::gui::StepItem *pTarget);

  /*!@brief graphically connect to a TriggerItem
   * @param pTarget pointer to TriggerItem
   */
  void connectTo(cedar::proc::gui::TriggerItem *pTarget);

  //!@brief read configuration from a node
  void readConfiguration(const cedar::aux::ConfigurationNode& node);

  //!@brief save configuration to a node
  void saveConfiguration(cedar::aux::ConfigurationNode& root);

  //!@brief checks for a target, if Trigger can connect to this target
  cedar::proc::gui::ConnectValidity canConnectTo(GraphicsBase* pTarget) const;

  //!@brief disconnect from a listener
  void disconnect(cedar::proc::gui::GraphicsBase* pListener);

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

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief the internal trigger
  cedar::proc::TriggerPtr mTrigger;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //!@brief the class id of the internal trigger, used for a tooltip
  cedar::proc::TriggerDeclarationPtr mClassId;

}; // class TriggerItem

#endif // CEDAR_PROC_GUI_TRIGGER_ITEM_H

