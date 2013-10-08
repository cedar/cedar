/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

namespace cedar
{
  namespace aux
  {
    namespace allocationPolicies
    {
      template <typename ValueType>
      class AllocationPolicy
      {
      protected:
        CEDAR_GENERATE_POINTER_TYPES(ValueType);
        typedef typename cedar::aux::Singleton<cedar::aux::FactoryManager<ValueTypePtr> > FactorySingleton;

      public:
        virtual cedar::aux::ConfigurablePtr configurableAt(const std::string& key) = 0;

        virtual cedar::aux::ConstConfigurablePtr configurableAt(const std::string& key) const = 0;

      protected:
        ValueTypePtr allocate(const cedar::aux::ConfigurationNode& node) const
        {
          const std::string& object_type = node.begin()->first;
          ValueTypePtr object = FactorySingleton::getInstance()->allocate(object_type);

          object->readConfiguration(node.begin()->second);

          return object;
        }
      };

      template <typename ValueType>
      class Instantly : public AllocationPolicy<ValueType>
      {
      public:
        //!@brief iterator for map data structure
        typedef typename std::map<std::string, typename AllocationPolicy<ValueType>::ValueTypePtr>::iterator iterator;

        //!@brief iterator for map data structure (const)
        typedef typename std::map<std::string, typename AllocationPolicy<ValueType>::ValueTypePtr>::const_iterator const_iterator;

        //!@brief reverse iterator for map data structure
        typedef typename std::map<std::string, typename AllocationPolicy<ValueType>::ValueTypePtr>::reverse_iterator reverse_iterator;

        //!@brief reverse iterator for map data structure (const)
        typedef typename std::map<std::string, typename AllocationPolicy<ValueType>::ValueTypePtr>::const_reverse_iterator const_reverse_iterator;

        //!@brief access an object in the map through a given key
        typename AllocationPolicy<ValueType>::ValueTypePtr& operator[](const std::string& key)
        {
          return this->mObjectMap[key];
        }

        /*!
         * @remarks This method must be overridden because ConfigurablePtr and ValueTypePtr are not considered covariant types.
         */
        const typename AllocationPolicy<ValueType>::ConstValueTypePtr operator[](const std::string& key) const
        {
          return this->mObjectMap.find(key)->second;
        }

      protected:
        void readObject(const std::string& key, const cedar::aux::ConfigurationNode& node)
        {
          this->mObjectMap[key] = this->allocate(node);
        }

        void storeObject(const std::string& key, cedar::aux::ConfigurationNode& node) const
        {
          typename AllocationPolicy<ValueType>::ConstValueTypePtr value = (*this)[key];

          cedar::aux::ConfigurationNode value_node;
          value->writeConfiguration(value_node);

          const std::string& type_id = AllocationPolicy<ValueType>::FactorySingleton::getInstance()->getTypeId(value);

          node.push_back(cedar::aux::ConfigurationNode::value_type(type_id, value_node));
        }

        const std::map<std::string, typename AllocationPolicy<ValueType>::ValueTypePtr>& storage() const
        {
          return this->mObjectMap;
        }

        std::map<std::string, typename AllocationPolicy<ValueType>::ValueTypePtr>& storage()
        {
          return this->mObjectMap;
        }

      protected:
        //! vector of pointers to the various objects
        std::map<std::string, typename AllocationPolicy<ValueType>::ValueTypePtr> mObjectMap;
      };



      template <typename ValueType>
      class OnDemand : public AllocationPolicy<ValueType>
      {
      public:
        //!@brief iterator for map data structure
        typedef typename std::map<std::string, cedar::aux::ConfigurationNode>::iterator iterator;

        //!@brief iterator for map data structure (const)
        typedef typename std::map<std::string, cedar::aux::ConfigurationNode>::const_iterator const_iterator;

        //!@brief reverse iterator for map data structure
        typedef typename std::map<std::string, cedar::aux::ConfigurationNode>::reverse_iterator reverse_iterator;

        //!@brief reverse iterator for map data structure (const)
        typedef typename std::map<std::string, cedar::aux::ConfigurationNode>::const_reverse_iterator const_reverse_iterator;


        //!@brief access an object in the map through a given key
        typename AllocationPolicy<ValueType>::ValueTypePtr& operator[](const std::string& key)
        {
          this->ensureKeyExists(key);
          return this->mObjectMap[key];
        }

        /*!
         * @remarks This method must be overridden because ConfigurablePtr and ValueTypePtr are not considered covariant types.
         */
        typename AllocationPolicy<ValueType>::ConstValueTypePtr operator[](const std::string& key) const
        {
          this->ensureKeyExists(key);
          return this->mObjectMap[key];
        }

        /*!@returns True, if the given string is a key in this map and an instance has been created.
         *
         * @throws If the given key is not a known configuration key.
         */
        bool instanceCreated(const std::string& key) const
        {
          CEDAR_ASSERT(this->isConfigurationKey(key));

          return this->mObjectMap.find(key) != this->mObjectMap.end();
        }

      protected:
        void readObject(const std::string& key, const cedar::aux::ConfigurationNode& node)
        {
          mConfigurations[key] = node;
        }

        void storeObject(const std::string& key, cedar::aux::ConfigurationNode& node) const
        {
//          typename AllocationPolicy::ValueTypePtr value = iter->second;
//
//          cedar::aux::ConfigurationNode value_node;
//          value->writeConfiguration(value_node);
//
//          const std::string& type_id = AllocationPolicy<ValueType>::FactorySingleton::getInstance()->getTypeId(value);
//
//          node.push_back(cedar::aux::ConfigurationNode::value_type(type_id, value_node));
        }

