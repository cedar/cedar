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
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/exceptions.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/VectorParameter.fwd.h"

// SYSTEM INCLUDES
#include <vector>
#include <algorithm>
#include <string>

namespace cedar
{
  namespace aux
  {
    /*!@brief Class that abstracts methods for accessing vector storage.
     *
     *        Template specializations can override the way that elements in std::vectors are accessed. This is
     *        necessary because std::vector<bool> behaves differently than vectors in general (at returns a copy instead
     *        of a reference).
     */
    template <typename T>
    class StorageAbstraction
    {
      public:
        /*!@brief Returns the element at the given index.
         */
        const T& at(size_t index) const
        {
          CEDAR_DEBUG_ASSERT(index < this->mValues.size());
          return this->mValues[index];
        }

      protected:
        //!@brief Internal storage container.
        std::vector<T> mValues;
    };

    /*!@brief Specialization of the storage abstraction for a vector of bools.
     */
    template<>
    class StorageAbstraction<bool>
    {
      public:
        /*!@brief Returns the element at the given index.
         */
        bool at(size_t index) const
        {
          CEDAR_DEBUG_ASSERT(index < this->mValues.size());
          return this->mValues[index];
        }

      protected:
        //!@brief Internal storage container.
        std::vector<bool> mValues;
    };
  }
}


/*!@brief A parameter class for vectors of type T.
 *
 * Most of the vector access functions are mirrored by this class. This ensures that a change signal is emitted every
 * time the content of the represented vector changes.
 */
