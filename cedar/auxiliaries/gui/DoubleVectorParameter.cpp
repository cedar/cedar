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

    File:        DoubleVectorParameter.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 20

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/DoubleVectorParameter.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::DoubleVectorParameter::DoubleVectorParameter(QWidget *pParent)
:
cedar::aux::gui::Parameter(pParent)
{
  this->setLayout(new QVBoxLayout());
  this->layout()->setContentsMargins(0, 0, 0, 0);
  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
}

//!@brief Destructor
cedar::aux::gui::DoubleVectorParameter::~DoubleVectorParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

//!@todo A lot of this code is almost the same as cedar::aux::gui::UIntVectorParameter.

void cedar::aux::gui::DoubleVectorParameter::parameterPointerChanged()
{
  cedar::aux::DoubleVectorParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::DoubleVectorParameter>(this->getParameter());

  QObject::connect(parameter.get(), SIGNAL(propertyChanged()), this, SLOT(propertyChanged()));
  this->propertyChanged();
}

void cedar::aux::gui::DoubleVectorParameter::propertyChanged()
{
  cedar::aux::DoubleVectorParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::DoubleVectorParameter>(this->getParameter());

  //!@todo Don't throw away old spinboxes, reuse them instead
  // Create the appropriate amount of spinboxes
  if (this->mSpinboxes.size() != parameter->size())
  {
    for (size_t i = 0; i < this->mSpinboxes.size(); ++i)
    {
      delete this->mSpinboxes.at(i);
    }
    this->mSpinboxes.clear();

    for (size_t i = 0; i < parameter->size(); ++i)
    {
      QDoubleSpinBox *p_widget = new QDoubleSpinBox();
      this->mSpinboxes.push_back(p_widget);
      this->layout()->addWidget(p_widget);
      p_widget->setMinimumHeight(20);

      // The limits have to be set here already so that the value is set properly.
      this->mSpinboxes.at(i)->setMinimum(parameter->getMinimum());
      this->mSpinboxes.at(i)->setMaximum(parameter->getMaximum());

      p_widget->setValue(parameter->at(i));
      QObject::connect(p_widget, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));
    }

    emit heightChanged();
  }

  // Update the spinboxes' properties
  for (size_t i = 0; i < this->mSpinboxes.size(); ++i)
  {
    this->mSpinboxes.at(i)->setMinimum(parameter->getMinimum());
    this->mSpinboxes.at(i)->setMaximum(parameter->getMaximum());
  }
}

void cedar::aux::gui::DoubleVectorParameter::valueChanged(double)
{
  cedar::aux::DoubleVectorParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::DoubleVectorParameter>(this->getParameter());
  std::vector<double> values = parameter->getValue();
  CEDAR_DEBUG_ASSERT(this->mSpinboxes.size() == values.size());
  for (size_t i = 0; i < values.size(); ++i)
  {
    values.at(i) = this->mSpinboxes.at(i)->value();
  }
  parameter->set(values);
}

