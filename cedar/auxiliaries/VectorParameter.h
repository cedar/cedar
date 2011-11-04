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
#include "auxiliaries/Parameter.h"
#include "auxiliaries/exceptions.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <vector>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
template <typename T>
class cedar::aux::VectorParameter : public cedar::aux::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // typedef
  //--------------------------------------------------------------------------------------------------------------------
public:
  typedef typename std::vector<T>::iterator iterator;
  typedef typename std::vector<T>::const_iterator const_iterator;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  VectorParameter(cedar::aux::Configurable *pOwner, const std::string& name)
  :
  cedar::aux::Parameter(pOwner, name, false)
  {
  }

  VectorParameter(cedar::aux::Configurable *pOwner, const std::string& name, const std::vector<T>& defaults)
  :
  cedar::aux::Parameter(pOwner, name, true),
  mDefaults(defaults),
  mSize(0)
  {
    this->makeDefault();
  }

  VectorParameter(cedar::aux::Configurable *pOwner, const std::string& name, size_t size, T defaultValue)
  :
  cedar::aux::Parameter(pOwner, name, true),
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
    for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin(); iter != root.end(); ++iter)
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

  const std::vector<T>& getValue() const
  {
    return this->mValues;
  }

  iterator begin()
  {
    return this->mValues.begin();
  }

  iterator end()
  {
    return this->mValues.end();
  }

  iterator erase(iterator iter)
  {
    iterator ret = this->mValues.erase(iter);
    this->emitChangedSignal();
    return ret;
  }

  const T& back() const
  {
    return this->mValues.back();
  }

  /*std::vector<T>& get()
  {
    return this->mValues;
  }*/

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

  bool contains(const T& value)
  {
    for (iterator iter = this->begin(); iter != this->end(); ++iter)
    {
      if ((*iter) == value)
      {
        return true;
      }
    }

    return false;
  }

  void pushBack(const T& value)
  {
    this->mValues.push_back(value);
    this->emitChangedSignal();
  }

  size_t size() const
  {
    return this->mValues.size();
  }

  void resize(size_t size, const T& value = T())
  {
    if (size == this->size())
      return;

    this->mValues.resize(size, value);

    this->emitPropertyChangedSignal();
  }

  const T& at(size_t index) const
  {
    return this->mValues.at(index);
  }

  void set(size_t index, const T& value)
  {
    this->mValues.at(index) = value;
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
    emit valueChanged();
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

