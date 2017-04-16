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

    File:        ConnectableParameter.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2015 01 16

    Description: Source file for the class cedar::proc::experiment::gui::ConnectableParameter.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/gui/ConnectableParameter.h"
#include "cedar/processing/experiment/ConnectableParameter.h"
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
        cedar::proc::experiment::ConnectableParameter,
        cedar::proc::experiment::gui::ConnectableParameter
      >();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::gui::ConnectableParameter::ConnectableParameter(QWidget* pParent)
:
cedar::aux::gui::Parameter(pParent),
mpConnectableComboBox(new QComboBox())
{
  this->mpConnectableComboBox->setEditable(true);

  auto layout = new QFormLayout();
  layout->setMargin(0);
  this->setLayout(layout);
  layout->addRow(mpConnectableComboBox);
  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
}

cedar::proc::experiment::gui::ConnectableParameter::~ConnectableParameter()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::experiment::gui::ConnectableParameter::connectableChanged()
{
  auto parameter = boost::dynamic_pointer_cast<cedar::proc::experiment::ConnectableParameter>(this->getParameter());
  parameter->setConnectablePath(this->mpConnectableComboBox->currentText().toStdString());
  emit connectableChangedSignal();
}

void cedar::proc::experiment::gui::ConnectableParameter::updateConnectables()
{
  mpConnectableComboBox->clear();
  std::vector<std::string> steps = SupervisorSingleton::getInstance()->getExperiment()->getGroupSteps();
  for (std::string step : steps)
  {
    mpConnectableComboBox->addItem(QString::fromStdString(step));
  }
}

void cedar::proc::experiment::gui::ConnectableParameter::parameterPointerChanged()
{
  updateConnectables();
  auto parameter = boost::dynamic_pointer_cast<cedar::proc::experiment::ConnectableParameter>(this->getParameter());
  if (parameter)
  {
    auto path = parameter->getConnectablePath();
    this->mpConnectableComboBox->setEditText(QString::fromStdString(path));
  }
  this->connectableChanged();
  connect(this->mpConnectableComboBox, SIGNAL(editTextChanged(const QString&)), this, SLOT(connectableChanged()));
}
