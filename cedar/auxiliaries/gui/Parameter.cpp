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

    File:        Parameter.cpp

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
#include "cedar/auxiliaries/gui/Parameter.h"
#include "cedar/auxiliaries/Parameter.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::Parameter::Parameter(QWidget *pParent)
:
QWidget(pParent)
{
  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterChanged()));
}

//!@brief Destructor
cedar::aux::gui::Parameter::~Parameter()
{
  if (this->mParameter)
  {
    QObject::disconnect(this->mParameter.get(), SIGNAL(propertyChanged()), this, SLOT(propertiesChanged()));
    QObject::disconnect(this->mParameter.get(), SIGNAL(valueChanged()), this, SLOT(valueChanged()));
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::aux::gui::Parameter::setReadOnly(bool readOnly)
{
  this->setDisabled(readOnly);
}

void cedar::aux::gui::Parameter::setParameter(cedar::aux::ParameterPtr pParameter)
{
  // disconnect old slots if a parameter was already set
  if (this->mParameter)
  {
    QObject::disconnect(this->mParameter.get(), SIGNAL(propertyChanged()), this, SLOT(propertiesChanged()));
    QObject::disconnect(this->mParameter.get(), SIGNAL(valueChanged()), this, SLOT(valueChanged()));
  }
  this->mParameter = pParameter;

  QObject::connect(pParameter.get(), SIGNAL(propertyChanged()), this, SLOT(propertiesChanged()));
  QObject::connect(pParameter.get(), SIGNAL(valueChanged()), this, SLOT(valueChanged()));

  emit parameterPointerChanged();
}

cedar::aux::ParameterPtr cedar::aux::gui::Parameter::getParameter()
{
  return this->mParameter;
}

void cedar::aux::gui::Parameter::parameterChanged()
{
}

void cedar::aux::gui::Parameter::propertiesChanged()
{
}

void cedar::aux::gui::Parameter::valueChanged()
{
}
