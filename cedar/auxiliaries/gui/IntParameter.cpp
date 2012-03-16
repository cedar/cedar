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

    File:        IntParameter.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2012 03 16

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/IntParameter.h"
#include "cedar/auxiliaries/IntParameter.h"
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
        cedar::aux::IntParameter,
        cedar::aux::gui::IntParameter
      >();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::IntParameter::IntParameter(QWidget *pParent)
:
cedar::aux::gui::Parameter(pParent)
{
  this->setLayout(new QHBoxLayout());
  this->mpSpinbox = new QSpinBox();
  this->layout()->setContentsMargins(0, 0, 0, 0);
  this->layout()->addWidget(this->mpSpinbox);
  this->mpSpinbox->setMinimum(0.0);
  this->mpSpinbox->setMaximum(100.0);

  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
}

//!@brief Destructor
cedar::aux::gui::IntParameter::~IntParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::IntParameter::parameterPointerChanged()
{
  cedar::aux::IntParameterPtr parameter = boost::dynamic_pointer_cast<cedar::aux::IntParameter>(this->getParameter());

  this->propertiesChanged();

  this->mpSpinbox->setValue(parameter->getValue());
  QObject::connect(this->mpSpinbox, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));

  QObject::connect(parameter.get(), SIGNAL(propertyChanged()), this, SLOT(propertiesChanged()));
}

void cedar::aux::gui::IntParameter::propertiesChanged()
{
  cedar::aux::IntParameterPtr parameter = boost::dynamic_pointer_cast<cedar::aux::IntParameter>(this->getParameter());
  this->mpSpinbox->setMinimum(parameter->getMinimum());
  this->mpSpinbox->setMaximum(parameter->getMaximum());
  this->mpSpinbox->setDisabled(parameter->isConstant());
}

void cedar::aux::gui::IntParameter::valueChanged(int value)
{
  cedar::aux::IntParameterPtr parameter = boost::dynamic_pointer_cast<cedar::aux::IntParameter>(this->getParameter());
  parameter->setValue(value);
}
