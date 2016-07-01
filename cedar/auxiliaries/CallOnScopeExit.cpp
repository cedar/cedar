/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        CallOnScopeExit.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 03 18

    Description: Header file for the class cedar::aux::CallOnScopeExit.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/CallOnScopeExit.h"

// FORWARD DECLARATIONS

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::CallOnScopeExit::CallOnScopeExit(const boost::function<void ()>& function)
:
mFunctionToCall(function),
mCall(true)
{
}

cedar::aux::CallOnScopeExit::~CallOnScopeExit()
{
  this->preDestruct();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::aux::CallOnScopeExit::callNow()
{
  this->mCall = false;
  this->mFunctionToCall();
}

void cedar::aux::CallOnScopeExit::resetCall()
{
  this->mCall = true;
}

void cedar::aux::CallOnScopeExit::preDestruct()
{
  if (this->mCall)
  {
    this->callNow();
  }
}
