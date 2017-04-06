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

#ifndef CEDAR_AUX_MAP_PARAMETER_H
#define CEDAR_AUX_MAP_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/exceptions.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MapParameter.fwd.h"

// SYSTEM INCLUDES
#include <map>

/*!@brief A parameter class for vectors of type T.
 *
 * Most of the vector access functions are mirrored by this class. This ensures that a change signal is emitted every
 * time the content of the represented vector changes.
 */
template <typename T>
class cedar::aux::MapParameter : public cedar::aux::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // typedef
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief iterator for internal container type
  typedef typename std::map<std::string, T>::iterator iterator;
  //!@brief const iterator for internal container type
  typedef typename std::map<std::string, T>::const_iterator const_iterator;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  MapParameter(cedar::aux::Configurable *pOwner, const std::string& name)
  :
  cedar::aux::Parameter(pOwner, name, false)
  {
  }

  //!@brief Constructor with default container.
  MapParameter(cedar::aux::Configurable *pOwner, const std::string& name, const std::map<std::string, T>& defaults)
  :
  cedar::aux::Parameter(pOwner, name, true),
  mDefaults(defaults)
  {
    this->makeDefault();
  }

  //!@brief Destructor
  ~MapParameter()
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
      this->mValues[iter->first] = iter->second.get_value<T>();
    }
  }

  //!@brief store the vector values in a configuration tree
  void writeToNode(cedar::aux::ConfigurationNode& root) const
  {
    cedar::aux::ConfigurationNode map_node;
    for (auto iter = this->mValues.begin(); iter != this->mValues.end(); ++iter)
    {
      const std::string& key = iter->first;
      const T& value = iter->second;
      cedar::aux::ConfigurationNode value_node;
      value_node.put_value(value);
      map_node.push_back(cedar::aux::ConfigurationNode::value_type(key, value_node));
    }
    root.push_back(cedar::aux::ConfigurationNode::value_type(this->getName(), map_node));
  }

  //!@brief get the current vector (const)
  const std::map<std::string, T>& getValue() const
  {
    return this->mValues;
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

  //! Finds the given key in the map and returns an iterator to the corresponding element.
  const_iterator find(const std::string& key) const
  {
    return this->mValues.find(key);
  }

  //! Erases the given key and its element from the map.
  void erase(const std::string& key)
  {
    this->mValues.erase(key);
  }

  //! Checks if the map is empty.
  bool empty()
  {
    return this->mValues.empty();
  }

  //!@brief checks if a key is already contained in this map
  bool containsKey(const std::string& key) const
  {
    return this->mValues.find(key) != this->mValues.end();
  }

  //!@brief checks if a value is already contained in this map
  bool containsValue(const T& value) const
  {
    for (const auto& pair : this->mValues)
    {
      if (pair.second == value)
      {
        return true;
      }
    }
    return false;
  }

  //!@brief return the size of the vector
  size_t size() const
  {
    return this->mValues.size();
  }

  //!@brief get the default vector
  const std::map<std::string, T>& getDefaultValues() const
  {
    return this->mDefaults;
  }

  //!@brief set one entry of the map to a new value
  virtual void setValue(const std::string& index, const T& value, bool lock = false)
  {
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

  //! Returns the value with the given index.
  T get(const std::string& index, bool lock = true) const
  {
    T copy;

    if (lock)
    {
      this->lockForRead();
    }

    copy = this->mValues.find(index)->second;

    if (lock)
    {
      this->unlock();
    }

    return copy;
  }

  //!@brief set vector to default
  void makeDefault()
  {
    this->mValues = mDefaults;
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
  std::map<std::string, T> mValues;

  //!@brief a default vector
  std::map<std::string, T> mDefaults;

}; // class cedar::aux::MapParameter

#endif // CEDAR_AUX_MAP_PARAMETER_H
