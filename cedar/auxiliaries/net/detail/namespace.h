/*=============================================================================

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

===============================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        namespace.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 10 Aug 2011 11:37:35 AM CEST

    Description: define the cedar::aux::net::detail namespace

    Credits:

=============================================================================*/

#ifndef CEDAR_NAMESPACE_H
#define CEDAR_NAMESPACE_H

#include "cedar/configuration.h"
#include "cedar/auxiliaries/namespace.h"

namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {
        //!@cond SKIPPED_DOCUMENTATION
        CEDAR_DECLARE_AUX_CLASS(AbstractNetBase);

        template <typename T>
        class AbstractNetReader;
        template <typename T>
        class AbstractNetWriter;

        template <typename T, bool BLOCK>
        class SimpleNetReader;
        template <typename T>
        class SimpleNetWriter;
         
        CEDAR_DECLARE_AUX_CLASS(InterfaceOpenable);

        struct CEDAR_AUX_LIB_EXPORT BasicNetHeader;
        struct CEDAR_AUX_LIB_EXPORT MatrixNetHeader;

        template <typename T> 
        class CollatedNetBase;
        template <typename T, bool BLOCK>
        class CollatedNetReader;
        template <typename T>
        class CollatedNetWriter;
        template<class T>
        class YARPCollatedPortable;

        template<typename T> 
        class InterfaceCollatedData;

        template<class T>
        struct CollatedTraits;

        template <typename T>
        struct MatrixTypeWrapper;

        template <typename CVT>
        class cvMatHelper;
        template <typename CVT>
        class StringHelper;
        //!@endcond
      }
    }
  }
}

#endif

