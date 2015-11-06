/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        BoolVectorParameter.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2015 11 04

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "BoolVectorParameter.h"
#include "cedar/auxiliaries/BoolVectorParameter.h"
#include "cedar/auxiliaries/TypeBasedFactory.h"
#include "cedar/auxiliaries/Singleton.h"

// SYSTEM INCLUDES
//
////----------------------------------------------------------------------------------------------------------------------
//// associate aux::gui parameter with the aux parameter
////----------------------------------------------------------------------------------------------------------------------
//namespace
//{
//  bool registered = cedar::aux::gui::ParameterFactorySingleton::getInstance()->add
//      <
//        cedar::aux::BoolVectorParameter,
//        cedar::aux::gui::BoolVectorParameter
//      >();
//}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

BoolVectorParameter::BoolVectorParameter(QWidget* pParent)
:
cedar::aux::gui::BoolVectorParameter(pParent)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
unsigned int BoolVectorParameter::size() const
{
  return this->mWidgets.size();
}

QCheckBox* BoolVectorParameter::widgetAt(int i)
{
  return this->mWidgets.at(i);
}
