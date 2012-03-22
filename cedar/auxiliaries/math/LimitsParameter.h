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
#include "cedar/auxiliaries/Parameter.h"

// SYSTEM INCLUDES


/*!@brief A base class template for parameters describing (numerical) limits.
 */
template <typename T>
class cedar::aux::LimitsParameter : public cedar::aux::Parameter
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
  cedar::aux::Parameter(pOwner, name, defaultValue),
  mLowerLimitMinimum(defaultLowerLimitMinimum),
  mLowerLimitMaximum(defaultLowerLimitMaximum),
  mUpperLimitMinimum(defaultUpperLimitMinimum),
  mUpperLimitMaximum(defaultUpperLimitMaximum)
  {
  }

  //!@brief the constructor
  LimitsParameter
  (
    cedar::aux::Configurable *pOwner,
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
    cedar::aux::ConfigurationNode minimum_child = root.get_child("minimum");
    mLimits.mMinimum = minimum_child.get_value<T>();

    cedar::aux::ConfigurationNode maximum_child = root.get_child("maximum");
    mLimits.mMaximum = maximum_child.get_value<T>();
  }

  /*!@brief Write the current limits into a configuration tree.
   *
   * @param root root of the configuration node the limits information will be written to
   */
  void writeToNode(cedar::aux::ConfigurationNode& root) const
  {
    cedar::aux::ConfigurationNode limits_node;

    cedar::aux::ConfigurationNode minimum_node;
    minimum_node.put_value(mLimits.mMinimum);
    limits_node.push_back(cedar::aux::ConfigurationNode::value_type("minimum", minimum_node));

    cedar::aux::ConfigurationNode maximum_node;
    maximum_node.put_value(mLimits.mMaximum);
    limits_node.push_back(cedar::aux::ConfigurationNode::value_type("maximum", maximum_node));

    root.push_back(cedar::aux::ConfigurationNode::value_type(getName(), limits_node));
  }

  void setMinimum(const T& minimum)
  {
    this->mLimits.mMinimum = minimum;
    this->emitPropertyChangedSignal();
  }

  void setMaximum(const T& maximum)
  {
    // todo: check whether the new maximum is inside the
    this->mLimits.mMaximum = maximum;
    this->emitPropertyChangedSignal();
  }

  const T& getMinimum() const
  {
    return this->mLimits.mMinimum;
  }

  const T& getMaximum() const
  {
    return this->mLimits.mMaximum;
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
  //!@brief The minimum value of the lower limit
  T mLowerLimitMinimum;
  //!@brief The maximum value of the lower limit
  T mLowerLimitMaximum;
  //!@brief The minimum value of the upper limit
  T mUpperLimitMinimum;
  //!@brief The maximum value of the upper limit
  T mUpperLimitMaximum;

  LimitsPtr mLimits;

}; // class cedar::aux::math::LimitsParameter

#endif // CEDAR_AUX_MATH_LIMITS_PARAMETER_H
