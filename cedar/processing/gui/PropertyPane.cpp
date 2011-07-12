/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        PropertyPane.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 03 09

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "processing/gui/PropertyPane.h"
#include "processing/Step.h"
#include "auxiliaries/namespace.h"
#include "auxiliaries/Parameter.h"
#include "auxiliaries/NumericParameter.h"
#include "processing/gui/BoolParameter.h"
#include "processing/gui/DoubleParameter.h"
#include "processing/gui/StringParameter.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QLabel>
#include <QLineEdit>
#include <QDoubleSpinBox>


cedar::proc::gui::PropertyPane::DataWidgetTypes cedar::proc::gui::PropertyPane::mDataWidgetTypes;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::PropertyPane::PropertyPane(QWidget *pParent)
:
QTableWidget(pParent)
{
}

cedar::proc::gui::PropertyPane::~PropertyPane()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::PropertyPane::display(cedar::proc::StepPtr pStep)
{
  int row = this->rowCount();
  cedar::proc::Step::ParameterMap& parameters = pStep->getParameters();
  for (Step::ParameterMap::iterator iter = parameters.begin(); iter != parameters.end(); ++iter)
  {
    cedar::aux::ParameterBasePtr& parameter = iter->second;
    this->insertRow(row);
    QLabel *p_label = new QLabel();
    p_label->setText(parameter->getName().c_str());
    this->setCellWidget(row, 0, p_label);

    cedar::proc::gui::ParameterBase *p_widget = dataWidgetTypes().get(parameter)->allocateRaw();
    p_widget->setParameter(parameter);
    this->setCellWidget(row, 1, p_widget);
  }
}


cedar::proc::gui::PropertyPane::DataWidgetTypes& cedar::proc::gui::PropertyPane::dataWidgetTypes()
{
  if (cedar::proc::gui::PropertyPane::mDataWidgetTypes.empty())
  {
    cedar::proc::gui::PropertyPane::mDataWidgetTypes.add<cedar::aux::DoubleParameter, cedar::proc::gui::DoubleParameter>();
    cedar::proc::gui::PropertyPane::mDataWidgetTypes.add<cedar::aux::StringParameter, cedar::proc::gui::StringParameter>();
    cedar::proc::gui::PropertyPane::mDataWidgetTypes.add<cedar::aux::BoolParameter, cedar::proc::gui::BoolParameter>();
  }
  return cedar::proc::gui::PropertyPane::mDataWidgetTypes;
}
