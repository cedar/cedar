/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        Thread.cpp

 ----- Author:      Christian Faubel
                    Bjoern Weghenkel
 ----- Email:       christian.faubel@ini.rub.de
                    bjoern.weghenkel@ini.rub.de
 ----- Date:        2010 11 16

 ----- Description: Implementation for the @em cedar::aux::Thread class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "Thread.h"
#include <QTime>

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace std;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::Thread::Thread()
{
  mIdleTime = 100;
  _mName = string("thread");
  mStop = false;
}

cedar::aux::Thread::Thread(unsigned idleTime) : mIdleTime(idleTime)
{
  _mName = string("thread");
  mStop = false;
}

cedar::aux::Thread::~Thread()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::Thread::stop( unsigned time )
{
	mStop = true;
	wait( time );
}

void cedar::aux::Thread::run(void)
{
	mStop = false;

	// some auxiliary variables
	QTime lastWakeup = QTime::currentTime();
	QTime scheduledWakeup = lastWakeup.addMSecs( 1000 );
	QTime tmpTime;
	srand ( QTime::currentTime().msec() );

	while( !mStop ) {

		// sleep until next wake up time
		msleep( std::max<int>( 0, QTime::currentTime().msecsTo( scheduledWakeup ) ) );

		// print wake up time
		//QTime currentWakeup = QTime::currentTime();
		//cout << currentWakeup.second() << ":" << currentWakeup.msec() << endl;

		// determine number of time steps since last run
		QTime wakeupMax = scheduledWakeup.addMSecs( 1000 ); // end of current time window
		tmpTime = lastWakeup.addMSecs( 1000 );
		unsigned stepsTaken = 0;
		while( tmpTime < wakeupMax ) {
			tmpTime = tmpTime.addMSecs( 1000 );
			stepsTaken++;
		}
		lastWakeup = scheduledWakeup;	// remember the current wakeup time
		scheduledWakeup = tmpTime;

		// print warning if time steps have been skipped
		if( stepsTaken > 1 )
			cout << "WARNING: " << stepsTaken << " time steps taken at once! "
			<< "Your system might be too slow for an execution interval of "
			<< mIdleTime << " milliseconds. Consider using a longer interval!"
			<< endl;

		// call step function
		step( stepsTaken * mIdleTime );

		// if the execution lasted unexpectedly long, we'd like to wake up for
		// the next regular time step
		while( scheduledWakeup < QTime::currentTime() )
			scheduledWakeup = scheduledWakeup.addMSecs( 1000 );
	}

	mStop = false;
	return;
}
