/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        Thread.cpp

 ----- Author:      Christian Faubel
 ----- Email:       christian.faubel@ini.rub.de
 ----- Date:        2010 10 12

 ----- Description: Implementation for the @em cedar::aux::Thread class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "Thread.h"

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

cedar::aux::Thread::Thread(int idleTime) : mIdleTime(idleTime)
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

void cedar::aux::Thread::stop()
{
  mStop = true;
}

void cedar::aux::Thread::run(void)
{
  mStop = false;

  while(!mStop)
  {
    usleep(mIdleTime);
    step();
  }
}
