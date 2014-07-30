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

    File:        ThreadCollection.cpp

    Maintainer:  Christian Bodenstein
    Email:       Christian.bodenstein@ini.rub.de
    Date:        2013 07 09

    Description: Implementation of the @em cedar::aux::ThreadCollection class.

    Credits:

======================================================================================================================*/



// CEDAR INCLUDES
#include "cedar/auxiliaries/ThreadCollection.h"
#include "cedar/auxiliaries/ThreadWrapper.h"


cedar::aux::ThreadCollection::ThreadCollection()
{
  mpListLock = new QReadWriteLock();
}

cedar::aux::ThreadCollection::~ThreadCollection()
{
  delete mpListLock;
}

void cedar::aux::ThreadCollection::addThread(cedar::aux::ThreadWrapperPtr thread)
{
  QWriteLocker locker(mpListLock);
  mThreads.push_back(thread);
}

void cedar::aux::ThreadCollection::startAll()
{
  QWriteLocker locker(mpListLock);
  for (unsigned int i = 0; i < mThreads.size(); i++)
  {
    mThreads[i]->start();
  }
}

void cedar::aux::ThreadCollection::stopAll()
{
  QWriteLocker locker(mpListLock);
  //First performing a requestStop() to stop all thread as fast as possible without blocking
  for (unsigned int i = 0; i < mThreads.size(); i++)
  {
    mThreads[i]->requestStop();
  }

  //Waiting for each thread stopped.
  for (unsigned int i = 0; i < mThreads.size(); i++)
  {
    mThreads[i]->stop();
  }
}

void cedar::aux::ThreadCollection::remove(int index)
{
  QWriteLocker locker(mpListLock);
  mThreads.erase(mThreads.begin()+index);
}

void cedar::aux::ThreadCollection::removeAll()
{
  QWriteLocker locker(mpListLock);
  mThreads.clear();
}

unsigned int cedar::aux::ThreadCollection::size() const
{
  QReadLocker locker(mpListLock);
  return mThreads.size();
}
