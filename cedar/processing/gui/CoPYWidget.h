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

    File:        CoPYWidget.h

    Maintainer:  Frederik Bendel
    Email:       frederik.bendel@ruhr-uni-bochum.de
    Date:        2020 12 22

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifndef CEDAR_PROC_GUI_COPY_WIDGET_H
#define CEDAR_PROC_GUI_COPY_WIDGET_H
#ifdef CEDAR_USE_COPY

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/Parameter.fwd.h"
#include "cedar/processing/Step.fwd.h"
#include "cedar/processing/gui/CoPYWidget.fwd.h"
#include "cedar/processing/gui/CoPYObject.fwd.h"
#include "cedar/processing/gui/Group.fwd.h"
#include "cedar/processing/gui/PythonQtConsole.fwd.h"
#include "cedar/processing/gui/Scene.fwd.h"
#include "cedar/processing/gui/StepItem.fwd.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/CodeWidget.h"
// SYSTEM INCLUDES

#ifndef Q_MOC_RUN
#include <boost/signals2/connection.hpp>
#endif // Q_MOC_RUN

#include <QWidget>
#include <QVariant>
#include <vector>
#include <string>
#include <stdexcept>
#include <QApplication>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QGraphicsItem>
#include <QGraphicsObject>
#include <QGraphicsSceneDragDropEvent>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QVariant>
#include <QWidget>
#include <map>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsObject>
#include <QGraphicsItem>


//!@brief A class for Scripting with Python to copy/create things.
class cedar::proc::gui::CoPYWidget : public QWidget
{
  Q_OBJECT
public:
  //!@brief The standard constructor.
  CoPYWidget(QWidget *pParent = NULL);
  //! Destructor.
  ~CoPYWidget();

  //!@brief setter for cedar::proc::gui::Ide.cpp
  void setScene(cedar::proc::gui::Scene* pScene);
  //!@brief Lock Execute Button when simulation is running for cedar::proc::gui::Ide.cpp

  //!@brief methods for using existing StepItems in CoPY
  void importStepInformation(cedar::proc::gui::StepItem* pStep);
  void importStepInformation(QList<QGraphicsItem*> pSteps);

  //!@brief method for appending Text in CoPY
  void appendToConsole(std::string text);
  //!@brief append set Parameter Command to Console
  void appendParameterToText(cedar::aux::ParameterPtr parameter, const std::string paramPath);
  void lockExecuteButton(const bool disabled);
private slots:
  void executeButtonClicked();
  bool saveButtonClicked();
  void loadButtonClicked();
  void resetButtonClicked();
  void getVariablesButtonClicked();
  void helpButtonClicked();
  void setVariables(QMap<QString, QString> vars);

private:
  std::string removeTrailingZeros(const double& num);
  //Highlighter for Console
  cedar::proc::gui::CodeWidgetScope::PythonSyntaxHighlighter *mpHighlighter;

  //Layout-Objects
  QPushButton* mpExecuteButton;
  QPushButton* mpResetButton;
  QPushButton* mpSaveButton;
  QPushButton* mpLoadButton;
  QPushButton* mpVariablesButton;
  QPushButton* mpHelpButton;
  QCheckBox* mpAutoResetCheckbox;
  cedar::proc::gui::PythonQtConsole* mpConsole;

  QTableWidget* mpVariableView;

  //Reference to Scene
  cedar::proc::gui::Scene* mpScene;

  //counter to overview Existing-Step-Selections and not overwrite the python object.
  int mSelCounter;

private:

  //private methods

  //!@brief cvonert cedar::proc::gui::StepItem* to std::string of shape "groupId.stepName"
  std::string getStepInfo(cedar::proc::gui::StepItem* pStep);

  //!@brief method to execute code
  void executeCode();

  //!@brief insert command for creating dragged cedar::proc::Step
  void dropEvent(QDropEvent *pEvent);
  void dragEnterEvent(QDragEnterEvent *pEvent);
  void dragMoveEvent(QDragMoveEvent *pEvent);
  void reset();

  void updateDict();

};
#endif // CEDAR_USE_COPY
#endif // CEDAR_PROC_GUI_COPY_WIDGET_H

