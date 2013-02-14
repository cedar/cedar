/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/ParameterTemplate.h"

// SYSTEM INCLUDES
#include <boost/units/quantity.hpp>
#include <boost/units/io.hpp>
#include <boost/static_assert.hpp>

namespace cedar
{
  namespace aux
  {
    template <typename T>
    boost::units::quantity<T> getUnitFromPostFix(const std::string& /* postFix */)
    {
      // default implementation -- should not be used
      // If you see an error generated from here, you are using a dimension that is not implemented with parameters.
      // Specialize this function for the given dimension to prevent the error.
      BOOST_STATIC_ASSERT(sizeof(T) == 0);
    }
  }
}

//!@todo These functions should probably be in the appropriate header for the given parameters (e.g., LengthParameter.h)

namespace cedar
{
  namespace aux
  {
    template<typename BaseUnit, typename Dimension, typename Unit>
    void addSubUnitToMaps
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
    void addBaseUnitToMaps
    (
      std::map<std::string, boost::units::quantity<Dimension> >& map,
      const Unit& unit
    )
    {
      map[BaseUnit::name()] = 1.0 * unit;
      map[BaseUnit::symbol()] = 1.0 * unit;
    }

    template <typename Dimension>
    void addAliasToMaps
    (
      std::map<std::string, boost::units::quantity<Dimension> >& map,
      const std::string& name,
      const std::string& alias
    )
    {
      //!@todo Proper exceptions
      CEDAR_ASSERT(map.find(name) != map.end());
      CEDAR_ASSERT(map.find(alias) == map.end());

      map[alias] = map[name];
    }

    template <typename Dimension>
    boost::units::quantity<Dimension> findUnit
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
      //!@todo Proper exception
      CEDAR_ASSERT(false);
    }
  }
}


// Custom translator for bool (only supports std::string)
namespace cedar
{
  namespace aux
  {
    template <typename T>
    struct UnitTranslator
    {
      // boost-specific name, do not change or stuff will break!
      typedef std::string internal_type;
      // boost-specific name, do not change or stuff will break!
      typedef boost::units::quantity<T> external_type;

      // Converts a string to bool
      boost::optional<external_type> get_value(const internal_type& str)
      {
        size_t delim = str.find_first_not_of("0123456789.");
        std::string number_str = str.substr(0, delim);
        std::string unit_str = str.substr(delim);
        unit_str = cedar::aux::erase(unit_str, " ");
        double number = cedar::aux::fromString<double>(number_str);

        return boost::optional<external_type>(number * cedar::aux::getUnitFromPostFix<T>(unit_str));
      }

      // Converts a bool to string
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


/*!@todo describe.
 *
 * @todo describe more.
 */
template <class T>
class cedar::aux::UnitParameterTemplate : public cedar::aux::ParameterTemplate<boost::units::quantity<T> >
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef cedar::aux::ParameterTemplate<boost::units::quantity<T> > Super;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  UnitParameterTemplate
  (
    cedar::aux::Configurable* pOwner,
    const std::string& name,
    const boost::units::quantity<T>& defaultValue
  )
  :
  Super(pOwner, name, defaultValue)
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

