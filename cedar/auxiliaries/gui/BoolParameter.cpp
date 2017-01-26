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

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/BoolParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/TypeBasedFactory.h"
#include "cedar/auxiliaries/Singleton.h"

// SYSTEM INCLUDES
#include <QHBoxLayout>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// associate aux::gui parameter with the aux parameter
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered = cedar::aux::gui::ParameterFactorySingleton::getInstance()->add
      <
        cedar::aux::BoolParameter,
        cedar::aux::gui::BoolParameter
      >();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::BoolParameter::BoolParameter(QWidget *pParent)
:
cedar::aux::gui::Parameter(pParent)
{
  this->setLayout(new QHBoxLayout());
  this->mpCheckBox = new QCheckBox();
  this->layout()->setContentsMargins(0, 0, 0, 0);
  this->layout()->addWidget(this->mpCheckBox);

  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
}

//!@brief Destructor
cedar::aux::gui::BoolParameter::~BoolParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::BoolParameter::parameterPointerChanged()
{
  cedar::aux::BoolParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::BoolParameter>(this->getParameter());
  parameter->lockForRead();
  bool checked = parameter->getValue();
  parameter->unlock();
  this->mpCheckBox->setChecked(checked);
  QObject::connect(this->mpCheckBox, SIGNAL(stateChanged(int)), this, SLOT(stateChanged(int)));
  QObject::connect(parameter.get(), SIGNAL(valueChanged()), this, SLOT(parameterValueChanged()));

  this->propertiesChanged();
}

void cedar::aux::gui::BoolParameter::propertiesChanged()
{
  cedar::aux::BoolParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::BoolParameter>(this->getParameter());
  this->mpCheckBox->setEnabled(!parameter->isConstant());
}

void cedar::aux::gui::BoolParameter::stateChanged(int state)
{
  bool value = (state == Qt::Checked);
  cedar::aux::BoolParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::BoolParameter>(this->getParameter());
  if (value != parameter->getValue())
  {
    parameter->setValue(value, true);
  }
}

void cedar::aux::gui::BoolParameter::parameterValueChanged()
{
  cedar::aux::BoolParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::BoolParameter>(this->getParameter());

  if (this->mpCheckBox->isChecked() != parameter->getValue())
  {
    // important: do not use setChecked here, as that is bugged and can lead to problems if the checkbox is immediately
    // disabled.
    // see: https://bugreports.qt-project.org/browse/qtbug-15309
    this->mpCheckBox->setCheckState(parameter->getValue()? Qt::Checked : Qt::Unchecked);
  }
}
