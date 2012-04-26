/*=============================================================================

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

===============================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        CollatedTraits.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011 04:43:49 PM CEST

    Description: CollatedTraits in the std::-namespace specialization

    Credits:

=============================================================================*/

#ifndef CEDAR_COLLATEDTRAITS_STD_H
#define CEDAR_COLLATEDTRAITS_STD_H

// LOCAL INCLUDES
// traits specializations:
#include "cedar/auxiliaries/net/detail/namespace.h"
#include "cedar/auxiliaries/net/detail/transport/collated/header/BasicNetHeader.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {
        template<> struct CollatedTraits<std::string>;
      }
    }
  }
}


//////////////// OPENCV SPECIALIZATION of our traits class

//!@brief CollatedTraits implementation for std::string
template<>
struct cedar::aux::net::detail::CollatedTraits<std::string>
{
  typedef std::string                                    DataType;
  typedef cedar::aux::net::detail::BasicNetHeader        HeaderType;
};


#endif

