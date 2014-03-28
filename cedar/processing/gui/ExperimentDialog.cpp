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

    File:        ExperimentDialog.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.ruhr-uni-bochum.de
    Date:        2014 01 22

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ExperimentDialog.h"
#include "cedar/processing/gui/Network.h"
#include "cedar/processing/experiment/ActionSequence.h"
#include "cedar/processing/experiment/gui/ActionSequence.h"
#include "cedar/auxiliaries/gui/Parameter.h"
#include "cedar/auxiliaries/Parameter.h"

// SYSTEM INCLUDES
#include <QFileDialog>
#include <QMessageBox>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::gui::ExperimentDialog::ExperimentDialog(cedar::proc::gui::Ide* parent)
{
  mParent = parent;this->experiment = boost::shared_ptr<cedar::proc::experiment::Experiment>
  (
      new cedar::proc::experiment::Experiment(mParent->getNetwork()->getNetwork())
  );
  this->experiment->setName("TestExperiment");
  this->setupUi(this);
  connect(this->saveButton, SIGNAL(clicked()), this, SLOT(save()));
  connect(this->loadButton, SIGNAL(clicked()), this, SLOT(load()));
  connect(this->saveAsButton, SIGNAL(clicked()), this, SLOT(saveAs()));
  connect(this->nameEdit, SIGNAL(editingFinished()), this, SLOT(nameChanged()));
  connect(this->runButton, SIGNAL(toggled(bool)), this, SLOT(runExperiment(bool)));
  connect(this->experiment.get(), SIGNAL(experimentStopped(bool)), this, SLOT(experimentStopped(bool)));
  connect(this->experiment.get(), SIGNAL(trialNumberChanged(int)), this, SLOT(repetitionNumberChanged(int)));
  connect(this->repetitionSpinBox, SIGNAL(valueChanged(int)), this, SLOT(repetitionChanged()));
  connect(this->mAddActionSequence,SIGNAL(clicked()),this,SLOT(addActionSequence()));

  this->redraw();
}

cedar::proc::gui::ExperimentDialog::~ExperimentDialog()
{

}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::proc::gui::ExperimentDialog::save()
{
  std::string filename = this->experiment->getFileName();
  if (filename.empty())
  {
    this->saveAs();
  }
  else
  {
    this->experiment->writeJson(filename);
  }
}

void cedar::proc::gui::ExperimentDialog::saveAs()
{
  std::string old_file = this->experiment->getFileName();
  std::string filename = QFileDialog::getSaveFileName(this,tr("Save Experiment"),tr(old_file.c_str()),tr("Experiment Files (*.json)")).toStdString();
  if (!filename.empty())
  {
    this->experiment->setFileName(filename);
    this->experiment->writeJson(filename);
  }
}


void cedar::proc::gui::ExperimentDialog::load()
{
  std::string old_file = this->experiment->getFileName();
  std::string filename = QFileDialog::getOpenFileName(this,tr("Open Experiment"),tr(old_file.c_str()),tr("Experiment Files (*.json)")).toStdString();
  if (!filename.empty())
  {
    this->experiment->readJson(filename);
    this->experiment->setFileName(filename);
    this->nameEdit->setText(QString::fromStdString(this->experiment->getName()));
    this->repetitionSpinBox->setValue(this->experiment->getRepetitions());
  }
  this->redraw();
}


void cedar::proc::gui::ExperimentDialog::nameChanged()
{
  this->experiment->setName(nameEdit->text().toStdString());
}


void cedar::proc::gui::ExperimentDialog::repetitionChanged()
{
  this->experiment->setRepetitions(this->repetitionSpinBox->value());
}


void cedar::proc::gui::ExperimentDialog::clearActionSequences()
{
  QLayoutItem *child;
  while ((child = this->mActionSequences->takeAt(0)) != 0) {
    delete child->widget();
    delete child;
  }
}

void cedar::proc::gui::ExperimentDialog::paintEvent(QPaintEvent* pe)
{
  QWidget::paintEvent(pe);
  //this->redraw();
}

void cedar::proc::gui::ExperimentDialog::addActionSequence()
{
  cedar::proc::experiment::ActionSequencePtr action_seq
    = cedar::proc::experiment::ActionSequencePtr(new cedar::proc::experiment::ActionSequence());
  action_seq->setName("ActionSequence");
  this->experiment->addActionSequence(action_seq);
  this->redraw();
}

cedar::proc::experiment::ExperimentPtr cedar::proc::gui::ExperimentDialog::getExperiment()
{
  return this->experiment;
}

void cedar::proc::gui::ExperimentDialog::runExperiment(bool status)
{
  if (status)
  {
    this->experiment->run();
    this->runButton->setText(QString::fromStdString("Cancel"));
  }
  else
  {
    QMessageBox::StandardButton cancel = QMessageBox::warning(this,"Cancel Experiment",
        "Do you really want to cancel this experiment", QMessageBox::Yes|QMessageBox::No);
    if (cancel == QMessageBox::Yes)
    {
      this->experiment->cancel();
      experimentStopped(true);
    }
    else
    {
      this->runButton->setChecked(true);
    }
  }
}

void cedar::proc::gui::ExperimentDialog::experimentStopped(bool status)
{
  if(status)
  {
    disconnect(this->runButton, SIGNAL(toggled(bool)), this, SLOT(runExperiment(bool)));
    this->runButton->setText(QString::fromStdString("Run"));
    this->runButton->setChecked(false);
    connect(this->runButton, SIGNAL(toggled(bool)), this, SLOT(runExperiment(bool)));
  }
}

void cedar::proc::gui::ExperimentDialog::repetitionNumberChanged(int number)
{
  this->mActualRepetition->setText(QString::number(number));
}

void cedar::proc::gui::ExperimentDialog::redraw()
{
  this->clearActionSequences();
  std::vector<cedar::proc::experiment::ActionSequencePtr> action_sequences =
      this->experiment->getActionSequences();
  for(unsigned int i=0; i < action_sequences.size();i++)
  {
    cedar::proc::experiment::ActionSequencePtr action_seq =action_sequences[i];
    cedar::proc::experiment::gui::ActionSequence* as_gui
      = new cedar::proc::experiment::gui::ActionSequence(action_seq,this);
    this->mActionSequences->addWidget(as_gui);

/*
    cedar::aux::ParameterPtr condition = action_seq->getParameter("Condition");
    cedar::aux::gui::Parameter *p_widget
      = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(condition)->allocateRaw();
    p_widget->setParent(this);
    p_widget->setParameter(condition);
    this->mActionSequences->addWidget(p_widget);

    cedar::aux::ParameterPtr action = action_seq->getParameter("ActionSet");
        p_widget = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(action)->allocateRaw();
        p_widget->setParent(this);
        p_widget->setParameter(action);
        this->mActionSequences->addWidget(p_widget);
*/
  }
}
