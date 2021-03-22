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

    You should have received a Copy of the GNU Lesser General Public License
    along with cedar. If not, see <http://www.gnu.org/licenses/>.

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        CoPYWidget.cpp

    Maintainer:  Frederik Bendel
    Email:       frederik.bendel@ruhr-uni-bochum.de
    Date:        2020 12 22

    Description:

    Credits:

======================================================================================================================*/
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/CoPYWidget.h"
#include <cedar/processing/Step.h>
#include <cedar/processing/gui/StepItem.h>

#include "ElementList.h"

// SYSTEM INCLUDES
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QCheckBox>
#include <thread>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::CoPYWidget::CoPYWidget(QWidget* pParent)
:
QWidget(pParent)
{

  //create QTLayout
  QVBoxLayout *layout = new QVBoxLayout(reinterpret_cast<QWidget *>(this));
  layout->setMargin(0);

  mpConsole = new PythonQtConsole(this);

  this->reset();

  //appendHighlighter to Editor
  this->mpHighlighter = new cedar::proc::gui::CodeWidgetScope::PythonSyntaxHighlighter(mpConsole->document());
  mSelCounter = 0;

  //finalize widget and button
  layout->addWidget(mpConsole);
  layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  QHBoxLayout *buttons = new QHBoxLayout();
  mpExecuteButton = new QPushButton("Execute");
  QObject::connect(mpExecuteButton, SIGNAL (clicked()), this, SLOT(executeButtonClicked()));
  buttons->addWidget(mpExecuteButton);
  mpResetButton = new QPushButton("Reset");
  QObject::connect(mpResetButton, SIGNAL (clicked()), this, SLOT(resetButtonClicked()));
  buttons->addWidget(mpResetButton);
  mpAutoResetCheckbox = new QCheckBox("Auto Reset");
  buttons->addWidget(mpAutoResetCheckbox);
  /*mpSaveButton = new QPushButton("S");-
  QObject::connect(mpSaveButton, SIGNAL (clicked()), this, SLOT(saveButtonClicked()));
  buttons->addWidget(mpSaveButton);
  mpLoadButton = new QPushButton("L");
  QObject::connect(mpLoadButton, SIGNAL (clicked()), this, SLOT(loadButtonClicked()));
  buttons->addWidget(mpLoadButton);*/


  layout->addLayout(buttons);
  setAcceptDrops(true);

}

cedar::proc::gui::CoPYWidget::~CoPYWidget()
{

}

//----------------------------------------------------------------------------------------------------------------------
// Set up methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::CoPYWidget::setScene(cedar::proc::gui::Scene* pScene)
{
  mpConsole->setScene(pScene);
}

void cedar::proc::gui::CoPYWidget::importStepInformation(cedar::proc::gui::StepItem* pStep)
{
  std::string output = '"' + this->getStepInfo(pStep) + '"';
  mpConsole->appendPlainText(QString::fromUtf8(output.c_str()));
}

void cedar::proc::gui::CoPYWidget::importStepInformation(QList<QGraphicsItem*> pSteps){
  std::list<cedar::proc::gui::StepItem*> steps;
  for (auto item : pSteps)
  {
    if (auto step = dynamic_cast<cedar::proc::gui::StepItem*>(item))
    {
      steps.push_back(step);
    }
  }
  if(steps.size() > 1)
  {
    QStringList list;
    std::string sel = "selection" + std::to_string(++mSelCounter);
    bool sameType = true;
    for (cedar::proc::gui::StepItem *step : steps){
      list.append(QString::fromUtf8(this->getStepInfo(step).c_str()));
      step->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_SEARCH_RESULT);
    }
    std::string outputString;
    for (QString item : list) outputString += item.toStdString() + ", ";
    mpConsole->addVariable(QString(sel.c_str()), list);
    mpConsole->consoleMessage(QString::fromUtf8(("Selected Items were imported to CoPY as '" + sel + "'").c_str()),
                            false);
    mpConsole->executeCode("print("+ QString::fromStdString(sel) + ")");
    mpConsole->appendPlainText(QString::fromUtf8(sel.c_str()));
  }
  else
  {
    this->importStepInformation(steps.back());
  }
}

void cedar::proc::gui::CoPYWidget::appendToConsole(std::string text){
  mpConsole->appendPlainText(QString::fromStdString(text));
}

void cedar::proc::gui::CoPYWidget::reset()
{
  mpConsole -> reset();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

//!@brief main execute method for executing CoPY
void cedar::proc::gui::CoPYWidget::executeCode()
{
  mpConsole->executeLine();
  if(mpAutoResetCheckbox->isChecked()){
    this->reset();
  }
}

/*void cedar::proc::gui::CoPYWidget::updateDict()
{
  PyObject* dict = NULL;
  if (PyModule_Check(mpContext)) {
    dict = PyModule_GetDict(mpContext);
  } else if (PyDict_Check(mpContext)) {
    dict = mpContext;
    dict
}*/

std::string cedar::proc::gui::CoPYWidget::getStepInfo(cedar::proc::gui::StepItem* pStep){
  std::string group_name = pStep->getStep()->getGroup()->getName();
  if (group_name == "") group_name = "root";
  std::string step_name = pStep->getStep()->getName();
  std::string full_name = group_name + "." + step_name;
  std::cout << full_name << std::endl;
  return full_name;
}

void cedar::proc::gui::CoPYWidget::dragEnterEvent(QDragEnterEvent *p_event)
{
  p_event->acceptProposedAction();
}

void cedar::proc::gui::CoPYWidget::dragMoveEvent(QDragMoveEvent *p_event)
{
  p_event->acceptProposedAction();
}

void cedar::proc::gui::CoPYWidget::dropEvent(QDropEvent *pEvent)
{
  auto declaration = cedar::proc::gui::ElementList::declarationFromDrop(pEvent);

  if (auto elem_declaration = dynamic_cast<const cedar::proc::ElementDeclaration *>(declaration))
  {
    mpConsole->insertPlainText(QString(QString("\n#Usage: py.create(classId, xPos, yPos, amount)\npy.create('") + QString(elem_declaration->getClassName().c_str()) +  QString("', 0, 0, 1)\n")));
  } else if (auto group_declaration = dynamic_cast<const cedar::proc::GroupDeclaration *>(declaration))
  {
    mpConsole->insertPlainText(QString(QString("\n#Usage: py.createGroup(groupId, xPos, yPos)\npy.createGroup('") + QString(group_declaration->getClassName().c_str()) +  QString("', 0, 0)\n")));
  }
  pEvent->acceptProposedAction();
}

//SLOTS

void cedar::proc::gui::CoPYWidget::executeButtonClicked()
{
  this->executeCode();
}

void cedar::proc::gui::CoPYWidget::resetButtonClicked()
{
  this->reset();
}

void cedar::proc::gui::CoPYWidget::saveButtonClicked()
{
  this->reset();
}

void cedar::proc::gui::CoPYWidget::loadButtonClicked()
{
  this->reset();
}