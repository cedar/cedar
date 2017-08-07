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

    File:        UnitParameterTemplate.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 02 12

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_UNIT_PARAMETER_TEMPLATE_H
#define CEDAR_AUX_UNIT_PARAMETER_TEMPLATE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/NumericParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/UnitParameterTemplate.fwd.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/units/base_dimension.hpp>
  #include <boost/units/derived_dimension.hpp>
  #include <boost/units/get_dimension.hpp>
  #include <boost/units/quantity.hpp>
  #include <boost/units/pow.hpp>
  #include <boost/units/systems/si/dimensionless.hpp>
  #include <boost/units/io.hpp>
  #include <boost/static_assert.hpp>
#endif // Q_MOC_RUN
#include <map>
#include <vector>
#include <string>
#include <sstream>

namespace cedar
{
  namespace aux
  {
    //! Extracts a unit from a string. Done via template specialization.
    template <typename T>
    inline boost::units::quantity<T> getUnitFromPostFix(const std::string& /* postFix */)
    {
      // default implementation -- should not be used
      // If you see an error generated from here, you are using a dimension that is not implemented with parameters.
      // Specialize this function for the given dimension to prevent the error.
      BOOST_STATIC_ASSERT(sizeof(T) == 0);
    }


    //! Specialization of cedar::aux::getUnitFromPostFix for dimensionless units.
    template <>
    inline boost::units::quantity<boost::units::si::dimensionless> getUnitFromPostFix(const std::string& postFix)
    {
      CEDAR_ASSERT(postFix.empty());
      return 1.0;
    }
  }
}

namespace cedar
{
  namespace aux
  {
    template<typename Dimension, typename Unit>
    inline void addSubUnitToMaps
    (
      std::map<std::string, boost::units::quantity<Dimension> >& map,
      const Unit& unit,
      const std::string& name,
      const std::string& symbol,
      double factor
    )
    {
      map[name] = factor * unit;
      map[symbol] = factor * unit;
    }

    template<typename BaseUnit, typename Dimension, typename Unit>
    inline void addBaseUnitToMaps
    (
      std::map<std::string, boost::units::quantity<Dimension> >& map,
      const Unit& unit
    )
    {
      map[BaseUnit::name()] = 1.0 * unit;
      map[BaseUnit::symbol()] = 1.0 * unit;
    }

    template <typename Dimension>
    inline void addAliasToMaps
    (
      std::map<std::string, boost::units::quantity<Dimension> >& map,
      const std::string& name,
      const std::string& alias
    )
    {
      if (map.find(name) == map.end())
      {
        CEDAR_THROW(cedar::aux::UnknownNameException, "Cannot add an alias for \"" + name + "\": not a known unit.");
      }
      if (map.find(alias) != map.end())
      {
        CEDAR_THROW(cedar::aux::DuplicateNameException, "Cannot add alias \"" + alias + "\" for unit \"" + name + "\": alias already exists.");
      }

      map[alias] = map[name];
    }

    template <typename Dimension>
    inline boost::units::quantity<Dimension> findUnit
    (
      const std::string& postFix,
      std::map<std::string, boost::units::quantity<Dimension> >& map
    )
    {
      // find the unit post fix in the map
      auto iter = map.find(postFix);
      if (iter != map.end())
      {
        return iter->second;
      }

      // could not find unit, throw exception
      CEDAR_THROW(cedar::aux::UnknownUnitSuffixException, "Could not find unit for suffix \"" + postFix + "\".");
    }

    template <typename UnitType> // unit type is, e.g., cedar::aux::TimeUnit::unit_type
    std::string getDefaultUnit()
    {
      return "";
    }

    /*!
     * @remarks Ignores the exponents in the string, the calling function has to take care of this.
     */
    template <typename TUnit>
    inline boost::units::quantity<TUnit> parseUnitString(const std::string& unitStr)
    {
      std::vector<std::string> split;
      cedar::aux::split(unitStr, "^", split);

      CEDAR_ASSERT(split.size() > 0);
      CEDAR_ASSERT(split.size() <= 2);

      return cedar::aux::getUnitFromPostFix<TUnit>(split.at(0));
    }

    template
    <
      typename T,
      typename Numerator,
      typename Denominator,
      long NumeratorPower,
      long DenominatorPower
    >
    inline boost::units::quantity<T> parseCompoundUnit(const std::string& unitStr)
    {
      typedef typename boost::units::static_rational<NumeratorPower>::type NumeratorRational;
      typedef typename boost::units::static_rational<DenominatorPower>::type DenominatorRational;

      // normalize the white space in the unit
      std::string unit_str = cedar::aux::regexReplace(unitStr, "\\s+", " ");

      // normalize exponents
      unit_str = cedar::aux::regexReplace(unit_str, "\\s*\\^\\s*", "^");

      std::vector<std::string> component_strs;
      cedar::aux::split(unit_str, " ", component_strs);

      // there must be at least one unit string
      CEDAR_ASSERT(component_strs.size() >= 1);

      double numerator = 1.0;
      double denominator = 1.0;

      Numerator i_numerator;
      Denominator i_denominator;
      auto numerator_pow = boost::units::pow<NumeratorRational>(i_numerator);
      auto denominator_pow = boost::units::pow<DenominatorRational>(i_denominator);

      for (auto iter = component_strs.begin(); iter != component_strs.end(); ++iter)
      {
        const std::string& component_unit = *iter;

        if (component_unit.empty())
        {
          continue;
        }

        std::vector<std::string> split;
        cedar::aux::split(component_unit, "^", split);

        // strings should only be of the form x or x^n
        CEDAR_ASSERT(split.size() > 0);
        CEDAR_ASSERT(split.size() <= 2);
        const std::string& unit = split.at(0);

        if (split.size() == 1)
        {
          // this is a unit in the numerator, no powers attached
          numerator *= cedar::aux::parseUnitString<Numerator>(unit) / i_numerator;
        }
        else
        {
          const std::string& exponent_str = split.at(1);
          long exponent = cedar::aux::fromString<long>(exponent_str);

          CEDAR_ASSERT(exponent_str.size() >= 1);

          if (exponent < 0)
          {
            CEDAR_ASSERT(std::abs(exponent) == DenominatorPower);
            denominator *= cedar::aux::parseUnitString<Denominator>(unit) / i_denominator;
          }
          else
          {
            CEDAR_ASSERT(std::abs(exponent) == NumeratorPower);
            numerator *= cedar::aux::parseUnitString<Numerator>(unit) / i_numerator;
          }
        }

      } // for loop
      return (numerator / denominator) * (numerator_pow / denominator_pow);
    }

  } // namespace aux
} // namespace cedar