        const std::map<std::string, cedar::aux::ConfigurationNode>& storage() const
        {
          return this->mConfigurations;
        }

        std::map<std::string, cedar::aux::ConfigurationNode>& storage()
        {
          return this->mConfigurations;
        }

        bool objectExists(const std::string& key) const
        {
          return this->mObjectMap.find(key) != this->mObjectMap.end() && this->mObjectMap[key];
        }

        void ensureKeyExists(const std::string& key) const
        {
          if (!this->objectExists(key))
          {
            this->allocateKey(key);
          }
        }

        //!@returns True, if the given key exists in the list of known configurations, false otherwise.
        bool isConfigurationKey(const std::string& key) const
        {
          return mConfigurations.find(key) != mConfigurations.end();
        }

        void allocateKey(const std::string& key) const
        {
          //!@todo make this a proper exception
          CEDAR_ASSERT(this->isConfigurationKey(key));

          cedar::aux::ConfigurationNode node = this->mConfigurations.find(key)->second;
          this->mObjectMap[key] = this->allocate(node);
        }

      protected:
        //! vector of pointers to the various objects
        mutable std::map<std::string, typename AllocationPolicy<ValueType>::ValueTypePtr> mObjectMap;

        //! Vector of configuration nodes that can be used to instantiate the various objects.
        std::map<std::string, cedar::aux::ConfigurationNode> mConfigurations;
      };
    }
  }
}

/*!@brief A parameter that reads a map of configurable objects from a file.
 *
 * @todo describe more.
 */
template <class ValueType, class TAllocationPolicy = cedar::aux::allocationPolicies::Instantly<ValueType> >
class cedar::aux::ObjectMapParameterTemplate : public cedar::aux::Parameter, public TAllocationPolicy
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  typedef typename TAllocationPolicy::iterator iterator;
  typedef typename TAllocationPolicy::const_iterator const_iterator;
  typedef typename TAllocationPolicy::reverse_iterator reverse_iterator;
  typedef typename TAllocationPolicy::const_reverse_iterator const_reverse_iterator;

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
    const std::map<std::string, typename TAllocationPolicy::ValueTypePtr>& defaults
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

      TAllocationPolicy::readObject(key, object_node);
    }
  }

  //!@brief write value to a configuration node
  //!@todo Write this part for allocateInstances = false
  virtual void writeToNode(cedar::aux::ConfigurationNode& root) const
  {
    cedar::aux::ConfigurationNode object_map_node;
    for (auto iter = this->begin(); iter != this->end(); ++iter)
    {
      const std::string& key = iter->first;
      cedar::aux::ConfigurationNode object_node;
      this->storeObject(key, object_node);
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
    return this->mObjectMap.size();
  }

  //!@brief return the max size of the map
  size_t max_size() const
  {
    return this->mObjectMap.max_size();
  }

  //!@brief returns whether the map is empty
  bool empty() const
  {
    return this->mObjectMap.empty();
  }

  /*!@brief Returns the given element.
   */
  typename TAllocationPolicy::ValueTypePtr get(const std::string& key)
  {
    return (*this)[key];
  }

  /*!
   * @remarks This method must be overridden because ConfigurablePtr and ValueTypePtr are not considered covariant types.
   */
  cedar::aux::ConfigurablePtr configurableAt(const std::string& key)
  {
    return (*this)[key];
  }

  cedar::aux::ConstConfigurablePtr configurableAt(const std::string& key) const
  {
    return (*this)[key];
  }

  /*!@brief Removes all objects from the list.
   */
  void clear()
  {
    this->mObjectMap.clear();
  }

  //!@brief find an object through a given key
  typename TAllocationPolicy::iterator find(const std::string& key)
  {
    return this->mObjectMap.find(key);
  }

  //!@brief find an object through a given key (const)
  typename TAllocationPolicy::const_iterator find(const std::string& key) const
  {
    return this->mObjectMap.find(key);
  }

  //!@brief returns an iterator to the first element of the map
  typename TAllocationPolicy::iterator begin()
  {
    return this->storage().begin();
  }

  //!@brief returns an iterator to the first element of the map (const)
  typename TAllocationPolicy::const_iterator begin() const
  {
    return this->storage().begin();
  }

  //!@brief returns a reverse iterator to the beginning of the data structure
  typename TAllocationPolicy::reverse_iterator rbegin()
  {
    return this->storage().rbegin();
  }

  //!@brief returns a reverse iterator to the beginning of the data structure (const)
  typename TAllocationPolicy::const_reverse_iterator rbegin() const
  {
    return this->storage().rbegin();
  }

  //!@brief returns an iterator to the end of the map
  typename TAllocationPolicy::iterator end()
  {
    return this->storage().end();
  }

  //!@brief returns an iterator to the end of the map (const)
  typename TAllocationPolicy::const_iterator end() const
  {
    return this->storage().end();
  }

  //!@brief returns a reverse iterator to the end
  typename TAllocationPolicy::reverse_iterator rend()
  {
    return this->storage().rend();
  }

  //!@brief returns a reverse iterator (const) to the end
  typename TAllocationPolicy::const_reverse_iterator rend() const
  {
    return this->mObjectMap.rend();
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
    typename std::map<std::string, typename TAllocationPolicy::ValueTypePtr>::iterator map_it;
    for (map_it = this->mObjectMap.begin(); map_it != this->mObjectMap.end(); ++map_it)
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
  //! The default values.
  std::map<std::string, typename TAllocationPolicy::ValueTypePtr> mDefaults;

}; // class cedar::aux::ObjectMapParameterTemplate

#endif // CEDAR_AUX_OBJECT_MAP_PARAMETER_TEMPLATE_H
