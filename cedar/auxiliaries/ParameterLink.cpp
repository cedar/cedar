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
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::ParameterLink::setLinkedParameters(cedar::aux::ParameterPtr left, cedar::aux::ParameterPtr right)
{
  //!@todo Exception
  CEDAR_ASSERT(this->canLink(left, right));

  // disconnect if parameters are already set
  if (this->mLeft)
  {
    this->mLeft->setLinked(false);

    QObject::disconnect(this->mLeft.get(), SIGNAL(valueChanged()), this, SLOT(leftChanged()));
    QObject::disconnect(this->mLeft.get(), SIGNAL(propertyChanged()), this, SLOT(leftPropertiesChanged()));
  }
  if (this->mRight)
  {
    this->mRight->setLinked(false);

    QObject::disconnect(this->mRight.get(), SIGNAL(valueChanged()), this, SLOT(rightChanged()));
    QObject::disconnect(this->mRight.get(), SIGNAL(propertyChanged()), this, SLOT(rightPropertiesChanged()));
  }

  this->mLeft = left;
  this->mRight = right;

  this->mLeft->setLinked(true);
  this->mRight->setLinked(true);

  QObject::connect(this->mLeft.get(), SIGNAL(valueChanged()), this, SLOT(leftChanged()));
  QObject::connect(this->mLeft.get(), SIGNAL(propertyChanged()), this, SLOT(leftPropertiesChanged()));
  QObject::connect(this->mRight.get(), SIGNAL(valueChanged()), this, SLOT(rightChanged()));
  QObject::connect(this->mRight.get(), SIGNAL(propertyChanged()), this, SLOT(rightPropertiesChanged()));
}

void cedar::aux::ParameterLink::leftPropertiesChanged()
{
  this->applyProperties(this->getLeft(), this->getRight());
}

void cedar::aux::ParameterLink::rightPropertiesChanged()
{
  this->applyProperties(this->getRight(), this->getLeft());
}

void cedar::aux::ParameterLink::applyProperties(cedar::aux::ConstParameterPtr source, cedar::aux::ParameterPtr target)
{
  target->setConstant(source->isConstant());
}

cedar::aux::ParameterPtr cedar::aux::ParameterLink::getLeft() const
{
  return this->mLeft;
}

cedar::aux::ParameterPtr cedar::aux::ParameterLink::getRight() const
{
  return this->mRight;
}

bool cedar::aux::ParameterLink::canLink(cedar::aux::ParameterPtr left, cedar::aux::ParameterPtr right)
{
  return this->checkIfLinkable(left, right);
}
