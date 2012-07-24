/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        main.cpp

    Maintainer:  Bjoern Weghenkel
    Email:       bjoern.weghenkel@ini.rub.de
    Date:        2010 11 17

    Description: Example for the behavior of the @em cedar::aux::Thread class.

    Credits:

======================================================================================================================*/

#include "cedar/auxiliaries/LoopedThread.h"

#include <cstdlib>
#include <iostream>

//!@brief threaded test class
class MyTestThread : public cedar::aux::LoopedThread {

private:

  //!@brief decides whether to add an artificial delay
  bool mArtificialDelay;

public:

  //!@brief constructor
  MyTestThread(double stepSize, bool delay = false) : LoopedThread(stepSize) 
  {
    mArtificialDelay = delay;
    srand(boost::posix_time::microsec_clock::universal_time().time_of_day().total_milliseconds());
  }

  void step(double time)
  {
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
    std::cout << "current time (sec/usec): " << now.time_of_day().seconds()
              << " / " << now.time_of_day().total_microseconds() % 1000000
              << " (time in step(): " << time << ")" << std::endl;
    if ( mArtificialDelay )
    {
      usleep( rand() % (3 * static_cast<int>(this->getStepSize()) * 1000));
    }
  }

  /*!@brief get info whether an artificial delay is added
   *
   * @return    state of mArtificialDelay
   */
  bool getArtificialDelay() 
  {
    return mArtificialDelay;
  }

  /*!@brief set whether an artificial delay is added
   *
   * @param delay    state of mArtificialDelay
   */
  void setArtificalDelay(bool delay)
  {
    mArtificialDelay = delay;
  }

};


int main()
{

  double timeInterval = 100.0;  // milliseconds
  MyTestThread thread(timeInterval);
  //thread.useFixedStepSize(false);
  //thread.setSimulatedTime(50);

  std::cout << "Starting a thread and let it run for 1 seconds ..." << std::endl;
  thread.start();
  thread.wait(1000);
  std::cout << "Stopping thread ..." << std::endl;
  thread.stop();

  std::cout << std::endl;
  std::cout << "Starting thread again with an artificially unreliable execution time ..." << std::endl;
  thread.setArtificalDelay(true);
  thread.start();
  thread.wait(1000);
  std::cout << "Stopping thread ..." << std::endl;
  thread.stop();
  thread.setArtificalDelay(false);

  std::cout << std::endl;
  std::cout << "Starting thread again with step size 0 for one millisecond ..." << std::endl;
  thread.setStepSize(0);
  thread.start();
  thread.wait(1);
  std::cout << "Stopping thread ..." << std::endl;
  thread.stop();

  return 0;
}
