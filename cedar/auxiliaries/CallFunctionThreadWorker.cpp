/*=============================================================================

    Copyright 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        CallFunctionThreadWorker.cpp

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Mon 07 Jan 2013 05:50:41 PM CET

    Description:

    Credits:

=============================================================================*/

// LOCAL INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/CallFunctionThreadWorker.h"
#include "cedar/auxiliaries/CallFunctionThread.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/stringFunctions.h"

cedar::aux::detail::CallFunctionThreadWorker::CallFunctionThreadWorker(cedar::aux::CallFunctionThread *wrapper) 
: ThreadWorker( wrapper ), mpWrapper(wrapper)
{
}

cedar::aux::detail::CallFunctionThreadWorker::~CallFunctionThreadWorker() 
{
}

void cedar::aux::detail::CallFunctionThreadWorker::work()
{
  // TODO: check whether wrapper (mpWrapper) still exists (paranoid)

  mStop = false; // TODO: locking

  mpWrapper->call();

  mStop = false; // TODO: locking
  return;
}


