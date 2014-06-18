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

    File:        Element.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 17

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/Element.h"
#include "cedar/processing/Group.h"

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::Element::Element()
{
  _mName->setDefault("element");
  _mName->setValidator(boost::bind(&cedar::proc::Element::validateName, this, _1));
}

cedar::proc::Element::~Element()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::Element::updateTriggerChains(std::set<cedar::proc::Trigger*>& /*visited*/)
{
  // empty default implementation
}

void cedar::proc::Element::setNetwork(cedar::proc::GroupPtr network)
{
  this->setGroup(network);
}

cedar::proc::GroupPtr cedar::proc::Element::getNetwork()
{
  return this->getGroup();
}

cedar::proc::ConstGroupPtr cedar::proc::Element::getNetwork() const
{
  return this->getGroup();
}

void cedar::proc::Element::setGroup(cedar::proc::GroupPtr group)
{
  // set the parent registry
  this->mRegisteredAt = group;

  // emit signal
  this->mGroupChanged();
}

cedar::proc::GroupPtr cedar::proc::Element::getGroup()
{
  return this->mRegisteredAt.lock();
}

cedar::proc::ConstGroupPtr cedar::proc::Element::getGroup() const
{
  return this->mRegisteredAt.lock();
}

void cedar::proc::Element::validateName(const std::string& newName) const
{
  if (newName.find('.') != std::string::npos)
  {
    CEDAR_THROW(cedar::aux::ValidationFailedException, "This name contains an invalid character (\".\")");
  }

  // nothing to do if the name is the same
  if (newName == this->getName())
  {
    return;
  }

  if (cedar::proc::ConstGroupPtr network = this->getGroup())
  {
    if (network->nameExists(newName))
    {
      CEDAR_THROW
      (
        cedar::aux::ValidationFailedException,
        "There is already an element with the name \"" + newName + "\" in this element's group."
      );
    }
  }
}

void cedar::proc::Element::copyFrom(cedar::aux::ConstConfigurablePtr src)
{
  // check type
  if (typeid(*this) != typeid(*src))
  {
    CEDAR_THROW(cedar::aux::TypeMismatchException, "cannot copy if types do not match");
  }
  cedar::aux::ConfigurationNode root;
  src->writeConfiguration(root);
  root.put("name", this->getName());
  this->readConfiguration(root);
}

void cedar::proc::Element::copyTo(cedar::aux::ConfigurablePtr target) const
{
  // check type
  if (typeid(*this) != typeid(*target))
  {
    CEDAR_THROW(cedar::aux::TypeMismatchException, "cannot copy if types do not match");
  }
  cedar::aux::ConfigurationNode root;
  this->writeConfiguration(root);
  if (auto elem = boost::dynamic_pointer_cast<cedar::proc::Element>(target))
  {
    root.put("name", elem->getName());
    target->readConfiguration(root);
  }
}
