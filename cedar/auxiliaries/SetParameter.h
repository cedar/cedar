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

    File:        SetParameter.h

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

#ifndef CEDAR_AUX_SET_PARAMETER_H
#define CEDAR_AUX_SET_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/exceptions.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/SetParameter.fwd.h"

// SYSTEM INCLUDES
#include <set>


/*!@brief A templated std::set parameter.
 *
 * More detailed description of the class.
 */
template <typename T>
class cedar::aux::SetParameter : public cedar::aux::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief a const iterator typedef
  typedef typename std::set<T>::const_iterator const_iterator;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  SetParameter(cedar::aux::Configurable *pOwner, const std::string& name)
  :
  cedar::aux::Parameter(pOwner, name, false)
  {
  }

  //!@brief Constructor with default value.
  SetParameter(cedar::aux::Configurable *pOwner, const std::string& name, const std::set<T>& defaults)
  :
  cedar::aux::Parameter(pOwner, name, true),
  mDefaults(defaults)
  {
    this->makeDefault();
  }

  //!@brief Destructor
  ~SetParameter()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief load a set of type T from a configuration tree
  void readFromNode(const cedar::aux::ConfigurationNode& root)
  {
    this->mValues.clear();
    for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin(); iter != root.end(); ++iter)
    {
      this->mValues.insert(iter->second.get_value<T>());
    }
  }

  //!@brief store a set of type T to a configuration tree
  void writeToNode(cedar::aux::ConfigurationNode& root) const
  {
    cedar::aux::ConfigurationNode vector_node;
    for (typename std::set<T>::const_iterator iter = this->mValues.begin(); iter != this->mValues.end(); ++iter)
    {
      cedar::aux::ConfigurationNode value_node;
      value_node.put_value(*iter);
      vector_node.push_back(cedar::aux::ConfigurationNode::value_type("", value_node));
    }
    root.push_back(cedar::aux::ConfigurationNode::value_type(this->getName(), vector_node));
  }

  //!@brief get the current set of type T (const)
  const std::set<T>& get() const
  {
    return this->mValues;
  }

  //!@brief get the current set of type T
  std::set<T>& get()
  {
    return this->mValues;
  }

  //!@brief get the default set
  const std::set<T>& getDefaultValues() const
  {
    return this->mDefaults;
  }

  //!@brief set the current set of type T
  void set(const std::set<T>& values)
  {
    this->mValues = values;
    emit valueChanged();
  }

  //!@brief set the std::set to default
  void makeDefault()
  {
    this->mValues = mDefaults;
  }

  //!@brief insert a new entry into the set
  void insert(const T& value)
  {
    this->mValues.insert(value);
  }

  //!@brief Erases the entry with the given value.
  void erase(const T& value)
  {
    this->mValues.erase(value);
  }

  //!@brief Checks whether the value is contained in the set.
  bool contains(const T& value)
  {
    return this->mValues.find(value) != this->mValues.end();
  }

  //!@brief Returns the size of the set.
  size_t size() const
  {
    return this->mValues.size();
  }

  //!@brief returns a const iterator to the first element of the set
  const_iterator begin() const
  {
    return this->mValues.begin();
  }

  //!@brief returns a const iterator to the end of the set
  const_iterator end() const
  {
    return this->mValues.end();
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
  //!@brief std::set of entries of type T
  std::set<T> mValues;
  //!@brief default set
  std::set<T> mDefaults;
}; // class cedar::aux::SetParameter

#endif // CEDAR_AUX_SET_PARAMETER_H
