/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        NetworkTimer.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.ruhr-uni-bochum.de
    Date:        2013 11 7

    Description: Singleton class for a central time in the  network.

    Credits:

======================================================================================================================*/

//CEDAR INCLUDES
#include "cedar/auxiliaries/NetworkTimer.h"

cedar::aux::NetworkTimer::NetworkTimer()
:mRunning(false)
{
  this->reset();
}

cedar::aux::NetworkTimer::~NetworkTimer()
{

}
void cedar::aux::NetworkTimer::start()
{
  // if timer on zero -> hold it on zero
  if (mStart==mStop)
  {
    mStart = QTime::currentTime();
  }
  mRunning = true;
}

void cedar::aux::NetworkTimer::reset()
{
  mStart = QTime::currentTime();
  mStop = mStart;
}


void cedar::aux::NetworkTimer::stop()
{
  mStop = QTime::currentTime();
  mRunning = false;
}

unsigned int cedar::aux::NetworkTimer::getTime()
{
  if (mRunning)
  {
    return mStart.msecsTo(QTime::currentTime());
  }
  return mStart.msecsTo(mStop);
}



