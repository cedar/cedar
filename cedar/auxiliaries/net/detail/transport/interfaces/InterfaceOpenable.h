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

    File:        InterfaceOpenable.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Tue 19 Jul 2011

    Description: Make sure that the Collated/Simple-type classes 
                 basically behave the same way.

                 Will be used by AbstracNetBase and CollatedNetBase.
                 Inherit virtually.

    Credits:

=============================================================================*/

#ifndef CEDAR_INTERFACE_OPENABLE_H
#define CEDAR_INTERFACE_OPENABLE_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/transport/interfaces/InterfaceOpenable.fwd.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <string>

//!@cond SKIPPED_DOCUMENTATION
/*!@brief internal Interface 
 *
 * Implemented by AbstractNetWriter and Reader
 */
class cedar::aux::net::detail::InterfaceOpenable
{
protected:
  /*!@brief open the port (called at RIAA object creation) */
  virtual bool open() = 0;
  /*!@brief close the port (called at RIAA object destruction) */
  virtual bool close() = 0;
  /*!@brief port name (not the user-given one, the full configured one) */
  virtual std::string getFullPortName() = 0;
};

//!@endcond

#endif
