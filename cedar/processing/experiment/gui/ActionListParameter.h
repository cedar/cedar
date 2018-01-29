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

    File:        ActionListParameter.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 07 02

    Description: Header file for the class cedar::proc::experiment::gui::ActionListParameter.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_GUI_ACTION_LIST_PARAMETER_H
#define CEDAR_PROC_EXPERIMENT_GUI_ACTION_LIST_PARAMETER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/Parameter.h"
#include "cedar/processing/experiment/action/Action.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/gui/ActionListParameter.fwd.h"

// SYSTEM INCLUDES
#include <QListWidget>

/*!@brief A GUI implementation of cedar::proc::experiment::action::Action::ActionListParamter
 *
 */
class cedar::proc::experiment::gui::ActionListParameter : public cedar::aux::gui::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:

  //!@brief A widget for displaying and communicating with the ActionList
  class ActionListWidget : public QListWidget
  {
    public:
      //! constructor
      ActionListWidget(ActionListParameter* parent): QListWidget(parent),mpParent(parent),mStartDragPos(0){}
    public:
      //! overloaded drag enter event
      void dragEnterEvent(QDragEnterEvent* event);

      //! overloaded drop event
      void dropEvent(QDropEvent* event);

      //! empty..
      void emitLayoutUpdateRequired()
      {
//        emit layoutChanged();
      }

    private:
      //!@brief Reference the the ActionListParamter.
      ActionListParameter* mpParent;

      //!@brief When a drag event has been started, this will save the current position of the Action in the list.
      int mStartDragPos;
  };
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ActionListParameter(QWidget* parent = NULL);;

  //!@brief Destructor
  virtual ~ActionListParameter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

public slots:
  //!@brief Adds a new Action to the ActionList.
  void addAction();

  //!@brief Redraws the ActionListWidget.
  void updateList();

  //!@brief Triggered when the ActionListParameter has changed.
  void parameterPointerChanged();

  //!@brief Triggered when a Action in the list has changed.
  void actionParameterChanged();

  //!@brief  Triggered when a Action in the list has been removed.
  void actionParameterRemoved();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief Reference to the ActionListParameter.
  cedar::proc::experiment::action::Action::ActionListParameterPtr mActionListParameter;

  //!@brief The Widget to display the ActionList.
  ActionListWidget* mActionListWidget;



}; // class cedar::proc::experiment::gui::ActionListParameter

#endif // CEDAR_PROC_EXPERIMENT_GUI_ACTION_LIST_PARAMETER_H

