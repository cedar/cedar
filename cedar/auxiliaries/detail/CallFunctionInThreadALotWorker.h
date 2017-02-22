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

    File:        CallFunctionInThreadALotWorker.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Mon 07 Jan 2013 05:50:01 PM CET

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_CALL_FUN_THREAD_ALOT_WORKER_H
#define CEDAR_CALL_FUN_THREAD_ALOT_WORKER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/detail/ThreadWorker.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/LoopMode.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/CallFunctionInThreadALot.fwd.h"
#include "cedar/auxiliaries/detail/CallFunctionInThreadALotWorker.fwd.h"

// SYSTEM INCLUDES
#include <string>
#include <iostream>
#include <QThread>
#ifndef Q_MOC_RUN
  #include <boost/date_time/posix_time/posix_time_types.hpp>
#endif

//! the corresponding worker class for the CallFunctionInThreadALot class
class cedar::aux::detail::CallFunctionInThreadALotWorker : public cedar::aux::detail::ThreadWorker
{
  Q_OBJECT

  public: 
    //! Constructor.
    CallFunctionInThreadALotWorker(cedar::aux::CallFunctionInThreadALot* wrapper);

  private:

  public:
    //! Does the actual work.
    void work(); // virtual in parent

  public slots:
    //! Executes the work function.
    void executeSlot();

  private:
    //! Wrapper that contains the function to be called.
    cedar::aux::CallFunctionInThreadALot* mpWrapper;
};


#endif // CEDAR_CALL_FUN_THREAD_ALOT_WORKER_H

