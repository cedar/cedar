/*=============================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
    Date:        Wed 20 Jul 2011

    Description: Header (part of the transported information that contains
                 the structure information of the data proper) for
                 matrices. ie rows, columns

    Credits:

=============================================================================*/

#ifndef CEDAR_MATRIX_NET_HEADER_H
#define CEDAR_MATRIX_NET_HEADER_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/transport/collated/header/MatrixNetHeader.fwd.h"
#include "cedar/auxiliaries/net/detail/transport/collated/header/BasicNetHeader.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

//!@cond SKIPPED_DOCUMENTATION
/*!@brief extend the header struct to matrix data (cols, rows)
 *
 */
struct cedar::aux::net::detail::MatrixNetHeader : BasicNetHeader
{
public:
  /*unsigned*/ int mRows; // cv::Mat uses signed ints for this
  /*unsigned*/ int mColumns; // cv::Mat uses signed ints for this
  unsigned int mElementSize;
};
//!@endcond

#endif

