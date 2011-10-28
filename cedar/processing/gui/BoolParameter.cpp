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

    File:        BoolParameter.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 12

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "processing/gui/BoolParameter.h"
#include "auxiliaries/ParameterTemplate.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QHBoxLayout>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::BoolParameter::BoolParameter(QWidget *pParent)
:
cedar::proc::gui::Parameter(pParent)
{
  this->setLayout(new QHBoxLayout());
  this->mpCheckBox = new QCheckBox();
  this->layout()->setContentsMargins(0, 0, 0, 0);
  this->layout()->addWidget(this->mpCheckBox);

  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
}

//!@brief Destructor
cedar::proc::gui::BoolParameter::~BoolParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::BoolParameter::parameterPointerChanged()
{
  cedar::aux::BoolParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::BoolParameter>(this->getParameter());
  this->mpCheckBox->setChecked(parameter->getValue());
  QObject::connect(this->mpCheckBox, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
}

void cedar::proc::gui::BoolParameter::stateChanged(int state)
{
  cedar::aux::BoolParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::BoolParameter>(this->getParameter());
  parameter->setValue(state == Qt::Checked);
}

