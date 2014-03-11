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

    File:        Parameter.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 01

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/threadingUtilities.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::Parameter::Parameter(cedar::aux::Configurable *pOwner, const std::string& name, bool hasDefault)
:
mpOwner(pOwner),
mHasDefault(hasDefault),
mConstant(false),
mIsHidden(false),
mChanged(false),
mAdvanced(false),
mpLock(new QReadWriteLock())
{
  CEDAR_ASSERT(this->mpOwner != NULL);
  this->setName(name);

  this->mpOwner->registerParameter(this);
}

cedar::aux::Parameter::~Parameter()
{
  if (this->mpLock != NULL)
  {
    delete this->mpLock;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::Parameter::addDeprecatedName(const std::string& deprecatedName)
{
  this->mpOwner->addDeprecatedName(this, deprecatedName);
}

void cedar::aux::Parameter::lockForWrite() const
{
  std::set<QReadWriteLock*> locks;
  this->appendLocks(locks);
  cedar::aux::lock(locks, cedar::aux::LOCK_TYPE_WRITE);
}

void cedar::aux::Parameter::lockForRead() const
{
  std::set<QReadWriteLock*> locks;
  this->appendLocks(locks);
  cedar::aux::lock(locks, cedar::aux::LOCK_TYPE_READ);
}

void cedar::aux::Parameter::unlock() const
{
  std::set<QReadWriteLock*> locks;
  this->appendLocks(locks);
  cedar::aux::unlock(locks);
}

void cedar::aux::Parameter::appendLocks(std::set<QReadWriteLock*>& locks) const
{
  locks.insert(this->mpLock);
}

void cedar::aux::Parameter::removeLocks(std::set<QReadWriteLock*>& locks) const
{
  locks.erase(locks.find(this->mpLock));
}

void cedar::aux::Parameter::setChangedFlag(bool changed)
{
  if (this->mChanged != changed)
  {
    this->mChanged = changed;
    emit changedFlagChanged();
  }
}

void cedar::aux::Parameter::emitChangedSignal()
{
  this->setChangedFlag(true);
  emit valueChanged();
}

void cedar::aux::Parameter::emitPropertyChangedSignal()
{
  this->setChangedFlag(true);
  emit propertyChanged();
}

bool cedar::aux::Parameter::isHidden() const
{
  return this->mIsHidden;
}

void cedar::aux::Parameter::setHidden(bool hide)
{
  this->mIsHidden = hide;
}

bool cedar::aux::Parameter::getReadAutomatically() const
{
  return this->mAutoRead;
}

void cedar::aux::Parameter::setReadAutomatically(bool value)
{
  this->mAutoRead = value;

  emit propertyChanged();
}

bool cedar::aux::Parameter::getHasDefault() const
{
  return this->mHasDefault;
}

void cedar::aux::Parameter::setHasDefault(bool value)
{
  this->mHasDefault = value;

  emit propertyChanged();
}

bool cedar::aux::Parameter::isConstant() const
{
  return this->mConstant;
}

void cedar::aux::Parameter::setConstant(bool value)
{
  this->mConstant = value;

  emit propertyChanged();
}

const std::string& cedar::aux::Parameter::getName() const
{
  return this->mName;
}

void cedar::aux::Parameter::setName(const std::string& name)
{
  this->mName = name;
}
