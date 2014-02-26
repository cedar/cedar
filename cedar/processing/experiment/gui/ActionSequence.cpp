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
#include "cedar/processing/gui/ExperimentDialog.h"
#include "cedar/auxiliaries/gui/Parameter.h"
#include "cedar/auxiliaries/Parameter.h"

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
QWidget(pParent)
{
  this->mSequence=sequence;
  this->mpParent=pParent;
  this->mLayout = new QVBoxLayout;
  this->setLayout(mLayout);

  //Add Name Label and Remove Button
  QHBoxLayout* nameRow = new QHBoxLayout;
  QLineEdit* name = new QLineEdit();
  name->setText(QString::fromStdString(sequence->getName()));
  QPushButton* rm = new QPushButton(QString::fromStdString("-"));
  connect(rm,SIGNAL(clicked()),this,SLOT(remove()));
  nameRow->addWidget(name);
  nameRow->addWidget(rm);
  mLayout->addLayout(nameRow);

  //Add Condition
  QHBoxLayout* conditionRow = new QHBoxLayout;
  QLabel* condition_text = new QLabel(QString::fromStdString("Condition:"));

  cedar::aux::ParameterPtr condition = this->mSequence->getParameter("Condition");
  cedar::aux::gui::Parameter* conditions
    = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(condition)->allocateRaw();
  conditions->setParent(this);
  conditions->setParameter(condition);
  mCondition = new QHBoxLayout;
  conditionRow->addWidget(condition_text);
  conditionRow->addWidget(conditions);
  conditionRow->addLayout(mCondition);

  //Add Actions
  QLabel* actions_text = new QLabel(QString::fromStdString("Actions:"));
  mLayout->addLayout(conditionRow);
  mActions = new QVBoxLayout;
  QPushButton* add_action = new QPushButton(QString::fromStdString("add action"));
  mLayout->addWidget(actions_text);
  mLayout->addLayout(mActions);
  mLayout->addWidget(add_action);

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
