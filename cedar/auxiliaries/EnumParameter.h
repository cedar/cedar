/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        EnumParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 28

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_ENUM_PARAMETER_H
#define CEDAR_AUX_ENUM_PARAMETER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/EnumBase.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/EnumParameter.fwd.h"

// SYSTEM INCLUDES
#include <set>

/*!@brief A parameter storing an enum value.
 *
 * More detailed description of the class coming soon.
 */
class cedar::aux::EnumParameter : public cedar::aux::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  EnumParameter(cedar::aux::Configurable *pOwner,
                const std::string& name,
                boost::shared_ptr<cedar::aux::EnumBase> enumBase);

  //!@brief The standard constructor, with an additional default value.
  EnumParameter(cedar::aux::Configurable *pOwner,
                const std::string& name,
                boost::shared_ptr<cedar::aux::EnumBase> enumBase,
                cedar::aux::EnumId defaultValue);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief read from a configuration node
  void readFromNode(const cedar::aux::ConfigurationNode& root);

  //!@brief write to a configuration node
  void writeToNode(cedar::aux::ConfigurationNode& root) const;

  //!@brief set enum value to default
  void makeDefault();

  //!@brief return the enum value
  cedar::aux::Enum getValue() const;

  //!@brief set enum value to a specified id
  void setValue(cedar::aux::EnumId enumId, bool lock = false);

  //!@brief set enum value to a specified id
  void setValue(const std::string& enumId, bool lock = false);

  //!@brief set enum value to a specified id
  CEDAR_DECLARE_DEPRECATED(void set(const std::string& enumId))
  {
    this->setValue(enumId);
  }

  //!@brief get the enum from which this parameter represents an entry
  const cedar::aux::EnumBase& getEnumDeclaration()
  {
    return *(this->mEnumDeclaration);
  }

  //!@brief Disables the given option.
  void disable(cedar::aux::EnumId value);

  //!@brief Enables the given option.
  void enable(cedar::aux::EnumId value);

  //! Enables or disables the given option
  void setEnabled(cedar::aux::EnumId value, bool enabled);

  //! Enables all values.
  void enableAll();

  //! Disables all values.
  void disableAll();

  //! Tests if the given value is enabled.
  bool isEnabled(cedar::aux::EnumId value) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void selectFirstEnabled();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief the enum value of this parameter
  cedar::aux::EnumId mValue;

  //!@brief the default value of this parameter
  cedar::aux::EnumId mDefault;

  //!@brief a pointer to the enum used by this parameter
  boost::shared_ptr<cedar::aux::EnumBase> mEnumDeclaration;

  //!@brief Set of all the disabled enum values.
  std::set<cedar::aux::EnumId> mDisabledValues;
}; // class cedar::aux::VectorParameter

#endif // CEDAR_AUX_ENUM_PARAMETER_H
