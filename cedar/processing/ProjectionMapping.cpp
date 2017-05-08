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

    File:        ProjectionMapping.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2011 11 18

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/ExceptionBase.h"
#include "cedar/processing/ProjectionMapping.h"
#include "cedar/processing/exceptions.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <limits>
#include <vector>

// this number is used to represent a dimension that is to be dropped
unsigned int cedar::proc::ProjectionMapping::msDropIndex = std::numeric_limits<unsigned int>::max();

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::ProjectionMapping::ProjectionMapping()
:
mOutputDimensionality(0)
{
}

cedar::proc::ProjectionMapping::~ProjectionMapping()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::ProjectionMapping::changeMapping(unsigned int inputIndex, unsigned int outputIndex)
{
  if (checkInputIndex(inputIndex) && checkOutputIndex(outputIndex))
  {
    mMapping[inputIndex] = outputIndex;
    updateValidity();
  }
}

void cedar::proc::ProjectionMapping::addMapping(unsigned int inputIndex, unsigned int outputIndex)
{
  mMapping[inputIndex] = outputIndex;
}

void cedar::proc::ProjectionMapping::updateValidity()
{
  mValidity = cedar::proc::ProjectionMapping::VALIDITY_VALID;

  // count the number of dimension drops
  unsigned int number_of_drops = 0;

  // check whether different input dimensions are mapped onto the same output dimension
  std::vector<unsigned int> output_dimension_histogram(mOutputDimensionality, 0);

  for
  (
    cedar::proc::ProjectionMapping::iterator iter = mMapping.begin();
    iter != mMapping.end();
    ++iter
  )
  {
    if (isDropped(iter->first))
    {
      ++number_of_drops;
    }
    else
    {
      if
      (
        mOutputDimensionality > 0
        &&
        (
          iter->second >= output_dimension_histogram.size()
          ||
          ++(output_dimension_histogram[iter->second]) > 1
        )
      )
      {
        mValidity = cedar::proc::ProjectionMapping::VALIDITY_ERROR;
      }
    }
  }

  // if the input dimensionality is smaller or equal to the output dimensionality ...
  if (getNumberOfMappings() <= mOutputDimensionality)
  {
    // ... none of the dimensions should be dropped.
    if (number_of_drops > 0)
    {
      mValidity = cedar::proc::ProjectionMapping::VALIDITY_ERROR;
    }
  }
  // if the input dimensionality is larger than the output dimensionality ...
  else
  {
    // ... the mapping should drop the correct number of excess dimensions.
    if (number_of_drops != (getNumberOfMappings() - mOutputDimensionality))
    {
      mValidity = cedar::proc::ProjectionMapping::VALIDITY_ERROR;
    }
  }
}

void cedar::proc::ProjectionMapping::drop(unsigned int inputIndex)
{
  if (checkInputIndex(inputIndex))
  {
    mMapping[inputIndex] = msDropIndex;
    updateValidity();
  }
}

void cedar::proc::ProjectionMapping::initialize(unsigned int numberOfMappings)
{
  // store the old mapping
  mMappingsMap[mMapping.size()] = mMapping;
  // try to find a mapping matching the number of mappings
  auto map = mMappingsMap.find(numberOfMappings);
  if (map != mMappingsMap.end())
  {
    // we previously used this number of mappings, restore
    mMapping = map->second;
  }
  else
  {
    // could not find a mapping, initialize
    mMapping.clear();
    for (unsigned int i = 0; i < numberOfMappings; ++i)
    {
      mMapping[i] = msDropIndex;
    }
    // also store the new configuration!
    mMappingsMap[numberOfMappings] = mMapping;
  }
  updateValidity();
}

unsigned int cedar::proc::ProjectionMapping::lookUp(unsigned int inputIndex) const
{
  std::map<unsigned int, unsigned int>::const_iterator iter = mMapping.find(inputIndex);

  // if no mapping exists for the input index or the input dimension is dropped
  if (iter == mMapping.end() || iter->second == msDropIndex)
  {
    CEDAR_THROW(cedar::proc::NoMappingException, "There is no mapping defined for the index of the input dimension you supplied.");
  }

  return iter->second;
}

unsigned int cedar::proc::ProjectionMapping::getNumberOfMappings() const
{
  return mMapping.size();
}

bool cedar::proc::ProjectionMapping::mappingExists(unsigned int inputIndex) const
{
  std::map<unsigned int, unsigned int>::const_iterator iter = mMapping.find(inputIndex);

  if (iter == mMapping.end())
  {
    return false;
  }

  return true;
}

bool cedar::proc::ProjectionMapping::isDropped(unsigned int inputIndex) const
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

void cedar::proc::ProjectionMapping::setOutputDimensionality(unsigned int dimensionality)
{
  mOutputDimensionality = dimensionality;
  // prune invalid entries
  for (unsigned int i = 0; i < mMapping.size(); ++i)
  {
    if (mMapping[i] >= dimensionality)
    {
      mMapping[i] = msDropIndex;
    }
  }
  updateValidity();
}

unsigned int cedar::proc::ProjectionMapping::getOutputDimensionality() const
{
  return mOutputDimensionality;
}

bool cedar::proc::ProjectionMapping::checkInputIndex(unsigned int inputIndex) const
{
  bool input_index_okay = true;

  if (inputIndex >= getNumberOfMappings())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException, "The index you supplied for the input dimension is too large,\
                                                       given the input dimensionality of the projection.");
    input_index_okay = false;
  }

  return input_index_okay;
}

bool cedar::proc::ProjectionMapping::checkOutputIndex(unsigned int outputIndex) const
{
  bool output_index_okay = true;

  if (outputIndex >= mOutputDimensionality)
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException, "The index you supplied for the output dimension is too large,\
                                                       given the output dimensionality of the projection.");
    output_index_okay = false;
  }

  return output_index_okay;
}

cedar::proc::ProjectionMapping::VALIDITY cedar::proc::ProjectionMapping::getValidity() const
{
  return mValidity;
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
