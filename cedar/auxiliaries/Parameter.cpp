/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/threadingUtilities.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <QReadLocker>
#include <QWriteLocker>


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::Parameter::Parameter(cedar::aux::Configurable *pOwner, const std::string& name, bool hasDefault)
:
mpOwner(nullptr),
mHasDefault(hasDefault),
mConstant(false),
mIsHidden(false),
mChanged(false),
mAdvanced(false),
mIsLinked(false),
mLastLockType(cedar::aux::LOCK_TYPE_DONT_LOCK),
mpLock(new QReadWriteLock())
{
  this->setName(name);

  if (pOwner != nullptr)
  {
    this->setOwner(pOwner);
  }
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

std::string cedar::aux::Parameter::childIndexToString(size_t i) const
{
  return cedar::aux::toString(i);
}

size_t cedar::aux::Parameter::childStringToIndex(std::string str) const
{
  return cedar::aux::fromString<size_t>(str);
}

cedar::aux::ConstConfigurablePtr cedar::aux::Parameter::getSingleConfigurableChild() const
{
  return cedar::aux::ConfigurablePtr();
}

cedar::aux::ConfigurablePtr cedar::aux::Parameter::getSingleConfigurableChild()
{
  // const casts prevent code duplication
  return boost::const_pointer_cast<cedar::aux::Configurable>(const_cast<cedar::aux::ConstParameter*>(this)->getSingleConfigurableChild());
}

bool cedar::aux::Parameter::hasSingleConfigurableChild() const
{
  return false;
}

bool cedar::aux::Parameter::canHaveConfigurableChildren() const
{
  return false;
}

size_t cedar::aux::Parameter::getNumberOfConfigurableChildren() const
{
  return 0;
}

cedar::aux::ConfigurablePtr cedar::aux::Parameter::getConfigurableChild(size_t i)
{
  // this const-cast avoids duplicate code
  return boost::const_pointer_cast<cedar::aux::Configurable>(this->retrieveConfigurableChild(i));
}

cedar::aux::ConstConfigurablePtr cedar::aux::Parameter::getConfigurableChild(size_t i) const
{
  return this->retrieveConfigurableChild(i);
}

cedar::aux::ConstConfigurablePtr cedar::aux::Parameter::retrieveConfigurableChild(size_t) const
{
  // default implementation returns an empty pointer; this should never be called.
  return cedar::aux::ConstConfigurablePtr();
}

bool cedar::aux::Parameter::isLinked() const
{
  return this->mIsLinked;
}

void cedar::aux::Parameter::setLinked(bool linked)
{
  this->mIsLinked = linked;
}

void cedar::aux::Parameter::unsetOwner()
{
  if(this->mpOwner != nullptr)
  {
    this->mpOwner->unregisterParameter(this);
    this->mpOwner = nullptr;
  }
}

void cedar::aux::Parameter::setOwner(cedar::aux::Configurable *pOwner)
{
  this->unsetOwner();
  this->mpOwner = pOwner;

  this->mpOwner->registerParameter(this);
}

void cedar::aux::Parameter::setName(const std::string& name)
{
  QWriteLocker locker(this->mName.getLockPtr());
  std::string old_name = this->mName.member();
  this->mName.member() = name;
  locker.unlock();

  this->signalNameChanged(old_name, name);
}

void cedar::aux::Parameter::copyValueFrom(cedar::aux::ConstParameterPtr /* other */)
{
  CEDAR_THROW
  (
    cedar::aux::NotImplementedException,
    "Copying values from this type of parameter ("
    + cedar::aux::objectTypeToString(this) +
    ") is not supported because the parameter type does not implement the copyValueFrom method."
  );
}

bool cedar::aux::Parameter::canCopyFrom(cedar::aux::ConstParameterPtr /* other */) const
{
  return false;
}

void cedar::aux::Parameter::addDeprecatedName(const std::string& deprecatedName)
{
  this->mpOwner->addDeprecatedName(this, deprecatedName);
}

void cedar::aux::Parameter::lockForWrite() const
{
  std::set<QReadWriteLock*> locks;
  this->appendLocks(locks);
  this->mLastLockType = cedar::aux::LOCK_TYPE_WRITE;
  cedar::aux::lock(locks, this->mLastLockType);
}

void cedar::aux::Parameter::lockForRead() const
{
  std::set<QReadWriteLock*> locks;
  this->appendLocks(locks);
  this->mLastLockType = cedar::aux::LOCK_TYPE_READ;
  cedar::aux::lock(locks, this->mLastLockType);
}

void cedar::aux::Parameter::unlock() const
{
  std::set<QReadWriteLock*> locks;
  this->appendLocks(locks);
  cedar::aux::unlock(locks, this->mLastLockType);
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
  bool was_constant = this->isConstant();
  if (was_constant != value)
  {
    this->mConstant = value;

    emit propertyChanged();
  }
}

std::string cedar::aux::Parameter::getName() const
{
  QReadLocker locker(this->mName.getLockPtr());
  std::string name = this->mName.member();

  return name;
}
