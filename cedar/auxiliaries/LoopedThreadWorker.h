
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

    File:        LoopedThreadWorker.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Mon 07 Jan 2013 05:50:01 PM CET

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_LOOPEDTHREADWORKER_H
#define CEDAR_LOOPEDTHREADWORKER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/LoopMode.h"

// SYSTEM INCLUDES
#include <string>
#include <iostream>
#include <QThread>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace cedar
{
  namespace aux
  {
    namespace detail
    {
      class LoopedThreadWorker; // doesnt need to be in namespace.h (internal)
    };
  };
};


class cedar::aux::detail::LoopedThreadWorker : public QObject
{
  Q_OBJECT

  public: 
    LoopedThreadWorker(cedar::aux::LoopedThread* wrapper);
    ~LoopedThreadWorker();

  private:
    void initStatistics();
    inline void updateStatistics(double stepsTaken);


  public slots:
    void work();

  public:
    /*!@brief Returns the last timesteps start time.
     *
     */
    boost::posix_time::ptime getLastTimeStepStart() const;

    /*!@brief Returns the last timesteps end time.
     *
     */
    boost::posix_time::ptime getLastTimeStepEnd() const;

    /*!@brief Returns the last timesteps duration.
     *
     */
    boost::posix_time::time_duration getLastTimeStepDuration() const;

    unsigned long getNumberOfSteps();
    double getMaxStepsTaken();

    void requestStop();
    bool stopRequested();

  private:
    cedar::aux::LoopedThread *mpWrapper;

    //!@brief stop is requested
    volatile bool mStop; // volatile disables some optimizations, but doesn't add thread-safety

    //!@brief total number of steps since start()
    unsigned long mNumberOfSteps;
    //!@brief
    double mSumOfStepsTaken;
    //!@brief
    double mMaxStepsTaken;
    //!@brief remember time stamps of last step
    boost::posix_time::ptime mLastTimeStepStart;
    //!@brief remember time stamps of last step
    boost::posix_time::ptime mLastTimeStepEnd;

};


#endif

