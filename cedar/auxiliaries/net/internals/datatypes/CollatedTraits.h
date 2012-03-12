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
    Date:        Wed 20 Jul 2011 04:30:14 PM CEST

    Description: traits class for the transferable data type.
                 Specialize this.

    Credits:

=============================================================================*/

#ifndef CEDAR_COLLATEDTRAITS_H
#define CEDAR_COLLATEDTRAITS_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/internals/namespace.h"

// PROJECT INCLUDES
#include <boost/static_assert.hpp>

// SYSTEM INCLUDES



namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {


/*!@brief traits struct for a 'collated data type' i.e. a matrix type
 *
 * the traits struct will hold information about necessary helper classes
 * and HAS TO BE SPECIALIZED
 */
template<class T>
struct collated_traits
{
  // default traits struct is empty
  // this should not compile
  collated_traits()
  {
    BOOST_STATIC_ASSERT(sizeof(T) == 0); 
  }
};


} } } } // end namespaces 

// specializations (these will compile):
#include "cedar/auxiliaries/net/internals/datatypes/opencv/CollatedTraits.h"


#endif

