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

    File:        MatrixNetHeaderAccessor.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011 02:34:24 PM CEST

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_MATRIXNETHEADERACCESSOR_H
#define CEDAR_MATRIXNETHEADERACCESSOR_H

// LOCAL INCLUDES
#include "../../../namespace.h"
#include "MatrixNetHeader.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {


/*!@brief static helper for getting data out of the header-structure
 *
 * Of course this is not good OOP, but we use this C-style accessors
 * so that the structs that hold the data can be the smalles possible
 * for network transfer
 */
class MatrixNetHeaderAccessor
{
public:
  static unsigned int getDataSize(MatrixNetHeader &header);
  static unsigned int getTotalElements(MatrixNetHeader &header);
  static unsigned int getElemSize(MatrixNetHeader &header);
};

} } } } // end namespaces

#endif
