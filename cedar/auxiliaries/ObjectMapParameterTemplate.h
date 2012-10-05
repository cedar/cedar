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

    File:        ObjectMapParameter.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 04 04

    Description: Parameter for a map of dynamically allocatable, configurable objects.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_OBJECT_MAP_PARAMETER_TEMPLATE_H
#define CEDAR_AUX_OBJECT_MAP_PARAMETER_TEMPLATE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES


/*!@brief A parameter that reads a map of configurable objects from a file.
 *
 * @todo describe more.
 */
template <class ValueType>
class cedar::aux::ObjectMapParameterTemplate : public cedar::aux::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  CEDAR_GENERATE_POINTER_TYPES(ValueType);
  typedef typename cedar::aux::Singleton<cedar::aux::FactoryManager<ValueTypePtr> > FactorySingleton;

public:
  //!@brief iterator for map data structure
  typedef typename std::map<std::string, ValueTypePtr>::iterator iterator;

  //!@brief iterator for map data structure (const)
  typedef typename std::map<std::string, ValueTypePtr>::const_iterator const_iterator;

  //!@brief reverse iterator for map data structure
  typedef typename std::map<std::string, ValueTypePtr>::reverse_iterator reverse_iterator;

  //!@brief reverse iterator for map data structure (const)
  typedef typename std::map<std::string, ValueTypePtr>::const_reverse_iterator const_reverse_iterator;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ObjectMapParameterTemplate(cedar::aux::Configurable *pOwner, const std::string& name)
  :
  cedar::aux::Parameter(pOwner, name, false)
  {
  }

  /*!@brief Constructor.
   *
   * @param pOwner   The configurable that owns this parameter.
   * @param name     The name of this parameter.
   * @param defaults The map used as default value.
   */
  ObjectMapParameterTemplate
  (
    cedar::aux::Configurable *pOwner,
    const std::string& name,
    const std::map<std::string, ValueTypePtr>& defaults
  )
  :
  cedar::aux::Parameter(pOwner, name, true)
  {
    this->mDefaults = defaults;
    this->makeDefault();
  }

  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief set this parameter to a value, read from a configuration node
  virtual void readFromNode(const cedar::aux::ConfigurationNode& node)
  {
    this->mObjectMap.clear();
    for (cedar::aux::ConfigurationNode::const_iterator iter = node.begin(); iter != node.end(); ++iter)
    {
      const std::string& key = iter->first;
      cedar::aux::ConfigurationNode object_node = iter->second;

      CEDAR_ASSERT(object_node.size() == 1);

      const std::string& object_type = object_node.begin()->first;
      ValueTypePtr object = FactorySingleton::getInstance()->allocate(object_type);

      object->readConfiguration(object_node.begin()->second);
      this->mObjectMap[key] = object;
    }
  }

  //!@brief write value to a configuration node
  virtual void writeToNode(cedar::aux::ConfigurationNode& root) const
  {
    cedar::aux::ConfigurationNode object_map_node;
    for (const_iterator iter = this->begin(); iter != this->end(); ++iter)
    {
      const std::string& key = iter->first;
      ValueTypePtr value = iter->second;
      cedar::aux::ConfigurationNode object_node;

      cedar::aux::ConfigurationNode value_node;
      value->writeConfiguration(value_node);

      const std::string& type_id = FactorySingleton::getInstance()->getTypeId(value);

      object_node.push_back(cedar::aux::ConfigurationNode::value_type(type_id, value_node));
      object_map_node.push_back(cedar::aux::ConfigurationNode::value_type(key, object_node));
    }

    root.push_back(cedar::aux::ConfigurationNode::value_type(this->getName(), object_map_node));
  }

  //!@brief set parameter to default
  virtual void makeDefault()
  {
    this->mObjectMap = this->mDefaults;
  }

  //!@brief return the size of the map
  size_t size() const
  {
    return mObjectMap.size();
  }

  //!@brief return the max size of the map
  size_t max_size() const
  {
    return mObjectMap.max_size();
  }

  //!@brief returns whether the map is empty
  bool empty() const
  {
    return mObjectMap.empty();
  }

  //!@brief access an object in the map through a given key
  ValueTypePtr& operator[](const std::string& key)
  {
    return mObjectMap[key];
  }

  /*!
   * @remarks This method must be overridden because ConfigurablePtr and ValueTypePtr are not considered covariant types.
   */
  const ConstValueTypePtr& operator[](const std::string& key) const
  {
    return mObjectMap[key];
  }

  /*!
   * @remarks This method must be overridden because ConfigurablePtr and ValueTypePtr are not considered covariant types.
   */
  cedar::aux::ConfigurablePtr configurableAt(const std::string& key)
  {
    return mObjectMap[key];
  }

  /*!@brief Removes all objects from the list.
   */
  void clear()
  {
    this->mObjectMap.clear();
  }

  //!@brief find an object through a given key
  iterator find(const std::string& key)
  {
    return mObjectMap.find(key);
  }

  //!@brief find an object through a given key (const)
  const_iterator find(const std::string& key) const
  {
    return mObjectMap.find(key);
  }

  //!@brief returns an iterator to the first element of the map
  iterator begin()
  {
    return mObjectMap.begin();
  }

  //!@brief returns an iterator to the first element of the map (const)
  const_iterator begin() const
  {
    return mObjectMap.begin();
  }

  //!@brief returns a reverse iterator to the beginning of the data structure
  reverse_iterator rbegin()
  {
    return mObjectMap.rbegin();
  }

  //!@brief returns a reverse iterator to the beginning of the data structure (const)
  const_reverse_iterator rbegin() const
  {
    return mObjectMap.rbegin();
  }

  //!@brief returns an iterator to the end of the map
  iterator end()
  {
    return mObjectMap.end();
  }

  //!@brief returns an iterator to the end of the map (const)
  const_iterator end() const
  {
    return mObjectMap.end();
  }

  //!@brief returns a reverse iterator to the end
  reverse_iterator rend()
  {
    return mObjectMap.rend();
  }

  //!@brief returns a reverse iterator (const) to the end
  const_reverse_iterator rend() const
  {
    return mObjectMap.rend();
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
  /*!@brief Sets the changed flag of the parameter and all the objects stored in it.
   */
  void setChangedFlag(bool changed)
  {
    // set the changed flag for the objects managed by the parameter
    iterator map_it;
    for (map_it = this->begin(); map_it != this->end(); ++map_it)
    {
      map_it->second->resetChangedStates(changed);
    }

    // set the changed flag for the parameter itself.
    this->cedar::aux::Parameter::setChangedFlag(changed);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! vector of pointers to the various objects
  std::map<std::string, ValueTypePtr> mObjectMap;

  //! The default values.
  std::map<std::string, ValueTypePtr> mDefaults;

}; // class cedar::aux::ObjectMapParameterTemplate

#endif // CEDAR_AUX_OBJECT_MAP_PARAMETER_TEMPLATE_H
