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

    File:        ConditionCheckValue.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 02 26

    Description: Source file for the class cedar::proc::experiment::gui::ConditionCheckValue.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/gui/ConditionCheckValue.h"
#include "cedar/processing/experiment/ExperimentController.h"
#include "cedar/processing/experiment/Experiment.h"
#include "cedar/auxiliaries/gui/Parameter.h"

// SYSTEM INCLUDES
#include "cedar/processing/experiment/ConditionCheckValue.h"
#include <QLabel>

//----------------------------------------------------------------------------------------------------------------------
// associate experiment::gui::Condition with the experiment::Condition
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered = cedar::proc::experiment::gui::ConditionFactorySingleton::getInstance()->add
      <
        cedar::proc::experiment::ConditionCheckValue,
        cedar::proc::experiment::gui::ConditionCheckValue
      >();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::gui::ConditionCheckValue::ConditionCheckValue()
{
  update();
}

cedar::proc::experiment::gui::ConditionCheckValue::~ConditionCheckValue()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::experiment::gui::ConditionCheckValue::redraw()
{
  mStepSelector = new QComboBox;
  mLayout->addWidget(new QLabel(QString::fromStdString("If Step:")));
  mLayout->addWidget(mStepSelector);
  mParameterSelector = new QComboBox;
  mLayout->addWidget(new QLabel(QString::fromStdString("Parameter")));
  mLayout->addWidget(mParameterSelector);
  mLayout->addWidget(new QLabel(QString::fromStdString(">")));
  if(mCondition)
  {
    //get desired Value
    cedar::aux::ParameterPtr desiredValue = mCondition->getParameter("DesiredValue");
    cedar::aux::gui::Parameter* desiredValueWidget =cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(desiredValue)->allocateRaw();
    desiredValueWidget->setParameter(desiredValue);
    desiredValueWidget->setParent(this);
    mLayout->addWidget(desiredValueWidget);

    //update selctor values
   updateSteps();
   cedar::aux::ParameterPtr stepString = mCondition->getParameter("StepToCheck");
   if (cedar::aux::StringParameterPtr  stepParameter = boost::dynamic_pointer_cast<cedar::aux::StringParameter>(stepString))
   {
     mStepSelector->setCurrentIndex(mStepSelector->findText(QString::fromStdString(stepParameter->getValue())));
   }
   updateParameters();
   cedar::aux::ParameterPtr parameterString = mCondition->getParameter("ParameterToCheck");
   if (cedar::aux::StringParameterPtr  parameterParameter = boost::dynamic_pointer_cast<cedar::aux::StringParameter>(parameterString))
   {
     mParameterSelector->setCurrentIndex(mParameterSelector->findText(QString::fromStdString(parameterParameter->getValue())));
   }
  }
  connect(mStepSelector,SIGNAL(currentIndexChanged(int)),this,SLOT(stepSelected()));
  connect(mParameterSelector,SIGNAL(currentIndexChanged(int)),this,SLOT(parameterSelected()));
}

void cedar::proc::experiment::gui::ConditionCheckValue::updateSteps()
{
  if (mStepSelector->count() > 0)
  {
    mStepSelector->clear();
  }
  std::vector<std::string> steps = ExperimentControllerSingleton::getInstance()->getExperiment()->getAllSteps();
  for (std::string step : steps)
  {
    mStepSelector->addItem(QString::fromStdString(step));
  }
}
void cedar::proc::experiment::gui::ConditionCheckValue::updateParameters()
{
  std::string index = mStepSelector->currentText().toStdString();
  if (index == "")
    return;
  std::vector<std::string> parameters = ExperimentControllerSingleton::getInstance()->getExperiment()->getStepValues(index);
   if (mParameterSelector->count() > 0)
   {
     mParameterSelector->clear();
   }
   for (std::string parameter : parameters)
   {
     mParameterSelector->addItem(QString::fromStdString(parameter));
   }
}

void cedar::proc::experiment::gui::ConditionCheckValue::stepSelected()
{
  std::string index = mStepSelector->currentText().toStdString();
  if (index == "" || !mCondition)
    return;

  cedar::aux::ParameterPtr parameter = mCondition->getParameter("StepToCheck");
  if (cedar::aux::StringParameterPtr  stepParameter = boost::dynamic_pointer_cast<cedar::aux::StringParameter>(parameter))
  {
      stepParameter->setValue(index);
  }
  updateParameters();


}

void cedar::proc::experiment::gui::ConditionCheckValue::parameterSelected()
{
  std::string index = mParameterSelector->currentText().toStdString();
  if (index == "" ||  !mCondition)
    return;

  cedar::aux::ParameterPtr parameter = mCondition->getParameter("ParameterToCheck");
  if (cedar::aux::StringParameterPtr  stepParameter = boost::dynamic_pointer_cast<cedar::aux::StringParameter>(parameter))
  {
    stepParameter->setValue(index);
  }
}
