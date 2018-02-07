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

    File:        StaticGainView.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2014 08 19

    Description: Source file for the class cedar::proc::gui::StaticGainView.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/stepViews/StaticGainView.h"
#include "cedar/processing/Connectable.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::StaticGainView::connectableChanged()
{
  this->cedar::proc::gui::DefaultConnectableIconView::connectableChanged();

  auto parameter = this->getConnectable()->getParameter("gain factor");
  QObject::connect(parameter.get(), SIGNAL(valueChanged()), this, SLOT(updateIconWeight()));

  this->updateIconWeight();
}

void cedar::proc::gui::StaticGainView::updateIconWeight()
{
  auto parameter = boost::dynamic_pointer_cast<cedar::aux::ConstDoubleParameter>(this->getConnectable()->getParameter("gain factor"));

  auto value = parameter->getValue();

  if (cedar::aux::math::isZero(value)) // think of numeric precision
  {
    this->setIconPath(":/steps/static_gain_zero.svg");
  }
  else if (cedar::aux::math::isZero(value - 1.0)) // think of numeric precision
  {
    this->setIconPath(":/steps/static_gain_one.svg");
  }
  else if (cedar::aux::math::isZero(value - -1.0))
  {
    this->setIconPath(":/steps/static_gain_minus_one.svg");
  }
  else if (value > 1.0)
  {
    this->setIconPath(":/steps/static_gain_larger_one.svg");
  }
  else if (value > 0.0
           && value < 1.0)
  {
    this->setIconPath(":/steps/static_gain_smaller_one.svg");
  }
  else if (value < 0.0
           && value > -1.0)
  {
    this->setIconPath(":/steps/static_gain_larger_minus_one.svg");
  }
  else if (value < -1.0)
  {
    this->setIconPath(":/steps/static_gain_smaller_minus_one.svg");
  }
  else
  {
    // fallback
    this->setIconPath(":/steps/static_gain_green.svg");
  }
}
