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

    File:        TriggerParameter.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 05 10

    Description: Source file for the class cedar::proc::experiment::gui::TriggerParameter.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/gui/TriggerParameter.h"
#include "cedar/processing/experiment/TriggerParameter.h"
#include "cedar/processing/experiment/Supervisor.h"

// SYSTEM INCLUDES
#include <QBoxLayout>
#include <QLabel>
#include <string>
#include <vector>


//----------------------------------------------------------------------------------------------------------------------
// associate aux::gui parameter with the aux parameter
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered = cedar::aux::gui::ParameterFactorySingleton::getInstance()->add
      <
        cedar::proc::experiment::TriggerParameter,
        cedar::proc::experiment::gui::TriggerParameter
      >();
}


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::gui::TriggerParameter::TriggerParameter(QWidget *pParent )
:
cedar::aux::gui::Parameter(pParent)
,
mpTrigger(new QComboBox)
{
  this->setLayout(new QHBoxLayout);
  this->layout()->setMargin(0);
  this->layout()->addWidget(new QLabel(QString::fromStdString("Trigger: ")));
  this->layout()->addWidget(mpTrigger);
  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
}

cedar::proc::experiment::gui::TriggerParameter::~TriggerParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::proc::experiment::gui::TriggerParameter::parameterPointerChanged()
{
  updateTriggers();
  cedar::proc::experiment::TriggerParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::proc::experiment::TriggerParameter>(this->getParameter());
  this->mpTrigger->setCurrentIndex(this->mpTrigger->findText((parameter->getTrigger().c_str())));
  connect(this->mpTrigger, SIGNAL(currentIndexChanged(int)), this, SLOT(triggerChanged()));
}

void cedar::proc::experiment::gui::TriggerParameter::triggerChanged()
{

  QString text = mpTrigger->currentText();
  cedar::proc::experiment::TriggerParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::proc::experiment::TriggerParameter>(this->getParameter());
  parameter->setTrigger(text.toStdString());
}

void cedar::proc::experiment::gui::TriggerParameter::updateTriggers()
{

  mpTrigger->clear();
  std::vector<std::string> triggers = SupervisorSingleton::getInstance()->getExperiment()->getGroupTriggers();
  for (std::string trigger : triggers)
  {
    mpTrigger->addItem(QString::fromStdString(trigger));
  }
}
