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

    File:        namespace.h

    Maintainer:  Mathis Richter

    Email:       mathis.richter@ini.rub.de

    Date:        2013 02 18

    Description: Namespace file for cedar::units.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_UNITS_NAMESPACE_H
#define CEDAR_UNITS_NAMESPACE_H

// CEDAR INCLUDES
#include "cedar/units/lib.h"

// SYSTEM INCLUDES

/*! Defined to indicate that the new unit framework is in effect. This can be used to keep software compatible with
 *  multiple versions of cedar.
 */
#define CEDAR_UNITS_FRAMEWORK_V2

namespace cedar
{
  /*!@brief Namespace for all aux classes. */
  namespace unit
  {
    //!@cond SKIPPED_DOCUMENTATION
    template <typename DimensionType> struct UnitMatrix;
    //!@endcond
  }
}

#endif // CEDAR_UNITS_NAMESPACE_H
