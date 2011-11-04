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

    File:        CollatedNetHeader.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011 02:32:04 PM CEST

    Description: this doesnt do much. Even before we think about 
                 what a matrix header contains, we want to have some
                 primitive way to check for corruption.

    Credits:

=============================================================================*/

#ifndef CEDAR_COLLATEDNETHEADER_H
#define CEDAR_COLLATEDNETHEADER_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/internals/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES



namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {

/*!@brief net portable struct that will hold the header (of a matrix)
 *
 * This will be inherited and extended for for example a header for
 * cv-matrizes.
 *
 * These structures are small by default, i.e. dont hold accessor
 * functions. For accessing the data, 
 * see the static class MatrixNetHeaderAccessor
 */
#include <yarp/os/begin_pack_for_net.h>
struct CollatedNetHeader
{
  // dummy for polymorphism
  yarp::os::NetUint32 magicNumber;
} PACKED_FOR_NET;

} } } } // end namespaces
      
#endif
