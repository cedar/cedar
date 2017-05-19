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

    File:        ProjectionMappingParameter.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2011 11 24

    Description: Parameter type for the projection mapping.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/ProjectionMappingParameter.h"
#include "cedar/processing/ProjectionMapping.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <map>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::ProjectionMappingParameter::ProjectionMappingParameter
(
  cedar::aux::Configurable *pOwner,
  const std::string& name
)
:
cedar::aux::Parameter(pOwner, name, false),
mValues(new cedar::proc::ProjectionMapping())
{
}

cedar::proc::ProjectionMappingParameter::ProjectionMappingParameter
(
  cedar::aux::Configurable *pOwner,
  const std::string& name,
  const cedar::proc::ProjectionMappingPtr& defaults
)
:
cedar::aux::Parameter(pOwner, name, true),
mValues(defaults)
{
}

//!@brief Destructor
cedar::proc::ProjectionMappingParameter::~ProjectionMappingParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::ProjectionMappingParameter::readFromNode(const cedar::aux::ConfigurationNode& root)
{
  mValues->initialize(0);
  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin(); iter != root.end(); ++iter)
  {
    unsigned int key;
    cedar::aux::fromString<unsigned int>(key, iter->first, std::dec);
    unsigned int value = iter->second.get_value<unsigned int>();

    mValues->addMapping(key, value);
  }

  mValues->updateValidity();
}

void cedar::proc::ProjectionMappingParameter::writeToNode(cedar::aux::ConfigurationNode& root) const
{
  cedar::aux::ConfigurationNode map_node;
  for
  (
    std::map<unsigned int, unsigned int>::iterator iter = mValues->begin();
    iter != mValues->end();
    ++iter
  )
  {
    cedar::aux::ConfigurationNode value_node;
    value_node.put_value(iter->second);
    map_node.push_back(cedar::aux::ConfigurationNode::value_type(cedar::aux::toString<unsigned int>(iter->first), value_node));
  }
  root.push_back(cedar::aux::ConfigurationNode::value_type(getName(), map_node));
}

void cedar::proc::ProjectionMappingParameter::changeMapping(unsigned int inputIndex, unsigned int outputIndex)
{
  mValues->changeMapping(inputIndex, outputIndex);
  emitChangedSignal();
}

void cedar::proc::ProjectionMappingParameter::drop(unsigned int inputIndex)
{
  mValues->drop(inputIndex);
  emitChangedSignal();
}

void cedar::proc::ProjectionMappingParameter::setOutputDimensionality(unsigned int dimensionality)
{
  mValues->setOutputDimensionality(dimensionality);
  emitPropertyChangedSignal();
}

void cedar::proc::ProjectionMappingParameter::initialize(unsigned int numberOfMappings)
{
  mValues->initialize(numberOfMappings);
  emitPropertyChangedSignal();
}

void cedar::proc::ProjectionMappingParameter::makeDefault()
{
}

const cedar::proc::ProjectionMappingPtr& cedar::proc::ProjectionMappingParameter::getValue() const
{
  return mValues;
}
