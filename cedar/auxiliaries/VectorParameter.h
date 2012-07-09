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

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <vector>


/*!@brief A parameter class for vectors of type T.
 *
 * Most of the vector access functions are mirrored by this class. This ensures that a change signal is emitted every
 * time the content of the represented vector changes.
 */
template <typename T>
class cedar::aux::VectorParameter : public cedar::aux::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // typedef
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief iterator for internal container type
  typedef typename std::vector<T>::iterator iterator;
  //!@brief const iterator for internal container type
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

  //!@brief Constructor with default container.
  VectorParameter(cedar::aux::Configurable *pOwner, const std::string& name, const std::vector<T>& defaults)
  :
  cedar::aux::Parameter(pOwner, name, true),
  mDefaults(defaults),
  mSize(0)
  {
    this->makeDefault();
  }

  //!@brief Constructor with default size and value.
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
  //!@brief set the vector to values gathered from a configuration tree
  void readFromNode(const cedar::aux::ConfigurationNode& root)
  {
    this->mValues.clear();
    for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin(); iter != root.end(); ++iter)
    {
      this->mValues.push_back(iter->second.get_value<T>());
    }
  }

  //!@brief store the vector values in a configuration tree
  void writeToNode(cedar::aux::ConfigurationNode& root) const
  {
    cedar::aux::ConfigurationNode vector_node;
    for (typename std::vector<T>::const_iterator iter = this->mValues.begin(); iter != this->mValues.end(); ++iter)
    {
      T value = *iter;
      cedar::aux::ConfigurationNode value_node;
      value_node.put_value(value);
      vector_node.push_back(cedar::aux::ConfigurationNode::value_type("", value_node));
    }
    root.push_back(cedar::aux::ConfigurationNode::value_type(this->getName(), vector_node));
  }

  //!@brief get the current vector (const)
  const std::vector<T>& getValue() const
  {
    return this->mValues;
  }

  //!@brief get the last element of the vector
  const T& back() const
  {
    return this->mValues.back();
  }

  //!@brief get the first element of the vector
  const_iterator begin() const
  {
    return this->mValues.begin();
  }

  //!@brief get the sentinel of the vector
  const_iterator end() const
  {
    return this->mValues.end();
  }

  //!@brief erase an entry of this vector
  iterator erase(iterator iter)
  {
    iterator ret = this->mValues.erase(iter);
    this->emitChangedSignal();
    return ret;
  }

  //!@brief Erase first occurrence of T entry
  void eraseFirst(const T& entry)
  {
    iterator it = std::find(this->mValues.begin(), this->mValues.end(), entry);
    if (it != this->mValues.end())
    {
      this->erase(it);
    }
  }

  //!@brief Erase all occurrences of T entry
  void eraseAll(const T& entry)
  {
    iterator it = std::find(this->mValues.begin(), this->mValues.end(), entry);
    while (it != this->mValues.end())
    {
      it = this->erase(it);
      it = std::find(it, this->mValues.end(), entry);
    }
  }

  //!@brief get the default value - throws an exception if no default value is present
  T getDefaultValue() const
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

  //!@brief checks if a value is already contained in this vector
  bool contains(const T& value) const
  {
    for (const_iterator iter = this->begin(); iter != this->end(); ++iter)
    {
      if ((*iter) == value)
      {
        return true;
      }
    }

    return false;
  }

  //!@brief add an element to the back of the vector
  void pushBack(const T& value)
  {
    this->mValues.push_back(value);
    this->emitChangedSignal();
  }

  //!@brief return the size of the vector
  size_t size() const
  {
    return this->mValues.size();
  }

  //!@brief resize the vector to a new size and initialize new entries to the given value
  void resize(size_t size, const T& value)
  {
    if (size == this->size())
    {
      return;
    }

    this->mValues.resize(size, value);

    this->emitPropertyChangedSignal();
  }

  //!@brief Resize the vector to a new size and initialize new entries to the default value set for this parameter.
  void resize(size_t size)
  {
    this->resize(size, this->mDefaultValue);
  }

  //!@brief get an item of this vector specified by an index
  const T& at(size_t index) const
  {
    return this->mValues.at(index);
  }

  //!@brief get the default vector
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

  //!@brief set the internal vector to a given vector
  void set(const std::vector<T>& values, bool lock = false)
  {
    if (lock)
    {
      this->lockForWrite();
    }

    this->mValues.resize(values.size());
    for (size_t i = 0; i < values.size(); ++i)
    {
      this->set(i, values[i]);
    }

    if (lock)
    {
      this->unlock();
    }

    this->emitChangedSignal();
    //!@todo emit a porperty changed signal here as well, as the new vector may have a different size
  }

  //!@brief set one entry of the vector to a new value
  //!@todo This should be called setValue
  virtual void set(size_t index, const T& value)
  {
    CEDAR_DEBUG_ASSERT(index < this->mValues.size());
    T old_value = this->mValues[index];
    this->mValues[index] = value;

    if (value != old_value)
    {
      this->emitChangedSignal();
    }
  }

  //!@brief Set the default value.
  void setDefault(size_t size, const T& value)
  {
    this->setDefaultSize(size);
    this->mDefaultValue = value;
  }

  //!@brief Set the default value.
  void setDefaultSize(size_t size)
  {
    this->mSize = size;
  }

  //!@brief set vector to default
  void makeDefault()
  {
    if (mSize == 0)
    {
      this->mValues = mDefaults;
    }
    else
    {
      mValues.assign(this->mSize, this->mDefaultValue);
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
protected:
  // none yet
private:
  //!@brief internal storage container of vector
  std::vector<T> mValues;
  //!@brief a default vector
  std::vector<T> mDefaults;
  //!@brief a default size
  size_t mSize;
  //!@brief a default value, which is used together with size to create a default vector, or used for new entries
  T mDefaultValue;

}; // class cedar::aux::VectorParameter

#endif // CEDAR_AUX_VECTOR_PARAMETER_H
