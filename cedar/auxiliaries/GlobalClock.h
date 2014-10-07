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

    File:        NetworkTimer.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.ruhr-uni-bochum.de
    Date:        2013 11 7

    Description: Singleton class for a central time in th network network.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_NETWORKTIMER_H_
#define CEDAR_AUX_NETWORKTIMER_H_
// CEDAR INCLUDES
#include "cedar/auxiliaries/Singleton.fwd.h"
#include "cedar/units/Time.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/GlobalClock.fwd.h"

// SYSTEM INCLUDES
#include <QTime>

//!@brief Can start, stop and reset the network time and should be used as a central time giver in a network.
class cedar::aux::GlobalClock
{

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------

  // uses singleton template.
  friend class cedar::aux::Singleton<GlobalClock>;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief The private constructor for singleton usage.
  GlobalClock();
public:
  //!@brief The Destructor.
  ~GlobalClock();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Starts the timer.
  void start();

  //!@brief Resets the timer.
  void reset();

  //!@brief Stops the timer. It can be resumed with another call to start.
  void stop();

  //!@brief Returns the elapsed time since timer has started
  //!@todo make this const?
  cedar::unit::Time getTime();

  //! Returns true if the timer is running.
  bool isRunning() const;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------

protected:
    // none yet

private:
  //!@brief Flag that indicates when the timer starts/stops
  bool mRunning;

  //!@brief The timer used to measure how much time has elapsed since the start.
  QTime mTimer;

  //!@brief This time is added to the elapsed time. It is used to track, e.g., times when the global clock is paused.
  int mAdditionalElapsedTime;
};

#include "cedar/auxiliaries/Singleton.h"

namespace cedar
{
  namespace aux
  {
    CEDAR_INSTANTIATE_AUX_TEMPLATE(cedar::aux::Singleton<cedar::aux::GlobalClock>);
    typedef cedar::aux::Singleton<cedar::aux::GlobalClock> GlobalClockSingleton;
  }
}

#endif /* CEDAR_AUX_NETWORKTIMER_H_ */
