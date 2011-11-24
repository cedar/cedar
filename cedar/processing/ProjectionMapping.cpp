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

    File:        ProjectionMapping.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2011 11 18

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/processing/ProjectionMapping.h"
#include "cedar/processing/exceptions.h"

// PROJECT INCLUDES
#include "cedar/auxiliaries/ExceptionBase.h"

// SYSTEM INCLUDES
#include <climits>

unsigned int cedar::proc::ProjectionMapping::msDropIndex = UINT_MAX;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::ProjectionMapping::addMapping(unsigned int inputIndex, unsigned int outputIndex)
{
  if (!this->isMappingBijective(inputIndex, outputIndex))
  {
    CEDAR_THROW(cedar::proc::ProjectionMappingNotBijectiveException, "With the mapping you want to add, the overall mapping does not stay bijective.");
  }

  mMapping[inputIndex] = outputIndex;
}

void cedar::proc::ProjectionMapping::dropDimension(unsigned int inputIndex)
{
  mMapping[inputIndex] = msDropIndex;
}

void cedar::proc::ProjectionMapping::removeMapping(unsigned int inputIndex)
{
  mMapping.erase(inputIndex);
}

void cedar::proc::ProjectionMapping::clear()
{
  mMapping.clear();
}

unsigned int cedar::proc::ProjectionMapping::lookUp(unsigned int inputIndex)
{
  std::map<unsigned int, unsigned int>::const_iterator iter = mMapping.find(inputIndex);

  if (iter == mMapping.end())
  {
    CEDAR_THROW(cedar::proc::NoMappingException, "There is no mapping defined for the index of the input dimension you supplied.");

    return inputIndex;
  }

  return iter->second;
}

unsigned int cedar::proc::ProjectionMapping::getNumberOfMappings()
{
  return mMapping.size();
}

bool cedar::proc::ProjectionMapping::isMappingBijective(unsigned int inputIndex, unsigned int outputIndex)
{
  for
  (
    std::map<unsigned int, unsigned int>::const_iterator iter = mMapping.begin();
    iter != mMapping.end();
    ++iter
  )
  {
    if (iter->second == outputIndex && !iter->first == inputIndex)
    {
      return false;
    }
  }

  return true;
}

bool cedar::proc::ProjectionMapping::mappingExists(unsigned int inputIndex)
{
  std::map<unsigned int, unsigned int>::const_iterator iter = mMapping.find(inputIndex);

  if (iter == mMapping.end())
  {
    return true;
  }

  return false;
}

bool cedar::proc::ProjectionMapping::isDropped(unsigned int inputIndex)
{
  std::map<unsigned int, unsigned int>::const_iterator iter = mMapping.find(inputIndex);

  if (iter == mMapping.end())
  {
    return false;
  }

  if (iter->second == msDropIndex)
  {
    return true;
  }

  return false;
}

cedar::proc::ProjectionMapping::iterator cedar::proc::ProjectionMapping::begin()
{
  return this->mMapping.begin();
}
cedar::proc::ProjectionMapping::const_iterator cedar::proc::ProjectionMapping::begin() const
{
  return this->mMapping.begin();
}
cedar::proc::ProjectionMapping::iterator cedar::proc::ProjectionMapping::end()
{
  return this->mMapping.end();
}
cedar::proc::ProjectionMapping::const_iterator cedar::proc::ProjectionMapping::end() const
{
  return this->mMapping.end();
}
