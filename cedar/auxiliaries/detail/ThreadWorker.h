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

    File:        ThreadWorker.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Mon 07 Jan 2013 05:50:01 PM CET

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_THREADWORKER_H
#define CEDAR_THREADWORKER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/LoopMode.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/detail/ThreadWorker.fwd.h"

// SYSTEM INCLUDES
#include <string>
#include <iostream>
#include <QThread>
#ifndef Q_MOC_RUN
  #include <boost/date_time/posix_time/posix_time_types.hpp>
#endif

/*!The worker base class that goes with ThreadWrapper
 *
 * Implementors should not use this.
 *@see LoopedThread, CallFunctionInThread
 */
class cedar::aux::detail::ThreadWorker : public QObject
{
  Q_OBJECT

  public:
    //! destructor
    virtual ~ThreadWorker();

  private:
    //! children will do the work, here
    virtual void work() = 0;

  public slots:
    //! slot to start the work in the new thread
    void workSlot();

  signals:
    //! slot that signals that work() has ended.
    void finishedWorking();
};


#endif

