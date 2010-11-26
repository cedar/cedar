/*------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        thread-test.cpp

 ----- Author:      Bjoern Weghenkel
 ----- Email:       bjoern.weghenkel@ini.rub.de
 ----- Date:        2010 11 17

 ----- Description: Example for the behavior of the @em cedar::aux::Thread class.

 ----- Credits:
 -----------------------------------------------------------------------------*/

#include "cedar/auxiliaries/Thread.h"

#include <cstdlib>
#include <iostream>
#include <QTime>

using namespace std;


class MyTestThread : public cedar::aux::Thread {

private:

  bool mArtificialDelay;

public:

  MyTestThread( bool delay = false ) {
    mArtificialDelay = delay;
    srand( QTime::currentTime().msec() );
  }

  MyTestThread( unsigned idleTime, bool delay = false ) : Thread( idleTime ) {
    mArtificialDelay = delay;
    srand( QTime::currentTime().msec() );
  }

  void step(unsigned int time) {
    QTime now = QTime::currentTime();
    cout << "wake up time (sec/msec): " << now.second() << " / " << now.msec() << endl;
    if( mArtificialDelay )
      msleep( rand() % (3*mIdleTime) );
  }

  bool getArtificialDelay() {
    return mArtificialDelay;
  }

  void setArtificalDelay( bool delay ) {
    mArtificialDelay = delay;
  }

};


int main() {

  unsigned int timeInterval = 100;
  MyTestThread thread( timeInterval );

  cout << "Starting a thread and let it run for 2 seconds ..." << endl;
  thread.start();
  thread.wait(2*1000);
  cout << "Stopping thread ..." << endl;
  thread.stop();

  cout << endl;
  cout << "Starting thread again with an artificially unreliable execution time ..." << endl;
  thread.setArtificalDelay( true );
  thread.start();
  thread.wait(2*1000);
  cout << "Stopping thread ..." << endl;
  thread.stop();

  return 0;
}
