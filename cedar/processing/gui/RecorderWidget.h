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

    File:        Recorder.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2013 08 19

    Description: Header for the @em cedar::aux::gui::RecorderWidget class.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_PROC_GUI_RECORDER_WIDGET_H
#define CEDAR_PROC_GUI_RECORDER_WIDGET_H

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/processing/Connectable.fwd.h"
#include "cedar/processing/gui/RecorderWidget.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QVBoxLayout>

/*!@brief GUI representation for the recorder tool.
  */
class cedar::proc::gui::RecorderWidget
:
public QWidget
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The public constructor.
  RecorderWidget();

  //!@brief The public constructor.
  RecorderWidget(QWidget* pParent);

  //!@brief The public destructor.
  ~RecorderWidget();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Sets the obtained step and recreating the widget to set the record parameters.
  void setConnectable(cedar::proc::ConnectablePtr connectable);

  void clear();

  /*!@brief If the name of a Step has changed all slots have to unregister in the recorder and
   *registered with the new name.
   */
  void emitStepRegisteredinRecorder();

signals:
  //! signal that a new step is registered in recorder
  void stepRegisteredinRecorder();

  //! signal that the settings changed
  void settingsChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Updates the GUI.
  void refreshWidget();

  //!@brief Create the headers for the Widget.
  void createHeader(const std::string& name);

  //!@brief Create a header for a role section.
  void createRoleSection(const std::string& name);

  //!@brief Resets the widget and its GUI elements.
  void clearLayout();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief The step currently displayed.
  cedar::proc::ConnectablePtr mConnectable;

  //!@brief The layout for this widget.
  QVBoxLayout* mMainLayout;
};

#endif // CEDAR_PROC_GUI_RECORDER_WIDGET_H
