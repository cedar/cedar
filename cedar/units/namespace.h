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

    File:        namespace.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 05 23

    Description: Namespace file for cedar::units.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_UNITS_NAMESPACE_H
#define CEDAR_UNITS_NAMESPACE_H

// CEDAR INCLUDES
#include "cedar/units/lib.h"

// SYSTEM INCLUDES
#include <string>
#include <boost/smart_ptr.hpp>


namespace cedar
{
  /*!@brief Namespace for all aux classes. */
  namespace unit
  {
    // because strings (as template arguments) must be constant at compile-time, this is used for the prefixes:
    //!@brief string prefix for microseconds
    extern CEDAR_UNITS_LIB_EXPORT const char prefix_us[];
    //!@brief string prefix for milliseconds
    extern CEDAR_UNITS_LIB_EXPORT const char prefix_ms[];
    //!@brief string prefix for seconds
    extern CEDAR_UNITS_LIB_EXPORT const char prefix_s[];

    //!@brief the base class Time
    class CEDAR_UNITS_LIB_EXPORT Time;

    /*! @todo This should probably be UnitBase and inherit from a class that is a template argument as well; that way,
     *        it can be used for units other than time as well.
     *        However, the downside might be that one can write something like
     *        time = speed;
     *        etc., which should not work. A solution might be to use Time (etc.) as a member, rather than inheriting
     *        from it, and working with Time objects instead of doubles?
     */
    template <unsigned int factor, const char* suffix> class TimeUnit;

    //!@brief the template concretization for microseconds
    typedef TimeUnit<1, prefix_us> Microseconds;
    //!@brief the template concretization for milliseconds
    typedef TimeUnit<1000, prefix_ms> Milliseconds;
    //!@brief the template concretization for seconds
    typedef TimeUnit<1000000, prefix_s> Seconds;
  }
}

#endif // CEDAR_UNITS_NAMESPACE_H
