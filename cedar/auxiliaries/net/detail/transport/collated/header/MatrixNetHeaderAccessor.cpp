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

    File:        MatrixNetHeaderAccessor.cpp

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011 02:41:42 PM CEST

    Description: static function to get information out of the
                 matrix headers (that are transported over the network)
                 We implement this seperately to keep the actual 
                 transported class (struct) as small as possible.

    Credits:

=============================================================================*/

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/namespace.h"
#include "cedar/auxiliaries/net/detail/transport/collated/header/MatrixNetHeader.h"
#include "cedar/auxiliaries/net/detail/transport/collated/header/MatrixNetHeaderAccessor.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

//-----------------------------------------------------------------------------
// static functions
//-----------------------------------------------------------------------------

namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {

// this is a static function
unsigned int MatrixNetHeaderAccessor::getDataSize(MatrixNetHeader &header)
{
  return header.mElementSize * header.mRows * header.mColumns;
}

// this is a static function
unsigned int MatrixNetHeaderAccessor::getTotalElements(MatrixNetHeader &header)
{
  return header.mRows * header.mColumns;
}

// this is a static function
unsigned int MatrixNetHeaderAccessor::getElementSize(MatrixNetHeader &header)
{
  return header.mElementSize;
}


} } } } // end namespaces 

