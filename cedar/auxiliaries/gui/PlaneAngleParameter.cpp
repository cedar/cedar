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

    File:        PlaneAngleParameter.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2013 03 04

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/PlaneAngleParameter.h"
#include "cedar/auxiliaries/PlaneAngleParameter.h"
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
        cedar::aux::PlaneAngleParameter,
        cedar::aux::gui::PlaneAngleParameter
      >();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::PlaneAngleParameter::PlaneAngleParameter(QWidget *pParent)
:
cedar::aux::gui::NumericParameter<cedar::unit::PlaneAngle, QDoubleSpinBox>(pParent)
{
}

//!@brief Destructor
cedar::aux::gui::PlaneAngleParameter::~PlaneAngleParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::PlaneAngleParameter::parameterChanged()
{
  cedar::aux::gui::NumericParameter<cedar::unit::PlaneAngle, QDoubleSpinBox>::parameterChanged();
  QObject::connect(this->mpWidget, SIGNAL(valueChanged(double)), this, SLOT(valueChanged(double)));
}


void cedar::aux::gui::PlaneAngleParameter::valueChanged(double)
{
  this->widgetValueChanged();
}

