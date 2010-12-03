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

using namespace std;
using namespace boost::posix_time;


class MyTestThread : public cedar::aux::LoopedThread {

private:

  bool mArtificialDelay;

public:

  MyTestThread( bool delay = false ) {
    mArtificialDelay = delay;
    srand(microsec_clock::universal_time().time_of_day().total_milliseconds());
  }

  MyTestThread( unsigned long stepSize, bool delay = false ) : LoopedThread( stepSize ) {
    mArtificialDelay = delay;
    srand(microsec_clock::universal_time().time_of_day().total_milliseconds());
  }

  void step(unsigned long time) {
    ptime now = microsec_clock::universal_time();
    cout << "wake up time (sec/msec): " << now.time_of_day().seconds() << " / " << now.time_of_day().total_microseconds() % 1000000 << endl;
    if( mArtificialDelay )
      usleep( rand() % (3*mStepSize.total_microseconds()) );
  }

  bool getArtificialDelay() {
    return mArtificialDelay;
  }

  void setArtificalDelay( bool delay ) {
    mArtificialDelay = delay;
  }

};


int main() {

  unsigned long timeInterval = 100000;  // microseconds
  MyTestThread thread( timeInterval );

  cout << "Starting a thread and let it run for 2 seconds ..." << endl;
  thread.start();
  thread.wait(1000);
  cout << "Stopping thread ..." << endl;
  thread.stop();

  cout << endl;
  cout << "Starting thread again with an artificially unreliable execution time ..." << endl;
  thread.setArtificalDelay(true);
  thread.start();
  thread.wait(1000);
  cout << "Stopping thread ..." << endl;
  thread.stop();

  return 0;
}
