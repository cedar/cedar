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

    File:        ThreadWorker.cpp

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Mon 07 Jan 2013 05:50:41 PM CET

    Description:

    Credits:

=============================================================================*/

// LOCAL INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/ThreadWorker.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/stringFunctions.h"

cedar::aux::detail::ThreadWorker::ThreadWorker(cedar::aux::ThreadWrapper *wrapper) 
: mpWrapper(wrapper), mStop(false)
{
}

cedar::aux::detail::ThreadWorker::~ThreadWorker() 
{
  // Note: do not delete mpWrapper here! it should delete us
}

void cedar::aux::detail::ThreadWorker::requestStop()
{
  // this will stop and exit the loop (not the thread - the thread is stopped
  // by the wrapper LoopedThread)

  // TODO: needs to be locked!
  mStop = true;
}

bool cedar::aux::detail::ThreadWorker::stopRequested()
{
  // TODO: locking
  return mStop;
}

