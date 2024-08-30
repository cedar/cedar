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
#include "cedar/auxiliaries/LoopMode.h"
#include "cedar/auxiliaries/Configurable.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/GlobalClock.fwd.h"

// SYSTEM INCLUDES
#include <QTime>
#ifndef Q_MOC_RUN
  #include <boost/signals2.hpp>
#endif // Q_MOC_RUN
#include <QReadWriteLock>

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
  cedar::unit::Time getTime() const;

  //! Returns true if the timer is running.
  bool isRunning() const;

  //! Adds the given amount of time to the global clock.
  void addTime(const cedar::unit::Time& time);

  cedar::unit::Time getDefaultCPUStepSize();

  void setDefaultCPUStepSize(cedar::unit::Time newDefaultStepSize);

  cedar::unit::Time getSimulationStepSize();

  void setMinimumComputationTime(cedar::unit::Time newMinComputationTime);

  cedar::unit::Time getMinimumComputationTime();

  void setSimulationStepSize(cedar::unit::Time newSimulationStepSize);

  cedar::aux::LoopMode::Id getLoopMode();

  bool isBatchMode();

  long getSeed();
  void setSeed(long seed);

  void setLoopMode(cedar::aux::LoopMode::Id newLoopMode);

  void setBatchMode(bool batchMode);

  void updateTakenSteps(unsigned long newStepsTaken);

  unsigned long getNumOfTakenSteps() const;

  double getCurrentMinTau();

  void updateCurrentMinTau();

  void updateFieldTauMap(cedar::aux::ConfigurableWeakPtr confWPointer, double tauValue);

  boost::signals2::connection connectToDefaultCPUStepSizeChangedSignal(boost::function<void(cedar::unit::Time)> slot)
  {
    return this->mDefaultCPUStepSizeChangedSignal.connect(slot);
  }

  boost::signals2::connection connectToMinimumComputationTimeChangedSignal(boost::function<void(cedar::unit::Time)> slot)
  {
    return this->mMinimumComputationTimeChangedSignal.connect(slot);
  }

  boost::signals2::connection connectToSimulationStepSizeChangedSignal(boost::function<void(cedar::unit::Time)> slot)
  {
    return this->mSimualtionStepSizeChangedSignal.connect(slot);
  }

  boost::signals2::connection connectToLoopModeChangedSignal(boost::function<void(cedar::aux::LoopMode::Id)> slot)
  {
    return this->mLoopModeChangedSignal.connect(slot);
  }

  boost::signals2::connection connectToCurMinTauChangedSignal(boost::function<void()> slot)
  {
      return this->mMinCurTauChangedSignal.connect(slot);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! Takes care of properly reacting to changes in the global time factor.
  void globalTimeFactorChanged(double newFactor);

  void addCurrentToAdditionalElapsedTime();

  double getCurrentElapsedMSec() const;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------

protected:
    // none yet

private:
  //! Lock used to synchronize access to the global clock.
  mutable QReadWriteLock* mpAccessLock;

  mutable QReadWriteLock* mpSimulationStepsTakenLock;

  //!@brief Flag that indicates when the timer starts/stops
  bool mRunning;

  //!@brief The timer used to measure how much time has elapsed since the start.
  QTime mTimer;

  //!@brief This time is added to the elapsed time. It is used to track, e.g., times when the global clock is paused.
  double mAdditionalElapsedTime;

  unsigned long mAdditionalTakenSteps;

  //! A copy of the current time factor, to avoid unnecessary locking/synchronization issues.
  double mCurrentTimeFactor;

  unsigned long mpSimulationStepsTaken;


  cedar::unit::Time mDefaultCPUStepSize;

  cedar::unit::Time mMinimumComputationTime;

  cedar::unit::Time mSimulationStepSize;

  cedar::aux::LoopMode::Id mLoopMode;

  bool mBatchMode;

  long seedState;

  //Current Minimal Tau across all Fields in the Architecture
  std::atomic<double> mCurMinTau;

  std::map<cedar::aux::ConfigurableWeakPtr , double> mFieldTauMap;

  //! Connected to the change signal of the global time factor.
  boost::signals2::scoped_connection mGlobalTimeFactorConnection;

  boost::signals2::signal<void(cedar::unit::Time)> mDefaultCPUStepSizeChangedSignal;

  boost::signals2::signal<void(cedar::unit::Time)> mMinimumComputationTimeChangedSignal;

  boost::signals2::signal<void(cedar::unit::Time)> mSimualtionStepSizeChangedSignal;

  boost::signals2::signal<void(cedar::aux::LoopMode::Id)> mLoopModeChangedSignal;

  boost::signals2::signal<void()> mMinCurTauChangedSignal;
};

#include "cedar/auxiliaries/Singleton.h"

namespace cedar
{
  namespace aux
  {
    CEDAR_INSTANTIATE_AUX_TEMPLATE(cedar::aux::Singleton<cedar::aux::GlobalClock>);
    //! a singleton for the global clock
    typedef cedar::aux::Singleton<cedar::aux::GlobalClock> GlobalClockSingleton;
  }
}

#endif /* CEDAR_AUX_NETWORKTIMER_H_ */
