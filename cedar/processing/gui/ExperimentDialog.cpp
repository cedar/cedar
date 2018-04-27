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
#include "cedar/processing/gui/Settings.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/experiment/ActionSequence.h"
#include "cedar/processing/experiment/gui/ActionSequence.h"
#include "cedar/auxiliaries/gui/Parameter.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/GlobalClock.h"
#include "cedar/units/Time.h"

// SYSTEM INCLUDES
#include <QFileDialog>
#include <QMessageBox>
#include <vector>
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::gui::ExperimentDialog::ExperimentDialog(cedar::proc::gui::Ide* parent)
{
  mParent = parent;
  //!@todo Can't this call createNewExperiment?
  this->setExperiment
  (
    cedar::proc::experiment::ExperimentPtr(new cedar::proc::experiment::Experiment(mParent->getGroup()->getGroup()))
  );
  this->mExperiment->setName("TestExperiment");
  this->setupUi(this);

  // Connect the GUI elements
  connect(this->mpNewButton, SIGNAL(clicked()), this, SLOT(createNewExperiment()));
  connect(this->saveButton, SIGNAL(clicked()), this, SLOT(save()));
  connect(this->loadButton, SIGNAL(clicked()), this, SLOT(load()));
  connect(this->saveAsButton, SIGNAL(clicked()), this, SLOT(saveAs()));
  connect(this->nameEdit, SIGNAL(editingFinished()), this, SLOT(nameChanged()));
  connect(this->runButton, SIGNAL(clicked()), this, SLOT(runExperiment()));
  connect(this->stopButton, SIGNAL(clicked()), this, SLOT(stopExperiment()));
  connect(this->repetitionSpinBox, SIGNAL(valueChanged(int)), this, SLOT(trialChanged()));
  connect(this->mAddActionSequence,SIGNAL(clicked()),this,SLOT(addActionSequence()));
  connect(this->mpRepeat, SIGNAL(toggled(bool)), this, SLOT(repeatChecked(bool)));

  //Time update
  mpGroupTime = new QTimer(this);
  connect(mpGroupTime, SIGNAL(timeout()), this, SLOT(timeUpdate()));
  mpGroupTime->start(100);

  scrollArea->setAutoFillBackground(true);
  QPalette p = scrollArea->palette();
  p.setColor(backgroundRole(), QColor(255,255,255));
  scrollArea->setPalette(p);

  this->redraw();
}

