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

    File:        Channel.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 23

    Description: Communication channel for a component or device (e.g., serial communication).

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/Channel.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::Channel::Channel()
:
mUseCount(0)
{
}

cedar::dev::Channel::~Channel()
{
  if (!mDestructWasPrepared)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "You forgot to call prepareChannelDestructAbsolutelyRequired() in the child's destructor!",
      CEDAR_CURRENT_FUNCTION_NAME
    );
  }
}

// needs to be called by the end-point of the inheritance tree
void cedar::dev::Channel::prepareChannelDestructAbsolutelyRequired()
{
  if (this->isOpen())
  {
    close();
  }

  mDestructWasPrepared= true;
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

std::ostream& cedar::dev::operator<<(std::ostream& stream, const cedar::dev::Channel& channel)
{
  stream << "Channel @ " << (&channel) << " of type " << cedar::aux::objectTypeToString(&channel) << std::endl;
  return stream;
}

std::ostream& cedar::dev::operator<<(std::ostream& stream, cedar::dev::ChannelPtr channel)
{
  stream << (*channel);
  return stream;
}

std::ostream& cedar::dev::operator<<(std::ostream& stream, cedar::dev::ConstChannelPtr channel)
{
  stream << (*channel);
  return stream;
}

void cedar::dev::Channel::increaseUseCount()
{
  QWriteLocker locker(&this->mLock);
  bool open = (this->mUseCount == 0);
  ++this->mUseCount;
  locker.unlock();

  if (open)
  {
    preOpenHook();
    openHook();
    postOpenHook();
  }
}

void cedar::dev::Channel::decreaseUseCount()
{
  QWriteLocker locker(&this->mLock);
  if (this->mUseCount == 0)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Not all your open() calls are matched by close() calls. Please check your code.",
      CEDAR_CURRENT_FUNCTION_NAME
    );
    return;
  }

  --this->mUseCount;
  bool close = (this->mUseCount == 0);
  locker.unlock();

  if (close)
  {
    preCloseHook();
    closeHook();
    postCloseHook();
  }
}

void cedar::dev::Channel::open()
{
  this->increaseUseCount();
}

void cedar::dev::Channel::close()
{
  this->decreaseUseCount();
}

bool cedar::dev::Channel::isOpen()
{
    return this->mUseCount != 0;
}
