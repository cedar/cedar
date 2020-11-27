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

    File:        IntParameter.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2012 03 16

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/auxiliaries/gui/IntParameter.h"
#include "cedar/auxiliaries/IntParameter.h"
#include "cedar/processing/auxiliaries/TypeBasedFactory.h"
#include "cedar/processing/auxiliaries/Singleton.h"

// SYSTEM INCLUDES
#include <QHBoxLayout>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// associate aux::gui parameter with the aux parameter
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered = cedar::proc::aux::gui::ParameterFactorySingleton::getInstance()->add
      <
        cedar::aux::IntParameter,
        cedar::proc::aux::gui::IntParameter
      >();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::aux::gui::IntParameter::IntParameter(QWidget *pParent)
:
cedar::proc::aux::gui::IntParameter::Base(pParent)
{
}

//!@brief Destructor
cedar::proc::aux::gui::IntParameter::~IntParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::aux::gui::IntParameter::parameterChanged()
{
  this->Base::parameterChanged();
  QObject::connect(this->mpWidget, SIGNAL(valueChanged(int)), this, SLOT(valueChanged(int)));
}


void cedar::proc::aux::gui::IntParameter::valueChanged(int)
{
  this->widgetValueChanged();
}
