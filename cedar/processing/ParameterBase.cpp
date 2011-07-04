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
#include "processing/ParameterBase.h"
#include "processing/exceptions.h"
#include "processing/Parameter.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::ParameterBase::ParameterBase(const std::string& name)
{
  this->setName(name);
}

cedar::proc::ParameterBase::~ParameterBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::proc::ParameterBase::getReadAutomatically() const
{
  return this->mAutoRead;
}

void cedar::proc::ParameterBase::setReadAutomatically(bool value)
{
  this->mAutoRead = value;
}

bool cedar::proc::ParameterBase::getHasDefault() const
{
  return this->mHasDefault;
}

void cedar::proc::ParameterBase::setHasDefault(bool value)
{
  this->mHasDefault = value;
}

bool cedar::proc::ParameterBase::isConstant() const
{
  return this->mConstant;
}

void cedar::proc::ParameterBase::setConstant(bool value)
{
  this->mConstant = value;
}

void cedar::proc::ParameterBase::set(const cedar::proc::ConfigurationNode& node)
{
  if (dynamic_cast<DoubleParameter*>(this))
  {
    dynamic_cast<DoubleParameter*>(this)->set(node.get_value<double>());
  }
  else if (dynamic_cast<StringParameter*>(this))
  {
    dynamic_cast<StringParameter*>(this)->set(node.get_value<std::string>());
  }
  else
  {
    CEDAR_THROW(cedar::proc::UnhandledTypeException, "Cannot set parameter value: the type of the parameter "
                                                     + this->getName() + "is not handled.");
  }
}
