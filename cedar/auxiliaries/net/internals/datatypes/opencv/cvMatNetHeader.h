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

    File:        cvMatNetHeader.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011 05:13:06 PM CEST

    Description: extend the MatrixNetHeader to hold information abaout
                 the matrix type.
                 We need this, because cv::Mat is vastly configurable
                 at runtime

    Credits:

=============================================================================*/

#ifndef CEDAR_CVMATNETHEADER_H
#define CEDAR_CVMATNETHEADER_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/internals/namespace.h"
#include "cedar/auxiliaries/net/internals/transport/collated/header/MatrixNetHeader.h"


// PROJECT INCLUDES

// SYSTEM INCLUDES


namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {

/*!@brief packed header with matrix info for network transfer
 *
 * This is the openCV::Mat specialization
 */
struct cvMatNetHeader : MatrixNetHeader
{
public:
  unsigned int cvMatType;
};

} } } } // end namespaces

#endif

