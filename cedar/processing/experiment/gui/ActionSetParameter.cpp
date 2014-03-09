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

    File:        ActionSetParameter.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 03 07

    Description: Source file for the class cedar::proc::experiment::gui::ActionSetParameter.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/gui/ActionSetParameter.h"
#include "cedar/processing/experiment/ActionSetParameter.h"
#include "cedar/auxiliaries/gui/Parameter.h"
#include "cedar/processing/experiment/ExperimentController.h"

// SYSTEM INCLUDES
#include <QLabel>

//----------------------------------------------------------------------------------------------------------------------
// associate experiment::gui::Action with the experiment::Action
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered = cedar::proc::experiment::gui::ActionFactorySingleton::getInstance()->add
      <
        cedar::proc::experiment::ActionSetParameter,
        cedar::proc::experiment::gui::ActionSetParameter
      >();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::gui::ActionSetParameter::ActionSetParameter()
{
}

cedar::proc::experiment::gui::ActionSetParameter::~ActionSetParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::experiment::gui::ActionSetParameter::redraw()
{
  mStepSelector = new QComboBox;
  mLayout->addWidget(new QLabel(QString::fromStdString("Set Step:")));
  mLayout->addWidget(mStepSelector);
  mParameterSelector = new QComboBox;
  mLayout->addWidget(new QLabel(QString::fromStdString("Parameter")));
  mLayout->addWidget(mParameterSelector);
  mLayout->addWidget(new QLabel(QString::fromStdString("to")));
  if(mAction)
  {
    //get desired Value
    cedar::aux::ParameterPtr desiredValue = mAction->getParameter("DesiredValue");
    cedar::aux::gui::Parameter* desiredValueWidget =cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(desiredValue)->allocateRaw();
    desiredValueWidget->setParameter(desiredValue);
    desiredValueWidget->setParent(this);
    mLayout->addWidget(desiredValueWidget);

    //update selctor values
   updateSteps();
   cedar::aux::ParameterPtr stepString = mAction->getParameter("StepToSet");
   if (cedar::aux::StringParameterPtr  stepParameter = boost::dynamic_pointer_cast<cedar::aux::StringParameter>(stepString))
   {
     mStepSelector->setCurrentIndex(mStepSelector->findText(QString::fromStdString(stepParameter->getValue())));
   }
   updateParameters();
   cedar::aux::ParameterPtr parameterString = mAction->getParameter("ParameterToSet");
   if (cedar::aux::StringParameterPtr  parameterParameter = boost::dynamic_pointer_cast<cedar::aux::StringParameter>(parameterString))
   {
     mParameterSelector->setCurrentIndex(mParameterSelector->findText(QString::fromStdString(parameterParameter->getValue())));
   }
  }
  connect(mStepSelector,SIGNAL(currentIndexChanged(int)),this,SLOT(stepSelected()));
  connect(mParameterSelector,SIGNAL(currentIndexChanged(int)),this,SLOT(parameterSelected()));
}

void cedar::proc::experiment::gui::ActionSetParameter::updateSteps()
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
void cedar::proc::experiment::gui::ActionSetParameter::updateParameters()
{
  std::string index = mStepSelector->currentText().toStdString();
  if (index == "")
    return;
  std::vector<std::string> parameters = ExperimentControllerSingleton::getInstance()->getExperiment()->getStepParameters(index);
   if (mParameterSelector->count() > 0)
   {
     mParameterSelector->clear();
   }
   for (std::string parameter : parameters)
   {
     mParameterSelector->addItem(QString::fromStdString(parameter));
   }
}

void cedar::proc::experiment::gui::ActionSetParameter::stepSelected()
{
  std::string index = mStepSelector->currentText().toStdString();
  if (index == "" || !mAction)
    return;

  cedar::aux::ParameterPtr parameter = mAction->getParameter("StepToSet");
  if (cedar::aux::StringParameterPtr  stepParameter = boost::dynamic_pointer_cast<cedar::aux::StringParameter>(parameter))
  {
      stepParameter->setValue(index);
  }
  updateParameters();


}

void cedar::proc::experiment::gui::ActionSetParameter::parameterSelected()
{
  std::string index = mParameterSelector->currentText().toStdString();
  if (index == "" ||  !mAction)
    return;

  cedar::aux::ParameterPtr parameter = mAction->getParameter("ParameterToSet");
  if (cedar::aux::StringParameterPtr  stepParameter = boost::dynamic_pointer_cast<cedar::aux::StringParameter>(parameter))
  {
    stepParameter->setValue(index);
  }
}
