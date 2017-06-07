/*=============================================================================

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

===============================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        CallFunctionInThreadWorker.cpp

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Mon 07 Jan 2013 05:50:41 PM CET

    Description:

    Credits:

=============================================================================*/

// LOCAL INCLUDES
#include "cedar/auxiliaries/detail/CallFunctionInThreadWorker.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/stringFunctions.h"

cedar::aux::detail::CallFunctionInThreadWorker::CallFunctionInThreadWorker(cedar::aux::CallFunctionInThread *wrapper) 
: mpWrapper(wrapper)
{
}

void cedar::aux::detail::CallFunctionInThreadWorker::work()
{
  CEDAR_ASSERT( mpWrapper != NULL );

  mpWrapper->call();

  mpWrapper->requestStop();

  return;
}


