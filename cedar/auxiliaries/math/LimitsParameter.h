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

    File:        LimitsParameter.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 03 21

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_MATH_LIMITS_PARAMETER_H
#define CEDAR_AUX_MATH_LIMITS_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/namespace.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/math/Limits.h"

// SYSTEM INCLUDES


/*!@brief A base class template for parameters describing (numerical) limits.
 */
template <typename T>
class cedar::aux::math::LimitsParameter : public cedar::aux::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  typedef typename boost::shared_ptr<cedar::aux::math::Limits<T> > LimitsPtr;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief the constructor
  LimitsParameter
  (
    cedar::aux::Configurable* pOwner,
    const std::string& name,
    const T& defaultLowerLimit,
    const T& defaultUpperLimit,
    const T& defaultLowerLimitMinimum,
    const T& defaultLowerLimitMaximum,
    const T& defaultUpperLimitMinimum,
    const T& defaultUpperLimitMaximum
  )
  :
  cedar::aux::Parameter(pOwner, name),
  mLowerLimitDefault(defaultLowerLimit),
  mUpperLimitDefault(defaultUpperLimit),
  mLowerLimitMinimum(defaultLowerLimitMinimum),
  mLowerLimitMaximum(defaultLowerLimitMaximum),
  mUpperLimitMinimum(defaultUpperLimitMinimum),
  mUpperLimitMaximum(defaultUpperLimitMaximum)
  {
  }

  //!@brief the constructor
  LimitsParameter
  (
    cedar::aux::Configurable* pOwner,
    const std::string& name,
    const T& defaultLowerLimitMinimum,
    const T& defaultLowerLimitMaximum,
    const T& defaultUpperLimitMinimum,
    const T& defaultUpperLimitMaximum
  )
  :
  cedar::aux::Parameter(pOwner, name),
  mLowerLimitMinimum(defaultLowerLimitMinimum),
  mLowerLimitMaximum(defaultLowerLimitMaximum),
  mUpperLimitMinimum(defaultUpperLimitMinimum),
  mUpperLimitMaximum(defaultUpperLimitMaximum)
  {
  }

  //!@brief Destructor
  ~LimitsParameter()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*!@brief Read limits from a configuration tree.
   *
   * @param root root of the configuration node holding the limits information
   */
  void readFromNode(const cedar::aux::ConfigurationNode& root)
  {
    cedar::aux::ConfigurationNode lower_limit_child = root.get_child("lower limit");
    mLimits.mLowerLimit = lower_limit_child.get_value<T>();

    cedar::aux::ConfigurationNode upper_limit_child = root.get_child("upper limit");
    mLimits.mUpperLimit = upper_limit_child.get_value<T>();
  }

  /*!@brief Write the current limits into a configuration tree.
   *
   * @param root root of the configuration node the limits information will be written to
   */
  void writeToNode(cedar::aux::ConfigurationNode& root) const
  {
    cedar::aux::ConfigurationNode limits_node;

    cedar::aux::ConfigurationNode lower_limit_node;
    lower_limit_node.put_value(mLimits.mLowerLimit);
    limits_node.push_back(cedar::aux::ConfigurationNode::value_type("lower limit", lower_limit_node));

    cedar::aux::ConfigurationNode upper_limit_node;
    upper_limit_node.put_value(mLimits.mUpperLimit);
    limits_node.push_back(cedar::aux::ConfigurationNode::value_type("upper limit", upper_limit_node));

    root.push_back(cedar::aux::ConfigurationNode::value_type(getName(), limits_node));
  }

  //!@brief set value to default
  void makeDefault()
  {
    this->setLowerLimit(mLowerLimitDefault);
    this->setUpperLimit(mUpperLimitDefault);
  }

  void setLowerLimit(const T& value)
  {
    this->mLimits.mLowerLimit = value;
    this->emitPropertyChangedSignal();
  }

  void setUpperLimit(const T& value)
  {
    // todo: check whether the new upper limit is inside the provided extrema
    this->mLimits.mUpperLimit = value;
    this->emitPropertyChangedSignal();
  }

  const T& getLowerLimit() const
  {
    return this->mLimits.mLowerLimit;
  }

  const T& getUpperLimit() const
  {
    return this->mLimits.mUpperLimit;
  }

  //!@brief get the minimum value of the lower limit
  const T& getLowerLimitMinimum() const
  {
    return this->mLowerLimitMinimum;
  }

  //!@brief set the minimum value of the lower limit
  void setLowerLimitMinimum(const T& value)
  {
    this->mLowerLimitMinimum = value;
    this->emitPropertyChangedSignal();
  }

  //!@brief get the maximum value of the lower limit
  const T& getLowerLimitMaximum() const
  {
    return this->mLowerLimitMaximum;
  }

  //!@brief set the maximum value of the lower limit
  void setLowerLimitMaximum(const T& value)
  {
    this->mLowerLimitMaximum = value;

    this->emitPropertyChangedSignal();
  }

  //!@brief get the minimum value of the upper limit
  const T& getUpperLimitMinimum() const
  {
    return this->mUpperLimitMinimum;
  }

  //!@brief set the minimum value of the upper limit
  void setUpperLimitMinimum(const T& value)
  {
    this->mUpperLimitMinimum = value;

    this->emitPropertyChangedSignal();
  }

  //!@brief get the maximum value of the upper limit
  const T& getUpperLimitMaximum() const
  {
    return this->mUpperLimitMaximum;
  }

  //!@brief set the maximum value of the upper limit
  void setUpperLimitMaximum(const T& value)
  {
    this->mUpperLimitMaximum = value;

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
  //!@brief The default value of the lower limit
  T mLowerLimitDefault;
  //!@brief The default value of the upper limit
  T mUpperLimitDefault;
  //!@brief The minimum value of the lower limit
  T mLowerLimitMinimum;
  //!@brief The maximum value of the lower limit
  T mLowerLimitMaximum;
  //!@brief The minimum value of the upper limit
  T mUpperLimitMinimum;
  //!@brief The maximum value of the upper limit
  T mUpperLimitMaximum;

  cedar::aux::math::Limits<T> mLimits;

}; // class cedar::aux::math::LimitsParameter

#endif // CEDAR_AUX_MATH_LIMITS_PARAMETER_H