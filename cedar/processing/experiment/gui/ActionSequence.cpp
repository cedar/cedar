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

    File:        ActionSequence.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 02 06

    Description: Source file for the class cedar::proc::experiment::gui::ActionSequence.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/gui/ActionSequence.h"
#include "cedar/processing/experiment/ActionSequence.h"
#include "cedar/processing/experiment/Experiment.h"
#include "cedar/processing/experiment/gui/ExperimentItemWidget.h"
#include "cedar/processing/gui/ExperimentDialog.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/gui/Parameter.h"

// SYSTEM INCLUDES
#include <QLineEdit>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::gui::ActionSequence::ActionSequence(
    cedar::proc::experiment::ActionSequencePtr sequence,
    cedar::proc::gui::ExperimentDialog* pParent
    )
:
QFrame(pParent),
mLayout(new QVBoxLayout()),
mCondition(NULL),
mSequence(sequence),
mpParent(pParent),
conditionRow(new QVBoxLayout),
mActions (new QVBoxLayout)
{
  this->setLayout(mLayout);
  this->conditionRow->setAlignment(Qt::AlignTop);
  this->setFrameShape(QFrame::StyledPanel);
  this->setFrameShadow(QFrame::Sunken);
  this->setAutoFillBackground(true);
  QPalette p = this->palette();
  p.setColor(backgroundRole(), QColor(212,208,200));
  this->setPalette(p);

  update();

}

cedar::proc::experiment::gui::ActionSequence::~ActionSequence()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::proc::experiment::gui::ActionSequence::remove()
{
  this->mpParent->getExperiment()->removeActionSequence(mSequence);
  delete this;
}

void cedar::proc::experiment::gui::ActionSequence::update()
{
  clear(mLayout);

  //Add Name Label and Remove Button
  QHBoxLayout* nameRow = new QHBoxLayout;
  cedar::aux::ParameterPtr nameParameter = this->mSequence->getParameter("name");
  cedar::aux::gui::Parameter* name
        = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(nameParameter)->allocateRaw();
  name->setParent(this);
  name->setParameter(nameParameter);
  QPushButton* rm = new QPushButton(QString::fromStdString("-"));
  connect(rm,SIGNAL(clicked()),this,SLOT(remove()));
  nameRow->addWidget(name);
  nameRow->addWidget(rm);
  mLayout->addLayout(nameRow);


  QVBoxLayout* body = new QVBoxLayout;

  //Add Condition
  QFrame* conditionBox = new QFrame;
  conditionBox->setLayout(conditionRow);
  conditionBox->setAutoFillBackground(true);
  QPalette p = conditionBox->palette();
  p.setColor(backgroundRole(), QColor(200,210,210));
  conditionBox->setPalette(p);
  body->addWidget(conditionBox);

  QLabel* condition_text = new QLabel(QString::fromStdString("Condition:"));
  conditionRow->addWidget(condition_text);

  try
  {
      cedar::aux::ParameterPtr condition = this->mSequence->getParameter("Condition");
      cedar::aux::gui::Parameter* conditionSelector
              = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(condition)->allocateRaw();
      conditionSelector->setParent(this);
      conditionSelector->setParameter(condition);
      conditionRow->addWidget(conditionSelector);
      connect(condition.get(),SIGNAL(valueChanged()),this,SLOT(updateCondition()));
      updateCondition();
  }
  catch (cedar::aux::UnknownTypeException& e)
  {

  }

  //Add Actions
  QLabel* actions_text = new QLabel(QString::fromStdString("Actions:"));
  QFrame* actionBox = new QFrame;
  QVBoxLayout* actionBoxLayout = new QVBoxLayout;
  actionBox->setLayout(actionBoxLayout);
  actionBoxLayout->addWidget(actions_text);
  QFrame* actionListBox = new QFrame();
  actionListBox->setLayout(mActions);
  actionListBox->setFrameShape(QFrame::Box);
  actionListBox->setFrameShadow(QFrame::Sunken);
  actionBoxLayout->addWidget(actionListBox);
  actionBox->setAutoFillBackground(true);
  p = actionBox->palette();
  p.setColor(backgroundRole(), QColor(230,230,210));
  actionBox->setPalette(p);
  body->addWidget(actionBox);
  try
  {
    cedar::aux::ParameterPtr action = this->mSequence->getParameter("ActionSet");
    cedar::aux::gui::Parameter* actionSelector
            = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(action)->allocateRaw();
    actionSelector->setParent(this);
    actionSelector->setParameter(action);
    actionBoxLayout->addWidget(actionSelector);
    connect(action.get(),SIGNAL(valueChanged()),this,SLOT(updateActions()));
  }
  catch (cedar::aux::UnknownTypeException& e)
  {

  }
  body->setMargin(0);
  mLayout->addLayout(body);

  updateActions();
}
void cedar::proc::experiment::gui::ActionSequence::updateCondition()
{
  delete mCondition;
  mCondition = new cedar::proc::experiment::gui::ExperimentItemWidget();
  mCondition->display(mSequence->getCondition());
  conditionRow->addWidget(mCondition);
}

void cedar::proc::experiment::gui::ActionSequence::updateActions()
{
  clear(mActions);

  for (cedar::proc::experiment::action::ActionPtr action : mSequence->getActions())
  {
    cedar::proc::experiment::gui::ExperimentItemWidget* actionWidget =
        new cedar::proc::experiment::gui::ExperimentItemWidget();
    actionWidget->display(action);
    QHBoxLayout* row = new QHBoxLayout();
    std::string name = cedar::proc::experiment::action::ActionManagerSingleton::getInstance()->getTypeId(action);
    row->addWidget(new QLabel(QString::fromStdString(name)));
    row->addWidget(actionWidget);
    mActions->addLayout(row);
  }
}

void cedar::proc::experiment::gui::ActionSequence::clear(QLayout* layout)
{
    QLayoutItem* child;
    while((child = layout->takeAt(0)) != 0)
    {
        if(child->layout() != 0)
        {
            clear(child->layout());
        }
        else if(child->widget() != 0)
        {
            delete child->widget();
        }

        delete child;
    }
}

