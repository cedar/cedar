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

#ifndef CEDAR_AUX_SET_PARAMETER_H
#define CEDAR_AUX_SET_PARAMETER_H

// LOCAL INCLUDES
#include "auxiliaries/namespace.h"
#include "auxiliaries/ParameterBase.h"
#include "auxiliaries/exceptions.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <set>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
template <typename T>
class cedar::aux::SetParameter : public cedar::aux::ParameterBase
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  SetParameter(const std::string& name)
  :
  cedar::aux::ParameterBase(name, false)
  {
  }

  SetParameter(const std::string& name, const std::set<T>& defaults)
  :
  cedar::aux::ParameterBase(name, true),
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
  void setTo(const cedar::aux::ConfigurationNode& root)
  {
    this->mValues.clear();
    for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin(); iter != root.end(); ++iter)
    {
      this->mValues.insert(iter->second.get_value<T>());
    }
  }

  void putTo(cedar::aux::ConfigurationNode& root)
  {
    cedar::aux::ConfigurationNode vector_node;
    for (typename std::set<T>::iterator iter = this->mValues.begin(); iter != this->mValues.end(); ++iter)
    {
      cedar::aux::ConfigurationNode value_node;
      value_node.put_value(*iter);
      vector_node.push_back(cedar::aux::ConfigurationNode::value_type("", value_node));
    }
    root.push_back(cedar::aux::ConfigurationNode::value_type(this->getName(), vector_node));
  }

  const std::set<T>& get() const
  {
    return this->mValues;
  }

  std::set<T>& get()
  {
    return this->mValues;
  }

  const std::set<T>& getDefaultValues() const
  {
    return this->mDefaults;
  }

  void set(const std::set<T>& values)
  {
    this->mValues = values;
    emit parameterChanged();
  }

  void makeDefault()
  {
    this->mValues = mDefaults;
  }

  void insert(const T& value)
  {
    this->mValues.insert(value);
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
  std::set<T> mValues;
  std::set<T> mDefaults;

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

#endif // CEDAR_AUX_SET_PARAMETER_H

