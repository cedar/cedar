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

    File:        VectorParameter.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 20

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_VECTOR_PARAMETER_H
#define CEDAR_AUX_VECTOR_PARAMETER_H

// LOCAL INCLUDES
#include "auxiliaries/namespace.h"
#include "auxiliaries/ParameterBase.h"
#include "auxiliaries/exceptions.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <vector>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
template <typename T>
class cedar::aux::VectorParameter : public cedar::aux::ParameterBase
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  VectorParameter(const std::string& name)
  :
  cedar::aux::ParameterBase(name, false)
  {
  }

  VectorParameter(const std::string& name, const std::vector<T>& defaults)
  :
  cedar::aux::ParameterBase(name, true),
  mDefaults(defaults),
  mSize(0)
  {
    this->makeDefault();
  }

  VectorParameter(const std::string& name, size_t size, T defaultValue)
  :
  cedar::aux::ParameterBase(name, true),
  mSize(size),
  mDefaultValue(defaultValue)
  {
    this->makeDefault();
  }

  //!@brief Destructor
  ~VectorParameter()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void setTo(const cedar::aux::ConfigurationNode& root)
  {
    this->mValues.clear();
    cedar::aux::ConfigurationNode node = root.get_child(this->getName());
    for (cedar::aux::ConfigurationNode::iterator iter = node.begin(); iter != node.end(); ++iter)
    {
      this->mValues.push_back(iter->second.get_value<T>());
    }
  }

  void putTo(cedar::aux::ConfigurationNode& root)
  {
    cedar::aux::ConfigurationNode vector_node;
    for (typename std::vector<T>::iterator iter = this->mValues.begin(); iter != this->mValues.end(); ++iter)
    {
      T& value = *iter;
      cedar::aux::ConfigurationNode value_node;
      value_node.put_value(value);
      vector_node.push_back(cedar::aux::ConfigurationNode::value_type("", value_node));
    }
    root.push_back(cedar::aux::ConfigurationNode::value_type(this->getName(), vector_node));
  }

  const std::vector<T>& get() const
  {
    return this->mValues;
  }

  std::vector<T>& get()
  {
    return this->mValues;
  }

  T getDefaultValue()
  {
    if (this->mSize == 0) // there is only a default vector, see if it is of size > 0
    {
      if (mDefaults.size() > 0)
      {
        return mDefaults.at(0);
      }
      else
      {
        CEDAR_THROW(cedar::aux::NoDefaultException, "the parameter " + this->getName() + " has no default value");
      }
    }
    else // return single default value
    {
      return this->mDefaultValue;
    }
  }

  const std::vector<T>& getDefaultValues() const
  {
    if (this->mSize != 0) // there is only a default dimensionality and one value, construct vector
    {
      mDefaults.clear();
      for (size_t i = 0; i < this->mSize; ++i)
      {
        mDefaults.push_back(this->mDefaultValue);
      }
    }
    return this->mDefaults;
  }

  void set(const std::vector<T>& values)
  {
    this->mValues = values;
    emit parameterChanged();
  }

  void makeDefault()
  {
    if (mSize == 0)
    {

      this->mValues = mDefaults;
    }
    else
    {
      mValues.clear();
      for (size_t i = 0; i < mSize; i++)
      {
        mValues.push_back(mDefaultValue);
      }
    }
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
  std::vector<T> mValues;
  std::vector<T> mDefaults;
  size_t mSize;
  T mDefaultValue;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::VectorParameter

#endif // CEDAR_AUX_VECTOR_PARAMETER_H

