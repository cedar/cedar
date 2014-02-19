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

    File:        DirectParameterLink.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 02 18

    Description: Source file for the class cedar::aux::DirectParameterLink.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/DirectParameterLink.h"
#include "cedar/auxiliaries/NumericParameter.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/utilities.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::DirectParameterLink::DirectParameterLink()
{
}

cedar::aux::DirectParameterLink::~DirectParameterLink()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::DirectParameterLink::leftChanged()
{
  this->assign(this->getLeft(), this->getRight());
}

void cedar::aux::DirectParameterLink::rightChanged()
{
  this->assign(this->getRight(), this->getLeft());
}

bool cedar::aux::DirectParameterLink::checkIfLinkable
     (
       cedar::aux::ConstParameterPtr /* left */,
       cedar::aux::ConstParameterPtr /* right */
     )
     const
{
  // TODO
  return true;
}

template<typename T1, typename T2>
void assign
(
  cedar::aux::ConstParameterPtr source,
  cedar::aux::ParameterPtr target
)
{
  // TODO
}

template <typename T>
double cedar::aux::DirectParameterLink::getNumericValue(cedar::aux::ConstParameterPtr source) const
{
  auto numeric_parameter  = boost::dynamic_pointer_cast<const cedar::aux::NumericParameter<T> >(source);
  CEDAR_ASSERT(numeric_parameter);
  return static_cast<double>(numeric_parameter->getValue());
}

template <typename T>
void cedar::aux::DirectParameterLink::setNumericValue(cedar::aux::ParameterPtr source, double value) const
{
  auto numeric_parameter = boost::dynamic_pointer_cast<cedar::aux::NumericParameter<T> >(source);
  CEDAR_ASSERT(numeric_parameter);
  numeric_parameter->setValue(static_cast<T>(value));
}

double cedar::aux::DirectParameterLink::getValue(cedar::aux::ConstParameterPtr source) const
{
  std::string source_type = this->getNumericType(source);

  if (source_type == "double")
  {
    return getNumericValue<double>(source);
  }
  else if (source_type == "unsigned int")
  {
    return getNumericValue<unsigned int>(source);
  }
  else if (source_type == "int")
  {
    return getNumericValue<int>(source);
  }
  else
  {
    CEDAR_THROW
    (
      cedar::aux::UnhandledTypeException,
      "Cannot get value from " + cedar::aux::objectTypeToString(source) + "."
    );
  }
}

void cedar::aux::DirectParameterLink::setValue(cedar::aux::ParameterPtr target, double value) const
{
  std::string target_type = this->getNumericType(target);
  if (target_type == "double")
  {
    setNumericValue<double>(target, value);
  }
  else if (target_type == "unsigned int")
  {
    if (value < 0.0)
    {
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "Setting a value less than zero at unsigned int parameter \"" + target->getName() + "\". Value restricted to zero.",
        "void setValue(cedar::aux::ConstParameterPtr target, double value)"
      );
      value = 0.0;
    }
    setNumericValue<unsigned int>(target, value);
  }
  else if (target_type == "int")
  {
    setNumericValue<int>(target, value);
  }
  else
  {
    CEDAR_THROW
    (
      cedar::aux::UnhandledTypeException,
      "Cannot get value from " + cedar::aux::objectTypeToString(target) + "."
    );
  }
}

void cedar::aux::DirectParameterLink::assign
     (
       cedar::aux::ConstParameterPtr source,
       cedar::aux::ParameterPtr target
     ) const
{
  // if the target parameter can copy from the source, use this built-in functionality
  if (target->canCopyFrom(source))
  {
    target->copyValueFrom(source);
    return;
  }

  // otherwise, we have to check if we know how to convert these values appropriately

  if (isNumeric(source) && isNumeric(target))
  {
    double value = getValue(source);
    setValue(target, value);
  }
  else
  {
    // finally, if we couldn't handle the parameters until this point, throw an exception
    CEDAR_THROW
    (
      cedar::aux::UnhandledTypeException,
      "Cannot assing values from " + cedar::aux::objectTypeToString(source) + " to "
      + cedar::aux::objectTypeToString(target)
    );
  }
}

bool cedar::aux::DirectParameterLink::isNumeric(cedar::aux::ConstParameterPtr parameter) const
{
  return cedar::aux::startsWith(cedar::aux::objectTypeToString(parameter), "cedar::aux::NumericParameter");
}

std::string cedar::aux::DirectParameterLink::getNumericType(cedar::aux::ConstParameterPtr parameter) const
{
  //!@todo Exception
  CEDAR_ASSERT(isNumeric(parameter));

  std::string rest, last;
  cedar::aux::splitLast(cedar::aux::objectTypeToString(parameter), "<", rest, last);
  CEDAR_ASSERT(last.length() >= 2);
  return last.substr(0, last.length() - 1);
}
