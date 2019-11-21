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

    File:        NaNCheckView.cpp

    Maintainer:  
    Email:       
    Date:        

    Description: Source file for the class cedar::proc::gui::NaNCheckView.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/stepViews/NaNCheckView.h"
#include "cedar/processing/steps/NaNCheck.h"
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
void cedar::proc::gui::NaNCheckView::connectableChanged()
{
  this->cedar::proc::gui::DefaultConnectableIconView::connectableChanged();

  auto step= boost::dynamic_pointer_cast<const cedar::proc::steps::NaNCheck>(this->getConnectable());

  QObject::connect(step.get(), SIGNAL(caughtNaNChangedSignal()), this, SLOT(updateIconWeight()));
  
  this->updateIconWeight();
}

void cedar::proc::gui::NaNCheckView::updateIconWeight()
{
  auto step= boost::dynamic_pointer_cast<const cedar::proc::steps::NaNCheck>(this->getConnectable());

  if (step->getCaughtNaN())
  {
    this->setIconPath(":/steps/nan_check_caught_nan.svg");
  }
  else if (step->getCaughtInf())
  {
    this->setIconPath(":/steps/nan_check_caught_inf.svg");
  }
  else if (step->getCaughtEmpty())
  {
    this->setIconPath(":/steps/nan_check_caught_empty.svg");
  }
  else 
  {
    this->setIconPath(":/steps/nan_check.svg");
  }
}
