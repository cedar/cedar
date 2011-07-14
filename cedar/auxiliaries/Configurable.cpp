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

    File:        Configurable.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 06

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "auxiliaries/Configurable.h"
#include "auxiliaries/ParameterBase.h"
#include "auxiliaries/Parameter.h"
#include "auxiliaries/exceptions.h"
// PROJECT INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::Configurable::Configurable()
:
mName(new cedar::aux::StringParameter("name", ""))
{
  this->registerParameter(mName);
}


cedar::aux::Configurable::~Configurable()
{
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::aux::Configurable::registerParameter(cedar::aux::ParameterBasePtr parameter)
{
  //! @todo check for duplicate names
  //! @todo make sure there are no dots in the name; make a global function for name checks.
  this->mParameters[parameter->getName()] = parameter;
}

const cedar::aux::Configurable::ParameterMap& cedar::aux::Configurable::getParameters() const
{
  return this->mParameters;
}

cedar::aux::Configurable::ParameterMap& cedar::aux::Configurable::getParameters()
{
  return this->mParameters;
}

void cedar::aux::Configurable::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  for (ParameterMap::iterator iter = this->mParameters.begin(); iter != this->mParameters.end(); ++iter)
  {
    try
    {
      const cedar::aux::ConfigurationNode& value = node.get_child(iter->second->getName());
      iter->second->set(value);
    }
    catch (const boost::property_tree::ptree_bad_path&)
    {
      if (! iter->second->getHasDefault())
      {
        CEDAR_THROW(cedar::aux::ParameterNotFoundException, "Config node " + iter->second->getName() + " not found!");
      }
      else
      {
        iter->second->makeDefault();
      }
    }
  }
}

const cedar::aux::Configurable::Children& cedar::aux::Configurable::configurableChildren() const
{
  return this->mChildren;
}

void cedar::aux::Configurable::addConfigurableChild(const std::string& name, cedar::aux::ConfigurablePtr child)
{
  if (this->mChildren.find(name) != this->mChildren.end())
  {
    CEDAR_THROW(cedar::aux::DuplicateNameException, "There is already a configurable child with the name \""
                                                    + name + "\".");
  }
  this->mChildren[name] = child;
}

void cedar::aux::Configurable::setName(const std::string& name)
{
  this->mName->set(name);
}

const std::string& cedar::aux::Configurable::getName() const
{
  return this->mName->get();
}
