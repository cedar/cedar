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
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/math/Limits.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/math/LimitsParameter.fwd.h"

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
    const T& defaultLowerLimitMinimum,
    const T& defaultLowerLimitMaximum,
    const T& defaultUpperLimit,
    const T& defaultUpperLimitMinimum,
    const T& defaultUpperLimitMaximum
  )
  :
  cedar::aux::Parameter(pOwner, name),
  mLowerLimitDefault(defaultLowerLimit),
  mLowerLimitMinimum(defaultLowerLimitMinimum),
  mLowerLimitMaximum(defaultLowerLimitMaximum),
  mUpperLimitDefault(defaultUpperLimit),
  mUpperLimitMinimum(defaultUpperLimitMinimum),
  mUpperLimitMaximum(defaultUpperLimitMaximum)
  {
    this->makeDefault();
  }

  //!@brief A constructor that takes limits.
  LimitsParameter
  (
    cedar::aux::Configurable* pOwner,
    const std::string& name,
    const T& defaultLowerLimit,
    const T& defaultUpperLimit,
    const cedar::aux::math::Limits<T>& lowerLimitRange = cedar::aux::math::Limits<T>::full(),
    const cedar::aux::math::Limits<T>& upperLimitRange = cedar::aux::math::Limits<T>::full()
  )
  :
  cedar::aux::Parameter(pOwner, name),
  mLowerLimitDefault(defaultLowerLimit),
  mLowerLimitMinimum(lowerLimitRange.getLower()),
  mLowerLimitMaximum(lowerLimitRange.getUpper()),
  mUpperLimitDefault(defaultUpperLimit),
  mUpperLimitMinimum(upperLimitRange.getLower()),
  mUpperLimitMaximum(upperLimitRange.getUpper())
  {
    this->makeDefault();
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
    mLimits.setLower(lower_limit_child.get_value<T>());

    cedar::aux::ConfigurationNode upper_limit_child = root.get_child("upper limit");
    mLimits.setUpper(upper_limit_child.get_value<T>());
  }

  /*!@brief Write the current limits into a configuration tree.
   *
   * @param root root of the configuration node the limits information will be written to
   */
  void writeToNode(cedar::aux::ConfigurationNode& root) const
  {
    cedar::aux::ConfigurationNode limits_node;

    cedar::aux::ConfigurationNode lower_limit_node;
    lower_limit_node.put_value(mLimits.getLower());
    limits_node.push_back(cedar::aux::ConfigurationNode::value_type("lower limit", lower_limit_node));

    cedar::aux::ConfigurationNode upper_limit_node;
    upper_limit_node.put_value(mLimits.getUpper());
    limits_node.push_back(cedar::aux::ConfigurationNode::value_type("upper limit", upper_limit_node));

    root.push_back(cedar::aux::ConfigurationNode::value_type(this->getName(), limits_node));
  }

  //!@brief set value to default
  void makeDefault()
  {
    this->setLowerLimit(mLowerLimitDefault);
    this->setUpperLimit(mUpperLimitDefault);
  }

  //!@brief Sets the default limits of the parameter.
  void setDefaults(const T& defaultLower, const T& defaultUpper)
  {
    this->mLowerLimitDefault = defaultLower;
    this->mUpperLimitDefault = defaultUpper;
  }

  //!@brief get the current value of type T of this parameter
  const cedar::aux::math::Limits<T>& getValue() const
  {
    return this->mLimits;
  }

  //!@brief set the value of type T of this parameter
  void setValue(const cedar::aux::math::Limits<T>& value, bool lock = false)
  {
    if (lock)
    {
      this->lockForWrite();
    }

    cedar::aux::math::Limits<T> old_value = this->mLimits;
    this->mLimits = value;

    if (lock)
    {
      this->unlock();
    }

    if (old_value != value)
    {
      this->emitChangedSignal();
    }
  }

  //! Sets both limits at the same time.
  void setLimits(const T& lower, const T& upper, bool lock = false)
  {
    this->setValue(cedar::aux::math::Limits<T>(lower, upper), lock);
  }

  //!@brief sets the lower limit and emits a signal
  void setLowerLimit(const T& value, bool lock = false)
  {
    if (lock)
    {
      this->lockForWrite();
    }

    T old_value = this->mLimits.getLower();
    this->mLimits.setLower(value);

    if (lock)
    {
      this->unlock();
    }

    if (value != old_value)
    {
      this->emitChangedSignal();
    }
  }

  //!@brief sets the upper limit and emits a signal
  void setUpperLimit(const T& value, bool lock = false)
  {
    if (lock)
    {
      this->lockForWrite();
    }

    T old_value = this->mLimits.getUpper();
    this->mLimits.setUpper(value);

    if (lock)
    {
      this->unlock();
    }

    if (value != old_value)
    {
      this->emitChangedSignal();
    }
  }

  //!@brief returns the lower limit
  const T& getLowerLimit() const
  {
    return this->mLimits.getLower();
  }

  //!@brief returns the upper limit
  const T& getUpperLimit() const
  {
    return this->mLimits.getUpper();
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
  //!@brief The minimum value of the lower limit
  T mLowerLimitMinimum;
  //!@brief The maximum value of the lower limit
  T mLowerLimitMaximum;

  //!@brief The default value of the upper limit
  T mUpperLimitDefault;
  //!@brief The minimum value of the upper limit
  T mUpperLimitMinimum;
  //!@brief The maximum value of the upper limit
  T mUpperLimitMaximum;

  //! holds the minimum and maximum limits
  cedar::aux::math::Limits<T> mLimits;

}; // class cedar::aux::math::LimitsParameter

#endif // CEDAR_AUX_MATH_LIMITS_PARAMETER_H