cedar::proc::gui::ExperimentDialog::~ExperimentDialog()
{
  if (this->mExperiment->isRunning())
  {
    this->mExperiment->stopExperiment();
  }
  mpGroupTime->stop();
  delete mpGroupTime;
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::ExperimentDialog::setExperiment(cedar::proc::experiment::ExperimentPtr experiment)
{
  if (this->mExperiment)
  {
    QObject::disconnect(this->mExperiment.get(), SIGNAL(experimentRunning(bool)), this, SLOT(experimentRunning(bool)));
    QObject::disconnect(this->mExperiment.get(), SIGNAL(trialNumberChanged(int)), this, SLOT(trialNumberChanged(int)));
    QObject::disconnect(this->mExperiment.get(), SIGNAL(groupChanged()), this, SLOT(redraw()));
  }

  this->mExperiment = experiment;
  QObject::connect(this->mExperiment.get(), SIGNAL(experimentRunning(bool)), this, SLOT(experimentRunning(bool)));
  QObject::connect(this->mExperiment.get(), SIGNAL(trialNumberChanged(int)), this, SLOT(trialNumberChanged(int)));
  QObject::connect(this->mExperiment.get(), SIGNAL(groupChanged()), this, SLOT(redraw()));
}

void cedar::proc::gui::ExperimentDialog::repeatChecked(bool checked)
{
  this->mExperiment->setRepeating(checked);
}

void cedar::proc::gui::ExperimentDialog::updateGroup()
{
  this->mExperiment->setGroup(mParent->getGroup()->getGroup());
}

void cedar::proc::gui::ExperimentDialog::save()
{
  std::string filename = this->mExperiment->getFileName();
  if (filename.empty())
  {
    this->saveAs();
  }
  else
  {
    this->mExperiment->writeJson(filename);
  }
}

void cedar::proc::gui::ExperimentDialog::saveAs()
{
  auto location_dir = cedar::proc::gui::SettingsSingleton::getInstance()->getExperimentDialogDirectory();
  QString filter = tr("Experiment Files (*.json)");
  std::string filename = QFileDialog::getSaveFileName
      (
        this,
        tr("Save Experiment"),
        location_dir->getValue().absolutePath(),
        filter
      ).toStdString();
  if (!filename.empty())
  {
    if (!cedar::aux::endsWith(filename, ".json"))
    {
      filename += ".json";
    }
    this->mExperiment->setFileName(filename);
    this->mExperiment->writeJson(filename);
    location_dir->setValue(QDir(QString::fromStdString(filename)));
  }
}


void cedar::proc::gui::ExperimentDialog::load()
{
  this->updateGroup();
  auto location_dir = cedar::proc::gui::SettingsSingleton::getInstance()->getExperimentDialogDirectory();
  QString filter = tr("Experiment Files (*.json)");
  std::string filename = QFileDialog::getOpenFileName
          (
            this,
            tr("Open Experiment"),
            location_dir->getValue().absolutePath(),
            filter,
            0,
            // js: Workaround for freezing file dialogs in QT5 (?)
            QFileDialog::DontUseNativeDialog
          ).toStdString();
  if (!filename.empty())
  {
    location_dir->setValue(QDir(QString::fromStdString(filename)));
    //!@todo Shouldn't this create a new experiment?
    this->mExperiment->readJson(filename);
    this->mExperiment->setFileName(filename);
    this->mpRepeat->setChecked(this->mExperiment->getRepeating());
    this->nameEdit->setText(QString::fromStdString(this->mExperiment->getName()));
    this->repetitionSpinBox->setValue(this->mExperiment->getTrialCount());
  }
  this->redraw();
}


void cedar::proc::gui::ExperimentDialog::nameChanged()
{
  this->mExperiment->setName(nameEdit->text().toStdString());
}


void cedar::proc::gui::ExperimentDialog::trialChanged()
{
  this->mExperiment->setTrialCount(this->repetitionSpinBox->value());
}


void cedar::proc::gui::ExperimentDialog::clearActionSequences()
{
  QLayoutItem* child;
  while ((child = this->mActionSequences->takeAt(0)) != 0)
  {
    delete child->widget();
    delete child;
  }
}

void cedar::proc::gui::ExperimentDialog::addActionSequence()
{
  //Todo: The Sequence should be constructed in the non-gui Class!
  cedar::proc::experiment::ActionSequencePtr action_seq
    = cedar::proc::experiment::ActionSequencePtr(new cedar::proc::experiment::ActionSequence(this->getExperiment()));
  action_seq->setName("ActionSequence");
  this->mExperiment->addActionSequence(action_seq);
  this->redraw();
}

cedar::proc::experiment::ExperimentPtr cedar::proc::gui::ExperimentDialog::getExperiment()
{
  return this->mExperiment;
}

void cedar::proc::gui::ExperimentDialog::runExperiment()
{
  std::map<std::string, std::vector<std::string> > issues;
  issues["errors"] = std::vector<std::string>();
  issues["warnings"] = std::vector<std::string>();

  bool valid = this->mExperiment->checkValidity(issues["errors"], issues["warnings"]);

  QMessageBox::Icon icon;
  if (valid)
  {
    icon = QMessageBox::Warning;
  }
  else
  {
    icon = QMessageBox::Critical;
  }

  QString details;

  bool issues_found = false;
  for (const auto& issue_list_pair : issues)
  {
    const auto& list = issue_list_pair.second;
    if (!list.empty())
    {
      issues_found = true;
      QString issue = QString::fromStdString(issue_list_pair.first);

      details += "The following ";
      details += issue;
      details += " occurred:\n\n";
      for (const auto& item : list)
      {
        details += QString::fromStdString(item) + "\n";
      }
    }
  }

  if (issues_found)
  {
    auto p_mb = new QMessageBox
                (
                  icon,
                  "Issues with the experiment",
                  "Some parts of the experiment are not set up properly. Start anyway?",
                  QMessageBox::Yes | QMessageBox::No
                );

    p_mb->setDetailedText(details);

    int r = p_mb->exec();

    delete p_mb;

    if (r == QMessageBox::No)
    {
      return;
    }
  }


  this->mExperiment->startExperiment();
}

void cedar::proc::gui::ExperimentDialog::stopExperiment()
{
  QMessageBox::StandardButton stop = QMessageBox::warning(this,"Cancel Experiment",
      "Do you really want to stop this experiment?", QMessageBox::Yes|QMessageBox::No);
  if (stop == QMessageBox::Yes)
  {
    this->mExperiment->stopExperiment();
  }
}


void cedar::proc::gui::ExperimentDialog::experimentRunning(bool status)
{
  if (status)
  {
    this->runButton->setEnabled(false);
    this->stopButton->setEnabled(true);
    this->setActionSequenceWidgetsEnabled(false);
    this->mStatusText->setText(QString::fromStdString("Running"));
  }
  else
  {
    this->runButton->setEnabled(true);
    this->stopButton->setEnabled(false);
    this->setActionSequenceWidgetsEnabled(true);
    this->mStatusText->setText(QString::fromStdString("Stopped"));
  }
}

void cedar::proc::gui::ExperimentDialog::setActionSequenceWidgetsEnabled(bool enabled)
{
  for (int i = 0; i < this->mActionSequences->count(); ++i)
  {
    auto child = this->mActionSequences->itemAt(i)->widget();
    if (auto action_sequence_widget = dynamic_cast<cedar::proc::experiment::gui::ActionSequence*>(child))
    {
      action_sequence_widget->setEnabled(enabled);
    }
  }
}


void cedar::proc::gui::ExperimentDialog::trialNumberChanged(int number)
{
  this->mActualRepetition->setText(QString::number(number+1));
}

void cedar::proc::gui::ExperimentDialog::redraw()
{
  this->clearActionSequences();
  std::vector<cedar::proc::experiment::ActionSequencePtr> action_sequences =
      this->mExperiment->getActionSequences();
  for (unsigned int i = 0; i < action_sequences.size(); i++)
  {
    cedar::proc::experiment::ActionSequencePtr action_seq = action_sequences[i];
    cedar::proc::experiment::gui::ActionSequence* as_gui
      = new cedar::proc::experiment::gui::ActionSequence(action_seq,this);
    this->mActionSequences->addWidget(as_gui);
  }
}

void cedar::proc::gui::ExperimentDialog::timeUpdate()
{
  cedar::unit::Time time = cedar::aux::GlobalClockSingleton::getInstance()->getTime();
  std::string formatted_time = cedar::aux::formatDuration(time);
  this->mTimeLabel->setText(QString::fromStdString(formatted_time));
}

void cedar::proc::gui::ExperimentDialog::createNewExperiment()
{
  auto r = QMessageBox::question
      (
        this,
        "New experiment?",
        "Do you want to discard the old experiment and create a new one?",
        QMessageBox::Ok | QMessageBox::Cancel
      );

  switch (r)
  {
    case QMessageBox::Cancel:
      return;

    case QMessageBox::Ok:
      break;

    default:
      // this should not happen, only Ok and Cancel are in the dialog
      CEDAR_ASSERT(false);
  }

  if (this->mExperiment->isRunning())
  {
    this->mExperiment->stopExperiment();
  }
  this->setExperiment
  (
    cedar::proc::experiment::ExperimentPtr(new cedar::proc::experiment::Experiment(mParent->getGroup()->getGroup()))
  );
  this->mExperiment->setName("new experiment");
  this->mExperiment->setFileName("new experiment.json");
  this->mpRepeat->setChecked(this->mExperiment->getRepeating());
  this->nameEdit->setText(QString::fromStdString(this->mExperiment->getName()));
  this->repetitionSpinBox->setValue(this->mExperiment->getTrialCount());
  this->redraw();
}
