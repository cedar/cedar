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

    File:        ObjectListParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.rub.de
    Date:        2012 02 15

    Description: Parameter for a list of arbitrary length of dynamically allocatable, configurable objects. Ha!

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_OBJECT_LIST_PARAMETER_H
#define CEDAR_AUX_OBJECT_LIST_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES


/*!@brief A parameter that reads a list of configurable objects from a file.
 *
 * @todo describe more.
 */
template <class BaseType> //!@todo Should BaseType include the pointer type?
class cedar::aux::ObjectListParameter : public cedar::aux::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  typedef typename boost::shared_ptr<BaseType> BaseTypePtr;
  typedef typename cedar::aux::Singleton<cedar::aux::FactoryManager<BaseTypePtr> > FactorySingleton;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ObjectListParameter(cedar::aux::Configurable *pOwner, const std::string& name)
  :
  cedar::aux::Parameter(pOwner, name, false)
  {
  }

  ObjectListParameter
  (
    cedar::aux::Configurable *pOwner,
    const std::string& name,
    const std::vector<BaseTypePtr>& defaults
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
    this->mObjectList.clear();
    for (cedar::aux::ConfigurationNode::const_iterator iter = node.begin(); iter != node.end(); ++iter)
    {
      const std::string& object_type = iter->first;
      BaseTypePtr object = FactorySingleton::getInstance()->allocate(object_type);

      object->readConfiguration(iter->second);
      this->mObjectList.push_back(object);
    }
  }

  //!@brief write value to a configuration node
  virtual void writeToNode(cedar::aux::ConfigurationNode& root) const
  {
    cedar::aux::ConfigurationNode object_list_node;
    for
    (
      typename std::vector<BaseTypePtr>::const_iterator iter = this->mObjectList.begin();
      iter != this->mObjectList.end();
      ++iter
    )
    {
      BaseTypePtr value = *iter;
      cedar::aux::ConfigurationNode value_node;
      value->writeConfiguration(value_node);

      const std::string& type_id = FactorySingleton::getInstance()->getTypeId(value);

      object_list_node.push_back(cedar::aux::ConfigurationNode::value_type(type_id, value_node));
    }
    root.push_back(cedar::aux::ConfigurationNode::value_type(this->getName(), object_list_node));
  }

  //!@brief set parameter to default
  virtual void makeDefault()
  {
    this->mObjectList = this->mDefaults;
    std::cout << "Set object list default; size: " << this->size() << std::endl;
  }


  //!@brief return the size of the vector
  size_t size() const
  {
    return mObjectList.size();
  }

  /*!@brief Returns the object with the given index.
   *
   * @todo There needs to be a const variant of this.
   */
  BaseTypePtr at(size_t index)
  {
    CEDAR_ASSERT(index < this->size());

    return this->mObjectList.at(index);
  }

  void pushBack(BaseTypePtr object)
  {
    //!@todo Check that the object is registered with the factory
    this->mObjectList.push_back(object);
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
  //! vector of pointers to the various objects
  std::vector<BaseTypePtr> mObjectList;

  //! The default values.
  std::vector<BaseTypePtr> mDefaults;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::ObjectListParameter

#endif // CEDAR_AUX_OBJECT_LIST_PARAMETER_H

