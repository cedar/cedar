/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ParameterBase.cpp

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

// LOCAL INCLUDES
#include "auxiliaries/ParameterBase.h"
#include "auxiliaries/exceptions.h"
#include "auxiliaries/Parameter.h"
#include "auxiliaries/NumericParameter.h"
#include "auxiliaries/Configurable.h"
#include "auxiliaries/assert.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::ParameterBase::ParameterBase(cedar::aux::Configurable *pOwner, const std::string& name, bool hasDefault)
:
mpOwner(pOwner),
mHasDefault(hasDefault),
mConstant(false),
mIsHidden(false),
mReferenceCount(0)
{
  CEDAR_ASSERT(this->mpOwner != NULL);
  this->setName(name);

  this->mpOwner->registerParameter(cedar::aux::ParameterBasePtr(this));
}

cedar::aux::ParameterBase::~ParameterBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void intrusive_ptr_add_ref(cedar::aux::ParameterBase *pObject)
{
  pObject->mReferenceCount += 1;
}

void intrusive_ptr_release(cedar::aux::ParameterBase *pObject)
{
  pObject->mReferenceCount -= 1;

  if (pObject->mReferenceCount == 0)
  {
    delete pObject;
  }
}

void cedar::aux::ParameterBase::emitChangedSignal()
{
  emit valueChanged();
}

bool cedar::aux::ParameterBase::isHidden() const
{
  return this->mIsHidden;
}

void cedar::aux::ParameterBase::setHidden(bool hide)
{
  this->mIsHidden = hide;
}

bool cedar::aux::ParameterBase::getReadAutomatically() const
{
  return this->mAutoRead;
}

void cedar::aux::ParameterBase::setReadAutomatically(bool value)
{
  this->mAutoRead = value;
}

bool cedar::aux::ParameterBase::getHasDefault() const
{
  return this->mHasDefault;
}

void cedar::aux::ParameterBase::setHasDefault(bool value)
{
  this->mHasDefault = value;
}

bool cedar::aux::ParameterBase::isConstant() const
{
  return this->mConstant;
}

void cedar::aux::ParameterBase::setConstant(bool value)
{
  this->mConstant = value;
}
