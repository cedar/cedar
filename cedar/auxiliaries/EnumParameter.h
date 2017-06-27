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
#include "cedar/auxiliaries/ParameterTemplate.h"
#include "cedar/auxiliaries/EnumBase.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/EnumParameter.fwd.h"

// SYSTEM INCLUDES
#include <set>
#include <string>


namespace cedar
{
  namespace aux
  {
    /*!@brief A namespace for some implementation details of cedar::aux::ParameterTemplate.
     */
    namespace EnumParameterDetails
    {
      /*!@brief Standard policy for reading and writing parameters.
       */
      class ValuePolicy
      {
      public:
        //! Type that is read from the configuration.
        typedef std::string ReadType;

        //! Constructor.
        ValuePolicy();

        //! Constructor.
        ValuePolicy(const cedar::aux::Enum& value);


      protected:
        //! Sets the enum type to be used.
        void setEnum(cedar::aux::EnumBasePtr enumDeclaration);

        //! Returns the value of the parameter as a string.
        std::string getValuePrivate() const;

        //! Sets the value in the parameter using a string.
        void setValuePrivate(const ReadType& value);

      protected:
        //! A pointer to the enum used by this parameter
        cedar::aux::EnumBasePtr mEnumDeclaration;

        //! Stored value.
        cedar::aux::Enum mValue;
      };
    }
  }
}

/*!@brief A parameter storing an enum value.
 *
 *        With this parameter, configurables can add an enum class as a parameter. This means that the user can select
 *        a value from a fixed set.
 */
class cedar::aux::EnumParameter : public cedar::aux::ParameterTemplate
                                         <
                                           cedar::aux::Enum,
                                           cedar::aux::EnumParameterDetails::ValuePolicy
                                         >
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef cedar::aux::ParameterTemplate
      <
        cedar::aux::Enum,
        cedar::aux::EnumParameterDetails::ValuePolicy
      >
      Super;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor with a default value.
  EnumParameter
  (
    cedar::aux::Configurable *pOwner,
    const std::string& name,
    cedar::aux::EnumBasePtr enumBase,
    cedar::aux::EnumId defaultValue
  );

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief set enum value to a specified id
  void setValue(cedar::aux::EnumId enumId, bool lock = false);

  //!@brief set enum value to a specified id
  void setValue(const std::string& enumId, bool lock = false);

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
  //!@brief Set of all the disabled enum values.
  std::set<cedar::aux::EnumId> mDisabledValues;

}; // class cedar::aux::EnumParameter

#endif // CEDAR_AUX_ENUM_PARAMETER_H
