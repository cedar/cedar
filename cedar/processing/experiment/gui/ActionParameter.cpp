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

    File:        ActionParameter.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2015 01 19

    Description: Source file for the class cedar::proc::experiment::gui::ActionParameter.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/gui/ActionParameter.h"
#include "cedar/processing/experiment/ActionParameter.h"
#include "cedar/processing/experiment/Supervisor.h"
#include "cedar/auxiliaries/TypeBasedFactory.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/gui/Parameter.h"

// SYSTEM INCLUDES
#include <QFormLayout>
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
        cedar::proc::experiment::ActionParameter,
        cedar::proc::experiment::gui::ActionParameter
      >();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::gui::ActionParameter::ActionParameter(QWidget* pParent)
:
cedar::aux::gui::Parameter(pParent),
mpActionComboBox(new QComboBox())
{
  this->mpActionComboBox->setEditable(true);

  auto layout = new QFormLayout();
  layout->setMargin(0);
  this->setLayout(layout);
  layout->addRow(mpActionComboBox);
  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
}

cedar::proc::experiment::gui::ActionParameter::~ActionParameter()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::experiment::gui::ActionParameter::connectableChanged()
{
  auto parameter = boost::dynamic_pointer_cast<cedar::proc::experiment::ActionParameter>(this->getParameter());
  parameter->setAction(this->mpActionComboBox->currentText().toStdString());
}

void cedar::proc::experiment::gui::ActionParameter::parameterPointerChanged()
{
  auto parameter = boost::dynamic_pointer_cast<cedar::proc::experiment::ActionParameter>(this->getParameter());
  QObject::connect(parameter.get(), SIGNAL(actionListChanged()), this, SLOT(updateActions()));
  QObject::connect(parameter.get(), SIGNAL(valueChanged()), this, SLOT(actionListChanged()));
  QObject::connect(mpActionComboBox, SIGNAL(editTextChanged(const QString&)), this, SLOT(actionChanged()));
  this->updateActions();
}

void cedar::proc::experiment::gui::ActionParameter::updateActions()
{
  mpActionComboBox->clear();
  auto parameter = boost::dynamic_pointer_cast<cedar::proc::experiment::ActionParameter>(this->getParameter());
  auto actions = parameter->getListOfActions();
  bool blocked = mpActionComboBox->blockSignals(true);
  for (std::string action : actions)
  {
    mpActionComboBox->addItem(QString::fromStdString(action));
  }
  mpActionComboBox->blockSignals(blocked);
  this->actionListChanged();
}

void cedar::proc::experiment::gui::ActionParameter::actionListChanged()
{
  auto parameter = boost::dynamic_pointer_cast<cedar::proc::experiment::ActionParameter>(this->getParameter());
  auto action = parameter->getAction();
  mpActionComboBox->setEditText(QString::fromStdString(action));
}

void cedar::proc::experiment::gui::ActionParameter::actionChanged()
{
  auto parameter = boost::dynamic_pointer_cast<cedar::proc::experiment::ActionParameter>(this->getParameter());
  parameter->setAction(mpActionComboBox->currentText().toStdString());
}
