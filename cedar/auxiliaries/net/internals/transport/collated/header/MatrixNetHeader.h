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

    File:        MatrixNetHeader.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011 02:37:21 PM CEST

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_MATRIXNETHEADER_H
#define CEDAR_MATRIXNETHEADER_H

// LOCAL INCLUDES
#include "../../../namespace.h"
#include "CollatedNetHeader.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {

/*!@brief extend the header struct to matrix data (cols, rows)
 *
 */
#include <yarp/os/begin_pack_for_net.h>
struct MatrixNetHeader : CollatedNetHeader
{
public:
  yarp::os::NetInt32 rows;
  yarp::os::NetInt32 cols;
  yarp::os::NetUint32 elemSize;
} PACKED_FOR_NET;

} } } }  // end namespaces

#endif

