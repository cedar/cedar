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

    File:        NeuralFieldView.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 08 01

    Description: Source file for the class cedar::dyn::gui::NeuralFieldView.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/dynamics/gui/NeuralFieldView.h"
#include "cedar/processing/Connectable.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/dynamics/fields/NeuralField.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dyn::gui::NeuralFieldView::connectableChanged()
{
  this->cedar::proc::gui::DefaultConnectableIconView::connectableChanged();

  auto parameter = this->getConnectable()->getParameter("dimensionality");
  QObject::connect(parameter.get(), SIGNAL(valueChanged()), this, SLOT(updateIconDimensionality()));

  if(auto step =  boost::dynamic_pointer_cast<const cedar::proc::Step>(this->getConnectable()))
  {
    QObject::connect(step.get(), SIGNAL(outPutValueChanged(bool)), this, SLOT(updateActivityIcon(bool)));
  }

  this->updateIconDimensionality();
}

void cedar::dyn::gui::NeuralFieldView::updateIconDimensionality()
{
  auto parameter = boost::dynamic_pointer_cast<cedar::aux::ConstUIntParameter>(this->getConnectable()->getParameter("dimensionality"));
  switch(parameter->getValue())
  {
    case 0:
      this->setIconPath(":/cedar/dynamics/gui/steps/field_0d.svg");
      break;

    case 1:
      this->setIconPath(":/cedar/dynamics/gui/steps/field_1d.svg");
      break;

    case 2:
      this->setIconPath(":/cedar/dynamics/gui/steps/field_2d.svg");
      break;

    case 3:
      this->setIconPath(":/cedar/dynamics/gui/steps/field_3d.svg");
      break;

    default:
      this->setIconPath(":/cedar/dynamics/gui/steps/field_nd.svg");
      break;
  }
}

void cedar::dyn::gui::NeuralFieldView::updateActivityIcon(bool isActive)
{
  auto parameter = boost::dynamic_pointer_cast<cedar::aux::ConstUIntParameter>(this->getConnectable()->getParameter("dimensionality"));
  switch(parameter->getValue())
  {
    case 0:
      isActive ? this->setIconPath(":/cedar/dynamics/gui/steps/field_0d_active.svg") :  this->setIconPath(":/cedar/dynamics/gui/steps/field_0d.svg");
      break;

    case 1:
      isActive ? this->setIconPath(":/cedar/dynamics/gui/steps/field_1d_active.svg") : this->setIconPath(":/cedar/dynamics/gui/steps/field_1d.svg");
      break;

    case 2:
      isActive ? this->setIconPath(":/cedar/dynamics/gui/steps/field_2d_active.svg") : this->setIconPath(":/cedar/dynamics/gui/steps/field_2d.svg");
      break;

    case 3:
      isActive ? this->setIconPath(":/cedar/dynamics/gui/steps/field_3d_active.svg") : this->setIconPath(":/cedar/dynamics/gui/steps/field_3d.svg");
      break;
    default:
      isActive ? this->setIconPath(":/cedar/dynamics/gui/steps/field_nd_active.svg") : this->setIconPath(":/cedar/dynamics/gui/steps/field_nd.svg");
      break;
  }
}

