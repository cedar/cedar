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


#include "ElementList.h"

// SYSTEM INCLUDES
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::CoPYWidget::CoPYWidget(QWidget* pParent)
:
QWidget(pParent)
{
  QVBoxLayout *layout = new QVBoxLayout(reinterpret_cast<QWidget *>(this));
  layout->setMargin(0);

  //init PythonQt with Console
  PythonQt::init(PythonQt::RedirectStdOut);
  context = PythonQt::self()->getMainModule();
  console = new PythonQtConsole(NULL, context);

  //finalize widget and button
  layout->addWidget(console);
  layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  QPushButton *mpExecuteButton = new QPushButton("Execute");
  QObject::connect(mpExecuteButton, SIGNAL (clicked()), this, SLOT(executeButtonClicked()));
  layout->addWidget(mpExecuteButton);

  setAcceptDrops(true);
  //appendHighlighter to Editor
  this->mpHighlighter = new cedar::proc::gui::CodeWidgetScope::PythonSyntaxHighlighter(console->document());

  QObject::connect(static_cast<cedar::proc::gui::CodeWidgetScope::CodeEditor*>(console), SIGNAL(textChanged()), this, SLOT(updateCodeString()));

  qRegisterMetaType<CoPYObject>("CoPYObject");
  PythonQt::self()->registerCPPClass("CoPYObject", "", "copy", PythonQtCreateObject<CoPYObjectWrapper>);

  pyWrap = new CoPYObjectWrapper(2);
  context.addObject("py", pyWrap);
  mPy = pyWrap -> getO();

  context.evalScript("from PythonQt.copy import CoPYObject\npy = py.object()\n");

}

cedar::proc::gui::CoPYWidget::~CoPYWidget()
{

}

//----------------------------------------------------------------------------------------------------------------------
// Set up methods
//----------------------------------------------------------------------------------------------------------------------

//!@brief set Scene out of cedar::proc::gui::Ide.cpp
void cedar::proc::gui::CoPYWidget::setScene(cedar::proc::gui::Scene* pScene)
{
  this->mpScene = pScene;
  mPy->setScene(this->mpScene);
  mPy->setGroup(pScene->getRootGroup()->getGroup());
  console->setScene(this->mpScene);
}

void cedar::proc::gui::CoPYWidget::reset()
{
  //init Python CoPYObject and cast it to CPPObject


}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

//!@brief main execute method for executing CoPY
void cedar::proc::gui::CoPYWidget::executeCode()
{
  console->executeLine();
  //this->reset();
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
    console->insertPlainText(QString(QString("\n#Usage: py.create(classId, xPos, yPos, amount)\npy.create('") + QString(elem_declaration->getClassName().c_str()) +  QString("', 0, 0, 1)\n")));
  } else if (auto group_declaration = dynamic_cast<const cedar::proc::GroupDeclaration *>(declaration))
  {
    console->insertPlainText(QString(QString("\n#Usage: py.createGroup(groupId, xPos, yPos)\npy.createGroup('") + QString(group_declaration->getClassName().c_str()) +  QString("', 0, 0)\n")));
  }
  pEvent->acceptProposedAction();
}

//SLOTS

void cedar::proc::gui::CoPYWidget::executeButtonClicked()
{
  this->executeCode();
}

void cedar::proc::gui::CoPYWidget::updateCodeString()
{

}
