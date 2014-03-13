/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        StepPropertyParameter.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 03 13

    Description: Source file for the class cedar::proc::experiment::gui::StepPropertyParameter.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/gui/StepPropertyParameter.h"
#include "cedar/processing/experiment/StepPropertyParameter.h"
#include "cedar/processing/experiment/ExperimentController.h"
#include "cedar/auxiliaries/TypeBasedFactory.h"
#include "cedar/auxiliaries/Singleton.h"

// SYSTEM INCLUDES
#include <QLabel>


//----------------------------------------------------------------------------------------------------------------------
// associate aux::gui parameter with the aux parameter
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered = cedar::aux::gui::ParameterFactorySingleton::getInstance()->add
      <
        cedar::proc::experiment::StepPropertyParameter,
        cedar::proc::experiment::gui::StepPropertyParameter
      >();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::gui::StepPropertyParameter::StepPropertyParameter(QWidget *pParent )
:
cedar::aux::gui::Parameter(pParent)
{
  mpStep = new QComboBox;
  mpProperty = new QComboBox;
  QVBoxLayout* layout = new QVBoxLayout;
  this->setLayout(layout);
  QHBoxLayout* step_row = new QHBoxLayout;
  step_row->addWidget(new QLabel(QString::fromStdString("Step: ")));
  step_row->addWidget(mpStep);
  QHBoxLayout* property_row = new QHBoxLayout;
  property_row->addWidget(new QLabel(QString::fromStdString("Property: ")));
  property_row->addWidget(mpProperty);

  layout->addLayout(step_row);
  layout->addLayout(property_row);
  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
}

cedar::proc::experiment::gui::StepPropertyParameter::~StepPropertyParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::experiment::gui::StepPropertyParameter::parameterPointerChanged()
{
  updateSteps();
  boost::intrusive_ptr<cedar::proc::experiment::StepPropertyParameter> parameter;
  parameter = boost::dynamic_pointer_cast<cedar::proc::experiment::StepPropertyParameter>(this->getParameter());
  this->mpStep->setCurrentIndex(this->mpStep->findText((parameter->getStep().c_str())));
  updateProperties();
  this->mpProperty->setCurrentIndex(this->mpStep->findText((parameter->getProperty().c_str())));
  connect(this->mpStep, SIGNAL(currentIndexChanged(int)), this, SLOT(stepChanged()));
  connect(this->mpProperty, SIGNAL(currentIndexChanged(int)), this, SLOT(propertyChanged()));

}

void cedar::proc::experiment::gui::StepPropertyParameter::stepChanged()
{
  QString text = mpStep->currentText();
  boost::intrusive_ptr<cedar::proc::experiment::StepPropertyParameter> parameter;
  parameter = boost::dynamic_pointer_cast<cedar::proc::experiment::StepPropertyParameter>(this->getParameter());
  parameter->setStep(text.toStdString());

  updateProperties();
}

void cedar::proc::experiment::gui::StepPropertyParameter::propertyChanged()
{

  cedar::aux::ParameterPtr hhhhh = this->getParameter();

  QString text = mpProperty->currentText();
  boost::intrusive_ptr<cedar::proc::experiment::StepPropertyParameter> parameter;
  parameter = boost::dynamic_pointer_cast<cedar::proc::experiment::StepPropertyParameter>(this->getParameter());
  parameter->setProperty(text.toStdString());

}

void cedar::proc::experiment::gui::StepPropertyParameter::updateSteps()
{
 mpStep->clear();
 std::vector<std::string> steps = ExperimentControllerSingleton::getInstance()->getExperiment()->getAllSteps();
 for (std::string step : steps)
  {
   mpStep->addItem(QString::fromStdString(step));
  }
}

void cedar::proc::experiment::gui::StepPropertyParameter::updateProperties()
{
  std::string index = mpStep->currentText().toStdString();
  if (index == "")
     return;
  std::vector<std::string> properties = ExperimentControllerSingleton::getInstance()->getExperiment()->getStepParameters(index);
  mpProperty->clear();
  for (std::string property : properties)
  {
    mpProperty->addItem(QString::fromStdString(property));
  }
}
