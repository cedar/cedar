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

    File:        IntVectorParameter.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 12

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "IntVectorParameter.h"
#include "cedar/auxiliaries/IntVectorParameter.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/TypeBasedFactory.h"
#include "cedar/auxiliaries/Singleton.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

IntVectorParameter::IntVectorParameter(QWidget *pParent)
:
cedar::aux::gui::IntVectorParameter(pParent)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool IntVectorParameter::empty() const
{
  return this->mWidgets.empty();
}

unsigned int IntVectorParameter::size() const
{
  return this->mWidgets.size();
}

QSpinBox* IntVectorParameter::widgetAt(int i)
{
  return this->mWidgets.at(i);
}