template <typename T>
class cedar::aux::VectorParameter : public cedar::aux::Parameter,
                                    public cedar::aux::StorageAbstraction<T>
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef cedar::aux::VectorParameter<T> SelfType;
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(SelfType);

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
  bool canCopyFrom(cedar::aux::ConstParameterPtr other) const
  {
    return static_cast<bool>(boost::dynamic_pointer_cast<ConstSelfType>(other));
  }

  void copyValueFrom(cedar::aux::ConstParameterPtr other)
  {
    if (auto other_self = boost::dynamic_pointer_cast<ConstSelfType>(other))
    {
      this->setValue(other_self->getValue());
    }
    else
    {
      CEDAR_THROW
      (
        cedar::aux::UnhandledTypeException,
        "Cannot copy vector parameter value: types don't match. Type of this: " + cedar::aux::objectTypeToString(this)
        + ", type of other: " + cedar::aux::objectTypeToString(other)
      );
    }
  }

  //!@brief set the vector to values gathered from a configuration tree
  void readFromNode(const cedar::aux::ConfigurationNode& root)
  {
    this->mValues.clear();
    this->mSingleValueConst.clear();
    for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin(); iter != root.end(); ++iter)
    {
      try
      {
        this->mValues.push_back(iter->second.get_value<T>());
        this->mSingleValueConst.push_back(false);
      }
      catch (const boost::property_tree::ptree_bad_data& e)
      {
        std::string info(e.what());
        CEDAR_THROW
        (
          cedar::aux::InvalidValueException,
          "Failed to read vector entry from string \"" + iter->second.get_value<std::string>() + "\". Boost says: ptree_bad_data. " + info
        );
      }
    }
  }

  //!@brief store the vector values in a configuration tree
  void writeToNode(cedar::aux::ConfigurationNode& root) const
  {
    cedar::aux::ConfigurationNode vector_node;
    for (T value : this->mValues)
    {
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
    size_t index = std::distance(this->mValues.begin(), iter);
    CEDAR_ASSERT(index < this->mSingleValueConst.size())
    this->mSingleValueConst.erase(this->mSingleValueConst.begin() + index);
    iterator ret = this->mValues.erase(iter);
    this->emitChangedSignal();
    return ret;
  }

  //! Erases the entry at the given index.
  void eraseIndex(size_t index)
  {
    CEDAR_ASSERT(index < this->mValues.size());

    this->mValues.erase(this->mValues.begin() + index);
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
    this->mSingleValueConst.push_back(false);
    this->emitChangedSignal();
  }

  //!@brief return the size of the vector
  size_t size() const
  {
    return this->mValues.size();
  }

  //! Swaps the given elements.
  void swap(size_t first, size_t second)
  {
    CEDAR_ASSERT(first < this->mValues.size());
    CEDAR_ASSERT(second < this->mValues.size());

    std::swap(this->mValues[first], this->mValues[second]);

    this->emitChangedSignal();
  }

  /*!@brief resize the vector to a new size and initialize new entries to the given value
   * @param size the new size of the vector
   * @param value the default value used to initialize new entries
   * @param blockSignal suppress the emission of a propertyChanged signal
   */
  void resize(size_t size, const T& value, bool blockSignal = false)
  {
    this->mSingleValueConst.resize(size, false);
    if (size == this->size())
    {
      return;
    }

    // make sure the defaults vector is at least as big as the current vector
    this->mDefaults.resize(std::max(this->mDefaults.size(), this->mValues.size()));
    for (size_t i = 0; i < this->mValues.size(); ++i)
    {
      this->mDefaults.at(i) = this->mValues.at(i);
    }

    this->mValues.resize(size, value);

    // restore previous value
    for (size_t i = 0; i < this->mValues.size() && i < this->mDefaults.size(); ++i)
    {
      this->mValues.at(i) = this->mDefaults.at(i);
    }

    if (!blockSignal)
    {
      this->emitPropertyChangedSignal();
    }
  }

  /*!@brief Resize the vector to a new size and initialize new entries to the default value set for this parameter.
   * @param size the new size of the vector
   */
  void resize(size_t size)
  {
    this->resize(size, this->mDefaultValue);
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
  void setValue(const std::vector<T>& values, bool lock = false)
  {
    cedar::aux::Parameter::WriteLockerPtr locker;
    if (lock)
    {
      locker = cedar::aux::Parameter::WriteLockerPtr(new cedar::aux::Parameter::WriteLocker(this));
    }

    bool changed = (this->mValues.size() != values.size());

    this->resize(values.size(), this->mDefaultValue, true);
    for (size_t i = 0; i < values.size(); ++i)
    {
      if (this->mValues[i] != values[i])
      {
        this->mValues[i] = values[i];
        changed = true;
      }
    }

    if (lock)
    {
      locker->unlock();
    }

    if (changed)
    {
      this->emitChangedSignal();
      this->emitPropertyChangedSignal();
    }
  }

  //!@brief set one entry of the vector to a new value
  virtual void setValue(size_t index, const T& value, bool lock = false)
  {
    CEDAR_DEBUG_ASSERT(index < this->mValues.size());

    if (lock)
    {
      this->lockForWrite();
    }

    T old_value = this->mValues[index];
    this->mValues[index] = value;

    if (lock)
    {
      this->unlock();
    }

    if (this->mValues[index] != old_value)
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
      this->mValues.assign(this->mSize, this->mDefaultValue);
    }
    this->mSingleValueConst.assign(this->mValues.size(), false);
  }

  //!@brief This function returns the constness of a single entry.
  bool isConstantAt(size_t index) const
  {
    CEDAR_ASSERT(index < mSingleValueConst.size());
    if (this->isConstant())
    {
      return true;
    }
    return mSingleValueConst[index];
  }

  //!@brief This function sets the constness of  a single entry.
  void setConstantAt(size_t index, bool constness)
  {
    CEDAR_ASSERT(index < mSingleValueConst.size());
    mSingleValueConst[index] = constness;
    this->emitPropertyChangedSignal();
  }

  //!@brief This function resets the constness of all entries - and it sounds funny!
  void unconstAll()
  {
    mSingleValueConst.assign(this->mSingleValueConst.size(), false);
    this->emitPropertyChangedSignal();
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
  //!@brief a default vector
  std::vector<T> mDefaults;

  //!@brief a vector that tags single values as const (in contrast to setting the whole vector to const)
  std::vector<bool> mSingleValueConst;

  //!@brief a default size
  size_t mSize;

  //!@brief a default value, which is used together with size to create a default vector, or used for new entries
  T mDefaultValue;

}; // class cedar::aux::VectorParameter

#endif // CEDAR_AUX_VECTOR_PARAMETER_H
