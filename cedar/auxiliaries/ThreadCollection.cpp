/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

void cedar::aux::ThreadCollection::addThread(const std::string& name, cedar::aux::ThreadWrapperPtr thread)
{
  QWriteLocker locker(mpListLock);
  if (mThreads.find(name) != mThreads.end())
  {
    CEDAR_THROW(cedar::aux::DuplicateNameException, "Thread of name \"" + name + "\" is already registered.");
  }
  mThreads[name] = thread;
}

void cedar::aux::ThreadCollection::startAll()
{
  QReadLocker locker(mpListLock);
  for (auto thread : mThreads)
  {
    thread.second->start();
  }
}

void cedar::aux::ThreadCollection::stopAll()
{
  QReadLocker locker(mpListLock);
  //First performing a requestStop() to stop all thread as fast as possible without blocking
  for (auto thread : mThreads)
  {
    thread.second->requestStop();
  }

  //Waiting for each thread stopped.
  for (auto thread : mThreads)
  {
    thread.second->stop();
  }
}

void cedar::aux::ThreadCollection::remove(const std::string& thread)
{
  QWriteLocker locker(mpListLock);
  auto it = mThreads.find(thread);
  if (it !=  mThreads.end())
  {
    mThreads.erase(it);
  }
  else
  {
    CEDAR_THROW(cedar::aux::NotFoundException, "Thread of name \"" + thread + "\" is not registered.");
  }
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

std::map<std::string, cedar::aux::ThreadWrapperPtr>& cedar::aux::ThreadCollection::getCollection()
{
  return this->mThreads;
}

const std::map<std::string, cedar::aux::ThreadWrapperPtr>& cedar::aux::ThreadCollection::getCollection() const
{
  return this->mThreads;
}

bool cedar::aux::ThreadCollection::isRegistered(const std::string& name) const
{
  return this->mThreads.find(name) != this->mThreads.end();
}
