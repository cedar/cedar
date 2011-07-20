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

    Description:

    Credits:

=============================================================================*/

// LOCAL INCLUDES
#include "../../../namespace.h"
#include "MatrixNetHeader.h"
#include "MatrixNetHeaderAccessor.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

//-----------------------------------------------------------------------------
// static functions
//-----------------------------------------------------------------------------

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {

unsigned int MatrixNetHeaderAccessor::getDataSize(MatrixNetHeader &header)
{
  return header.elemSize * header.rows * header.cols;
}

unsigned int MatrixNetHeaderAccessor::getTotalElements(MatrixNetHeader &header)
{
  return header.rows * header.cols;
}

unsigned int MatrixNetHeaderAccessor::getElemSize(MatrixNetHeader &header)
{
  return header.elemSize;
}


} } } } // end namespaces 