namespace cedar
{
  namespace aux
  {
    /*!@brief A base class for translating units from and to strings.
     *
     * This class makes heavy use of template specialization for the different units, mainly of
     * cedar::aux::parseUnitString. These specializations can usually be found in the header corresponding to the
     * parameter for that unit.
     *
     * For example, for a cedar::units::Length, these can be found in cedar/aux/LengthParameter.h
     *
     * @tparam T type of unit to be translated from or to a boost::units::quantity<T>.
     */
    template <typename T>
    struct UnitTranslator
    {
      //! boost-specific name, do not change or stuff will break!
      typedef std::string internal_type;
      //! boost-specific name, do not change or stuff will break!
      typedef boost::units::quantity<T> external_type;

      //! Converts a string to a quantity.
      boost::optional<external_type> get_value(const internal_type& str)
      {
        // normalize all white space to a single space
        std::string norm_str = cedar::aux::regexReplace(str, "\\s+", " ");

        // remove white space at the beginning and end of the string
        norm_str = cedar::aux::regexReplace(norm_str, "(^\\s+|\\s+$)", "");

        size_t delim = norm_str.find_first_not_of("-0123456789.e");
        std::string number_str = norm_str.substr(0, delim);
        std::string unit_str;
        if (delim != std::string::npos)
        {
          unit_str = norm_str.substr(delim);
        }
        else
        {
          unit_str = getDefaultUnit<T>();
        }

        if (number_str.empty() || unit_str.empty())
        {
          CEDAR_THROW(cedar::aux::InvalidValueException, "Could not extract unit or value from string \"" + str + "\".");
        }

        // normalize the white space in the unit
        unit_str = cedar::aux::regexReplace(unit_str, "\\s+", " ");

        // remove whitespace at the beginning of the unit string
        delim = unit_str.find_first_not_of(" ");
        if (delim == std::string::npos)
        {
          delim = 0;
        }
        unit_str = unit_str.substr(delim);

        // get the number to put in front of the unit
        double number = cedar::aux::fromString<double>(number_str);

        // unit is not a combined one, just return plain unit
        return boost::optional<external_type>(number * cedar::aux::parseUnitString<T>(unit_str));
      }

      //! Converts a unit to string.
      boost::optional<internal_type> put_value(const external_type& unit)
      {
        std::stringstream stream;
        stream << unit;
        return boost::optional<internal_type>(stream.str());
      }
    };
  }
}

//  Specialize translator_between to specify our own way of reading units
//!@cond SKIPPED_DOCUMENTATION
namespace boost
{
  namespace property_tree
  {
    template<typename T, typename Ch, typename Traits, typename Alloc>
    struct translator_between<std::basic_string<Ch, Traits, Alloc >, boost::units::quantity<T> >
    {
        typedef cedar::aux::UnitTranslator<T> type;
    };
  } // namespace property_tree
} // namespace boost
//!@endcond


/*!@brief A template class for parameters that store values with attached units.
 */
template <class T>
class cedar::aux::UnitParameterTemplate : public cedar::aux::NumericParameter<boost::units::quantity<T> >
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef cedar::aux::NumericParameter<boost::units::quantity<T> > Super;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor that takes a default value.
  UnitParameterTemplate
  (
    cedar::aux::Configurable *pOwner,
    const std::string& name,
    const boost::units::quantity<T>& defaultValue,
    const typename cedar::aux::NumericParameter<boost::units::quantity<T> >::LimitType& limits
      = cedar::aux::NumericParameter<boost::units::quantity<T> >::LimitType::full(),
    const double& singleStep = double(1.0) // quickfix
  )
  :
  Super(pOwner, name, defaultValue, limits, singleStep)
  {
  }

  //! Constructor without default value.
  UnitParameterTemplate
  (
    cedar::aux::Configurable* pOwner,
    const std::string& name,
    const boost::units::quantity<T>& defaultValue,
    const boost::units::quantity<T>& minimum,
    const boost::units::quantity<T>& maximum,
    const double& singleStep = double(1.0) // quickfix
  )
  :
  Super(pOwner, name, defaultValue, minimum, maximum, singleStep)
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::UnitParameterTemplate

#endif // CEDAR_AUX_UNIT_PARAMETER_TEMPLATE_H

