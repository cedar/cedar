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

    File:        ThreadCollection.h

    Maintainer:  Christian Bodenstein
    Email:       Christian.bodenstein@ini.rub.de
    Date:        2013 07 09

    Description: Header for the @em cedar::aux::ThreadCollection class.

    Credits:

======================================================================================================================*/



#ifndef CEDAR_AUX_THREADCOLLECTION_H_
#define CEDAR_AUX_THREADCOLLECTION_H_

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/ThreadCollection.fwd.h"
#include "cedar/auxiliaries/ThreadWrapper.fwd.h"

// SYSTEM INCLUDES
#include <QReadWriteLock>
#include <vector>

/*!@brief This class manages multiple threads.
 *            It provides some functions and tools for easily start, stop and access the threads.
 */
class cedar::aux::ThreadCollection
{
public:
  //----------------------------------------------------------------------------
  // constructors and destructor
  //----------------------------------------------------------------------------

  //!@brief The constructor.
  ThreadCollection();

  //!@brief The destructor.
   ~ThreadCollection();

  //----------------------------------------------------------------------------
  // public methods
  //----------------------------------------------------------------------------

public:
  //!@brief Adds a new thread to the ThreadCollection.
  void addThread(cedar::aux::ThreadWrapperPtr thread);

  //!@brief Starts all threads in the ThreadCollection.
  void startAll();

  //!@brief Stops all threads in the ThreadCollection.
  void stopAll();

  //!@brief Removes all threads from the ThreadCollection.
  void removeAll();

  //!@brief Removes a thread from the collection.
  void remove(int index);

  //!@brief Returns the number of Threads in the ThreadCollection.
  unsigned int size() const;

  //!@brief Returns thread i of the ThreadCollection and cast it to T.
  template <typename T>
  boost::shared_ptr<T> get(unsigned int index) const
  {
    QReadLocker locker(mpListLock);
    return boost::static_pointer_cast<T>(mThreads[index]);
  }

  //----------------------------------------------------------------------------
  // private members
  //----------------------------------------------------------------------------
private:
  std::vector<cedar::aux::ThreadWrapperPtr> mThreads;
  QReadWriteLock* mpListLock;
};

#endif /* CEDAR_AUX_THREADCOLLECTION_H_ */
