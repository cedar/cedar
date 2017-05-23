/*=============================================================================

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

===============================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        CollatedTraits.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011

    Description: traits class for the transferable data type.
                 Specialize this.

    Credits:

=============================================================================*/

#ifndef CEDAR_COLLATED_TRAITS_H
#define CEDAR_COLLATED_TRAITS_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/datatypesupport/CollatedTraits.fwd.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/static_assert.hpp>
#endif
#include <string>

//!@cond SKIPPED_DOCUMENTATION


/*!@brief traits struct for a 'collated data type' i.e. a matrix type
 *
 * the traits struct will hold information about necessary helper classes
 * and HAS TO BE SPECIALIZED
 */
template<class T>
struct cedar::aux::net::detail::CollatedTraits
{
  // default traits struct is empty
  // this should not compile
  CollatedTraits()
  {
    BOOST_STATIC_ASSERT(sizeof(T) == 0); 
  }
};

//!@endcond

// specializations (these will compile):
#include "cedar/auxiliaries/net/detail/datatypesupport/opencv/CollatedTraits.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/std/CollatedTraits.h"


#endif

