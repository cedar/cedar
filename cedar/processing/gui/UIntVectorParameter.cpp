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

    File:        UIntVectorParameter.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 21

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "processing/gui/UIntVectorParameter.h"
#include "auxiliaries/NumericVectorParameter.h"
#include "auxiliaries/macros.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::UIntVectorParameter::UIntVectorParameter(QWidget *pParent)
:
cedar::proc::gui::ParameterBase(pParent)
{
  this->setLayout(new QVBoxLayout());
  this->layout()->setContentsMargins(0, 0, 0, 0);
  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
}

//!@brief Destructor
cedar::proc::gui::UIntVectorParameter::~UIntVectorParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::UIntVectorParameter::parameterPointerChanged()
{
  cedar::aux::UIntVectorParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::UIntVectorParameter>(this->getParameter());
  // first, clear the old widgets
  for (size_t i = 0; i < this->mSpinboxes.size(); ++i)
  {
    if (this->mSpinboxes.at(i))
    {
      delete this->mSpinboxes.at(i);
    }
  }
  this->mSpinboxes.clear();
  for (
        size_t i = 0;
        i < boost::dynamic_pointer_cast<cedar::aux::UIntVectorParameter>(this->getParameter())->get().size();
        ++i
      )
  {
    this->mSpinboxes.push_back(new QSpinBox());
    this->layout()->addWidget(this->mSpinboxes.at(i));
    this->mSpinboxes.at(i)->setMinimumHeight(20);
    this->mSpinboxes.at(i)->setMinimum(parameter->getMinimum());
    this->mSpinboxes.at(i)->setMaximum(parameter->getMaximum());
    this->mSpinboxes.at(i)->setValue(parameter->get().at(i));
    QObject::connect(this->mSpinboxes.at(i), SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
  }
}

void cedar::proc::gui::UIntVectorParameter::valueChanged(int)
{
  cedar::aux::UIntVectorParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::UIntVectorParameter>(this->getParameter());
  std::vector<unsigned int> values = parameter->get();
  CEDAR_DEBUG_ASSERT(this->mSpinboxes.size() == values.size());
  for (size_t i = 0; i < values.size(); ++i)
  {
    values.at(i) = this->mSpinboxes.at(i)->value();
  }
  parameter->set(values);
}
