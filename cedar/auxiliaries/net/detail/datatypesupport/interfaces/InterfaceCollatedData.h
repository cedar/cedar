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

    File:        InterfaceCollatedData.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011 04:41:29 PM CEST

    Description: the transport part of the NetTransporter framework
                 needs this interface to be implemented

    Credits:

=============================================================================*/

#ifndef CEDAR_INTERFACECOLLATEDDATA_H
#define CEDAR_INTERFACECOLLATEDDATA_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/namespace.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/CollatedTraits.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES



namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {
//!@cond SKIPPED_DOCUMENTATION


/* Abstract interface of matrix-like data
 *
 * we hold typedef-info for the traits-type, data and header-type
 */
template<typename T> 
class InterfaceCollatedData
{
protected:
  typedef CollatedTraits<T>               TraitsType;
  typedef typename TraitsType::DataType   DataType;
  typedef typename TraitsType::HeaderType HeaderType;

protected:
  virtual bool checkCollatedDataForWrite(const DataType &data,
                                            HeaderType &header) = 0;
  virtual bool checkCollatedDataForRead(const HeaderType &header) = 0;
};

//!@endcond
} } } } // end namespace

#endif
