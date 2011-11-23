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

    File:        ProjectionMappingParameter.cpp

    Maintainer:  Mathis Richter

    Email:       mathis.richter@ini.rub.de

    Date:        2011 11 16

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/processing/gui/ProjectionMappingParameter.h"

// PROJECT INCLUDES
#include "cedar/processing/namespace.h"
#include "cedar/processing/ProjectionMappingParameter.h"
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::ProjectionMappingParameter::ProjectionMappingParameter(QWidget *pParent)
:
cedar::proc::gui::Parameter(pParent)
{
  this->setLayout(new QVBoxLayout());
  this->layout()->setContentsMargins(0, 0, 0, 0);
  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
}

//!@brief Destructor
cedar::proc::gui::ProjectionMappingParameter::~ProjectionMappingParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

//!@todo A lot of this code is almost the same as cedar::proc::gui::UIntVectorParameter.

void cedar::proc::gui::ProjectionMappingParameter::parameterPointerChanged()
{
  cedar::proc::ProjectionMappingParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::proc::ProjectionMappingParameter>(this->getParameter());

  QObject::connect(parameter.get(), SIGNAL(propertyChanged()), this, SLOT(propertyChanged()));
  this->propertyChanged();
}

void cedar::proc::gui::ProjectionMappingParameter::propertyChanged()
{
  cedar::proc::ProjectionMappingParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::proc::ProjectionMappingParameter>(this->getParameter());

  //!@todo Don't throw away old spinboxes, reuse them instead
  // Create the appropriate amount of spinboxes
  if (this->mComboBoxes.size() != parameter->getValue()->getNumberOfMappings())
  {
    for (size_t i = 0; i < this->mComboBoxes.size(); ++i)
    {
      delete this->mComboBoxes.at(i);
    }
    this->mComboBoxes.clear();

    QStringList output_dimensions;
    output_dimensions.push_back("drop");
    for (unsigned int i = 0; i < parameter->getMaximumNumberOfMappings(); ++i)
    {
      output_dimensions.push_back(cedar::aux::toString<unsigned int>(i).c_str());
    }

    for (size_t i = 0; i < parameter->getMaximumNumberOfMappings(); ++i)
    {
      QComboBox *p_widget = new QComboBox();
      this->mComboBoxes.push_back(p_widget);
      this->layout()->addWidget(p_widget);
      p_widget->setMinimumHeight(20);

      this->mComboBoxes.at(i)->addItems(output_dimensions);

      p_widget->setCurrentIndex(parameter->getValue()->lookUp(i));
      QObject::connect(p_widget, SIGNAL(currentIndexChanged(int)), this, SLOT(currentIndexChanged(int)));
    }

    emit heightChanged();
  }
}

void cedar::proc::gui::ProjectionMappingParameter::currentIndexChanged(int)
{
  cedar::proc::ProjectionMappingParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::proc::ProjectionMappingParameter>(this->getParameter());

//  CEDAR_DEBUG_ASSERT(this->mComboBoxes.size() == values.size());
  for (size_t i = 0; i < this->mComboBoxes.size(); ++i)
  {
    if (this->mComboBoxes.at(i)->currentText().compare("drop"))
    {
      parameter->dropDimension(i);
    }
    else
    {
      unsigned int output_dimension = static_cast<unsigned int>(this->mComboBoxes.at(i)->currentIndex() - 1);
      parameter->addMapping(i, output_dimension);
    }
  }
}

