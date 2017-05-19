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

    File:        VectorParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 02 27

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_NUMERIC_PARAMETER_HELPER_H
#define CEDAR_AUX_NUMERIC_PARAMETER_HELPER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/NumericParameter.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <string>

namespace cedar
{
  namespace aux
  {
    namespace NumericParameterHelper
    {
      inline bool isNumeric(cedar::aux::ConstParameterPtr parameter)
      {
        return cedar::aux::startsWith(cedar::aux::objectTypeToString(parameter), "cedar::aux::NumericParameter");
      }

      inline std::string getNumericType(cedar::aux::ConstParameterPtr parameter)
      {
        if (!isNumeric(parameter))
        {
          CEDAR_THROW(cedar::aux::TypeMismatchException, "Cannot get the type of number stored in the parameter: parameter is not numeric.");
        }

        std::string rest, last;
        cedar::aux::splitLast(cedar::aux::objectTypeToString(parameter), "<", rest, last);
        CEDAR_ASSERT(last.length() >= 2);
        return last.substr(0, last.length() - 1);
      }


      template <typename T>
      inline double getNumericValue(cedar::aux::ConstParameterPtr source)
      {
        auto numeric_parameter = boost::dynamic_pointer_cast<const cedar::aux::NumericParameter<T> >(source);
        CEDAR_ASSERT(numeric_parameter);
        return static_cast<double>(numeric_parameter->getValue());
      }

      template <typename T>
      inline void setNumericValue(cedar::aux::ParameterPtr source, double value)
      {
        auto numeric_parameter = boost::dynamic_pointer_cast<cedar::aux::NumericParameter<T> >(source);
        CEDAR_ASSERT(numeric_parameter);
        numeric_parameter->setValue(static_cast<T>(value));
      }

      inline double getValue(cedar::aux::ConstParameterPtr source)
      {
        std::string source_type = getNumericType(source);

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

      inline void setValue(cedar::aux::ParameterPtr target, double value)
      {
        std::string target_type = getNumericType(target);
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

    }
  }
}


#endif // CEDAR_AUX_NUMERIC_PARAMETER_HELPER_H
