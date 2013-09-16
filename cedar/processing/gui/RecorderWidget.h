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

    File:        Recorder.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2013 08 19

    Description: Header for the @em cedar::aux::gui::RecorderWidget class.

    Credits:

======================================================================================================================*/


#ifndef RECORDERWIDGET_H_
#define RECORDERWIDGET_H_

// CEDAR INCLUDES
#include <cedar/processing/gui/namespace.h>
#include <cedar/processing/Step.h>

// SYSTEM INCLUDES
#include <qwidget.h>
#include <QVBoxLayout>

/*!@brief GUI representation for the recorder tool.
  */
class cedar::proc::gui::RecorderWidget
:
public QWidget
{
  //----------------------------------------------------------------------------
  // macros
  //----------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The public constructor.
  RecorderWidget();

  //!@brief The public constructor.
  RecorderWidget( QWidget* pParent);

  //!@brief The public destructor.
  ~RecorderWidget();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public slots:
  //!@brief Sets the obtained step and recreating the widget to set the record parameters.
  void setStep(cedar::proc::StepPtr step);
 
  //!@brief Resets this widget
  void resetContents();

  //!@brief Unregister all slots of this step;
  void unregister(cedar::proc::StepPtr pStep);

  /*!@brief If the name of a Step has changed all slots have to unregister in the recoder and 
   *registered with the new name.
   */
  void updateName();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Updates the GUI.
  void refreshWidget();

  //!@brief Resets the widget and its GUI elements.
  void clearLayout();

  //!@brief Create the headers for the Widget.
  void createHeader(const std::string& name);

  //!@brief Create a header for a role section.
  void createRoleSection(const std::string& name);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief The step currently displayed.
  cedar::proc::StepPtr mStepToConfigure;

  //!@brief The layout for this widget.
  QVBoxLayout* mMainLayout;

};

#endif /* RECORDERWIDGET_H_ */
