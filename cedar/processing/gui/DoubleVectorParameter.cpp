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

// LOCAL INCLUDES
#include "processing/gui/DoubleVectorParameter.h"
#include "auxiliaries/NumericVectorParameter.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::DoubleVectorParameter::DoubleVectorParameter(QWidget *pParent)
:
cedar::proc::gui::ParameterBase(pParent)
{
  this->setLayout(new QVBoxLayout());
  this->layout()->setContentsMargins(0, 0, 0, 0);
  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
}

//!@brief Destructor
cedar::proc::gui::DoubleVectorParameter::~DoubleVectorParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::DoubleVectorParameter::parameterPointerChanged()
{
  cedar::aux::DoubleVectorParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::DoubleVectorParameter>(this->getParameter());
  std::cout << "size " << boost::dynamic_pointer_cast<cedar::aux::DoubleVectorParameter>(this->getParameter())->get().size() << std::endl;
  this->mSpinboxes.clear();
  for (size_t i = 0; i < boost::dynamic_pointer_cast<cedar::aux::DoubleVectorParameter>(this->getParameter())->get().size(); i++)
  {
    this->mSpinboxes.push_back(new QDoubleSpinBox());
    this->layout()->addWidget(this->mSpinboxes.at(i));
    this->mSpinboxes.at(i)->setMinimum(-100.0);
    this->mSpinboxes.at(i)->setMaximum(+100.0);
  }
  std::cout << "step1 " << std::endl;
  for (size_t i = 0; i < mSpinboxes.size(); i++)
  {
    this->mSpinboxes.at(i)->setMinimum(parameter->getMinimum());
    this->mSpinboxes.at(i)->setMaximum(parameter->getMaximum());
    this->mSpinboxes.at(i)->setValue(parameter->get().at(i));
    QObject::connect(this->mSpinboxes.at(i), SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));
  }
}

void cedar::proc::gui::DoubleVectorParameter::valueChanged(double value)
{
  cedar::aux::DoubleVectorParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::DoubleVectorParameter>(this->getParameter());
//  parameter->set(value);
}

