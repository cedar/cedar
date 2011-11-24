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

    File:        ProjectionMapParameter.h

    Maintainer:  Mathis Richter

    Email:       mathis.richter@ini.rub.de

    Date:        2011 11 21

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_PROJECTION_MAPPING_PARAMETER_H
#define CEDAR_PROC_PROJECTION_MAPPING_PARAMETER_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/exceptions.h"

// PROJECT INCLUDES
#include "cedar/processing/ProjectionMapping.h"

// SYSTEM INCLUDES
#include <climits>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::proc::ProjectionMappingParameter : public cedar::aux::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // typedef
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ProjectionMappingParameter(
                              cedar::aux::Configurable *pOwner,
                              const std::string& name,
                              unsigned int minimumNumberOfMappings = 0,
                              unsigned int maximumNumberOfMappings = UINT_MAX
                            )
  :
  cedar::aux::Parameter(pOwner, name, false),
  mMinimumNumberOfMappings(minimumNumberOfMappings),
  mMaximumNumberOfMappings(maximumNumberOfMappings)
  {
  }

  ProjectionMappingParameter(
                              cedar::aux::Configurable *pOwner,
                              const std::string& name,
                              const cedar::proc::ProjectionMappingPtr& defaults,
                              unsigned int minimumNumberOfMappings = 0,
                              unsigned int maximumNumberOfMappings = UINT_MAX
                            )
  :
  cedar::aux::Parameter(pOwner, name, true),
  mValues(defaults),
  mMinimumNumberOfMappings(minimumNumberOfMappings),
  mMaximumNumberOfMappings(maximumNumberOfMappings)
  {
  }

  //!@brief Destructor
  ~ProjectionMappingParameter()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void setTo(const cedar::aux::ConfigurationNode& root)
  {
    this->mValues->clear();
    for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin(); iter != root.end(); ++iter)
    {
      unsigned int key;
      cedar::aux::fromString<unsigned int>(key, iter->first, std::dec);
      unsigned int value = iter->second.get_value<unsigned int>();

      this->mValues->addMapping(key, value);
    }
  }

  void putTo(cedar::aux::ConfigurationNode& root) const
  {
    cedar::aux::ConfigurationNode map_node;
    for
    (
      std::map<unsigned int, unsigned int>::iterator iter = this->mValues->begin();
      iter != this->mValues->end();
      ++iter
    )
    {
      cedar::aux::ConfigurationNode value_node;
      value_node.put_value(iter->second);
      map_node.push_back(cedar::aux::ConfigurationNode::value_type(cedar::aux::toString<unsigned int>(iter->first), value_node));
    }
    root.push_back(cedar::aux::ConfigurationNode::value_type(this->getName(), map_node));
  }

  const cedar::proc::ProjectionMappingPtr& getValue() const
  {
    return this->mValues;
  }

  void addMapping(unsigned int inputIndex, unsigned int outputIndex)
  {
    if (!this->mValues->mappingExists(inputIndex))
    {
      if (this->mValues->getNumberOfMappings() < this->mMaximumNumberOfMappings)
      {
        this->mValues->addMapping(inputIndex, outputIndex);
        emitPropertyChangedSignal();
      }
    }
  }

  void dropDimension(unsigned int inputIndex)
  {
    if (this->mValues->mappingExists(inputIndex))
    {
      if (this->mValues->getNumberOfMappings() < this->mMaximumNumberOfMappings)
      {
        this->mValues->dropDimension(inputIndex);
        emitPropertyChangedSignal();
      }
    }
  }

  void removeMapping(unsigned int inputIndex)
  {
    if (this->mValues->getNumberOfMappings() > this->mMinimumNumberOfMappings)
    {
      this->mValues->removeMapping(inputIndex);
      emitPropertyChangedSignal();
    }
  }

  void setMinimumNumberOfMappings(unsigned int minimumNumberOfMappings)
  {
    mMinimumNumberOfMappings = minimumNumberOfMappings;
  }

  void setMaximumNumberOfMappings(unsigned int maximumNumberOfMappings)
  {
    mMaximumNumberOfMappings = maximumNumberOfMappings;
  }

  unsigned int getMinimumNumberOfMappings() const
  {
    return mMinimumNumberOfMappings;
  }

  unsigned int getMaximumNumberOfMappings() const
  {
    return mMaximumNumberOfMappings;
  }

  void makeDefault()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  cedar::proc::ProjectionMappingPtr mValues;
  unsigned int mMinimumNumberOfMappings;
  unsigned int mMaximumNumberOfMappings;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::ProjectionMappingParameter

#endif // CEDAR_PROC_PROJECTION_MAPPING_PARAMETER_H

