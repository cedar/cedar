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

    File:        LinearSpatialTemplateView.cpp

    Maintainer:  
    Email:       
    Date:        

    Description: Source file for the class cedar::proc::gui::LinearSpatialTemplateView.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/stepViews/LinearSpatialTemplateView.h"
#include "cedar/processing/Connectable.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::LinearSpatialTemplateView::connectableChanged()
{
  this->cedar::proc::gui::DefaultConnectableIconView::connectableChanged();

  auto parameter1 = this->getConnectable()->getParameter("size x");
  auto parameter2 = this->getConnectable()->getParameter("size y");

  QObject::connect(parameter1.get(), SIGNAL(valueChanged()), this, SLOT(updateIconWeight()));
  QObject::connect(parameter2.get(), SIGNAL(valueChanged()), this, SLOT(updateIconWeight()));

  this->updateIconWeight();
}

void cedar::proc::gui::LinearSpatialTemplateView::updateIconWeight()
{
  auto parameter1 = boost::dynamic_pointer_cast<cedar::aux::ConstUIntParameter>(this->getConnectable()->getParameter("size x"));
  auto parameter2 = boost::dynamic_pointer_cast<cedar::aux::ConstUIntParameter>(this->getConnectable()->getParameter("size y"));

  auto value1 = parameter1->getValue();
  auto value2 = parameter2->getValue();

  if (value1 == 1 && value2 != 1)
  {
    this->setIconPath(":/steps/linear_spatial_template_1xn.svg");
  }
  else if (value1 != 1 && value2 == 1)
  {
    this->setIconPath(":/steps/linear_spatial_template_nx1.svg");
  }
  else 
  {
    this->setIconPath(":/steps/linear_spatial_template.svg");
  }
}
