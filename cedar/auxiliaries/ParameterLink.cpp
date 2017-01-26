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

    File:        ParameterLink.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 02 18

    Description: Source file for the class cedar::aux::ParameterLink.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/ParameterLink.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::ParameterLink::ParameterLink()
{
}

cedar::aux::ParameterLink::~ParameterLink()
{
  // disconnect if parameters are set
  if (this->mSource)
  {
    this->unsetSource();
  }

  if (this->mTarget)
  {
    this->unsetTarget();
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::ParameterLink::callSourceChanged()
{
  if (this->getSource() && this->getTarget())
  {
    this->sourceChanged();
  }
}

void cedar::aux::ParameterLink::callTargetChanged()
{
  if (this->getSource() && this->getTarget())
  {
    this->targetChanged();
  }
}

void cedar::aux::ParameterLink::setSource(cedar::aux::ParameterPtr parameter)
{
  // disconnect if parameters are already set
  if (this->mSource)
  {
    this->unsetSource();
  }

  this->mSource = parameter;
  this->mSource->setLinked(true);
  QObject::connect(this->mSource.get(), SIGNAL(valueChanged()), this, SLOT(callSourceChanged()));
  QObject::connect(this->mSource.get(), SIGNAL(propertyChanged()), this, SLOT(sourcePropertiesChanged()));

  if (this->mSource && this->mTarget)
  {
    this->sourceChanged();
  }
}

void cedar::aux::ParameterLink::unsetSource()
{
  this->mSource->setLinked(false);

  QObject::disconnect(this->mSource.get(), SIGNAL(valueChanged()), this, SLOT(callSourceChanged()));
  QObject::disconnect(this->mSource.get(), SIGNAL(propertyChanged()), this, SLOT(sourcePropertiesChanged()));
}

void cedar::aux::ParameterLink::setTarget(cedar::aux::ParameterPtr parameter)
{
  // disconnect if parameters are already set
  if (this->mTarget)
  {
    this->unsetTarget();
  }

  this->mTarget = parameter;
  this->mTarget->setLinked(true);

  QObject::connect(this->mTarget.get(), SIGNAL(valueChanged()), this, SLOT(callTargetChanged()));
  QObject::connect(this->mTarget.get(), SIGNAL(propertyChanged()), this, SLOT(targetPropertiesChanged()));

  if (this->mSource && this->mTarget)
  {
    this->sourceChanged();
  }
}

void cedar::aux::ParameterLink::unsetTarget()
{
  this->mTarget->setLinked(false);

  QObject::disconnect(this->mTarget.get(), SIGNAL(valueChanged()), this, SLOT(callTargetChanged()));
  QObject::disconnect(this->mTarget.get(), SIGNAL(propertyChanged()), this, SLOT(targetPropertiesChanged()));
}

void cedar::aux::ParameterLink::setLinkedParameters(cedar::aux::ParameterPtr source, cedar::aux::ParameterPtr target)
{
  CEDAR_ASSERT(source);
  CEDAR_ASSERT(target);
  if (!this->canLink(source, target))
  {
    CEDAR_THROW
    (
      cedar::aux::ParameterLinkingException,
      "Cannot link parameters " + source->getName() + " and " + target->getName()
    );
  }

  this->setSource(source);
  this->setTarget(target);
}

void cedar::aux::ParameterLink::sourcePropertiesChanged()
{
  this->applyProperties(this->getSource(), this->getTarget());

  // also, if the properties changed, we need to copy the values
  this->sourceChanged();
}

void cedar::aux::ParameterLink::targetPropertiesChanged()
{
  this->applyProperties(this->getTarget(), this->getSource());

  // also, if the properties changed, we need to copy the values
  this->targetChanged();
}

void cedar::aux::ParameterLink::applyProperties(cedar::aux::ConstParameterPtr source, cedar::aux::ParameterPtr target)
{
  if (target && source)
  {
    target->setConstant(source->isConstant());
  }
}

cedar::aux::ParameterPtr cedar::aux::ParameterLink::getSource() const
{
  return this->mSource;
}

cedar::aux::ParameterPtr cedar::aux::ParameterLink::getTarget() const
{
  return this->mTarget;
}

bool cedar::aux::ParameterLink::canLink(cedar::aux::ParameterPtr source, cedar::aux::ParameterPtr target)
{
  return this->checkIfLinkable(source, target);
}
