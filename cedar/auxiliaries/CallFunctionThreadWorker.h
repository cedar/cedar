
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

    File:        CallFunctionThreadWorker.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Mon 07 Jan 2013 05:50:01 PM CET

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_CALL_FUN_THREADWORKER_H
#define CEDAR_CALL_FUN_THREADWORKER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/LoopMode.h"
#include "cedar/auxiliaries/ThreadWorker.h"

// SYSTEM INCLUDES
#include <string>
#include <iostream>
#include <QThread>
#include <boost/date_time/posix_time/posix_time_types.hpp>

class cedar::aux::detail::CallFunctionThreadWorker : public cedar::aux::detail::ThreadWorker
{
  public: 
    CallFunctionThreadWorker(cedar::aux::CallFunctionThread* wrapper);
    ~CallFunctionThreadWorker();

  private:

  public:
    void work(); // virtual in parent

  public:

  private:
    cedar::aux::CallFunctionThread* mpWrapper;
};


#endif

