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

    File:        utilities.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 08 11

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_UTILITIES_H
#define CEDAR_AUX_UTILITIES_H

// LOCAL INCLUDES
#include "auxiliaries/macros.h"

// SYSTEM INCLUDES


namespace cedar
{
  namespace aux
  {
    /*!@brief   If you think a dynamic cast can never fail, use this cast instead.
     *
     *          In debug builds, it automatically checks whether you are right. In release mode, it statically casts to
     *          the desired type.
     *          The advantage of using this function is a speed gain: dynamic_casts are slow compared to static_casts.
     *
     * @remarks The syntax for using this cast is DerivedType* p2 = cedar::aux::asserted_cast<DerivedType*>(p);
     *          The second template argument does not have to be specified; it is determined automatically by the
     *          compiler!
     */
    template <typename TOut, typename TIn>
    TOut asserted_cast(TIn pIn)
    {
      CEDAR_DEBUG_ASSERT(dynamic_cast<TOut>(pIn) != NULL);
      return static_cast<TOut>(pIn);
    }
  }
}

#endif // CEDAR_AUX_UTILITIES_H