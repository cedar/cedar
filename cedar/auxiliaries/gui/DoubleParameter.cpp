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

    File:        DoubleParameter.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 06

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/DoubleParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
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
        cedar::aux::DoubleParameter,
        cedar::aux::gui::DoubleParameter
      >();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::DoubleParameter::DoubleParameter(QWidget *pParent)
:
cedar::aux::gui::Parameter(pParent)
{
  this->setLayout(new QHBoxLayout());
  this->mpSpinbox = new QDoubleSpinBox();
  this->layout()->setContentsMargins(0, 0, 0, 0);
  this->layout()->addWidget(this->mpSpinbox);
  this->mpSpinbox->setMinimum(-100.0);
  this->mpSpinbox->setMaximum(+100.0);
  this->mpSpinbox->setDecimals(4); //!@todo Make this an option in NumericParameter

  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
}

//!@brief Destructor
cedar::aux::gui::DoubleParameter::~DoubleParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::DoubleParameter::parameterPointerChanged()
{
  this->applyProperties();

  cedar::aux::DoubleParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::DoubleParameter>(this->getParameter());

  // read current value
  parameter->lockForRead();
  this->mpSpinbox->setValue(parameter->getValue());
  parameter->unlock();

  // connect signals/slots
  QObject::connect(this->mpSpinbox, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));
  QObject::connect(parameter.get(), SIGNAL(propertyChanged()), this, SLOT(propertyChanged()));
}

void cedar::aux::gui::DoubleParameter::propertyChanged()
{
  this->applyProperties();
}

void cedar::aux::gui::DoubleParameter::applyProperties()
{
  cedar::aux::DoubleParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::DoubleParameter>(this->getParameter());

  parameter->lockForRead();
  this->mpSpinbox->setMinimum(parameter->getMinimum());
  this->mpSpinbox->setMaximum(parameter->getMaximum());
  this->mpSpinbox->setDisabled(parameter->isConstant());
  parameter->unlock();
}

void cedar::aux::gui::DoubleParameter::valueChanged(double value)
{
  cedar::aux::DoubleParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::DoubleParameter>(this->getParameter());

  parameter->setValue(value, true);
}

