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

    File:        CollatedNetBase.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011

    Description: Shared code between the CollatedNetWriter and -Reader.

    Credits:

=============================================================================*/

#ifndef CEDAR_COLLATED_NET_BASE_H
#define CEDAR_COLLATED_NET_BASE_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/transport/collated/CollatedNetBase.fwd.h"
#include "cedar/auxiliaries/net/detail/transport/interfaces/InterfaceOpenable.h"

#include "cedar/auxiliaries/net/detail/datatypesupport/CollatedTraits.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/MatrixTypeWrapper.h"
#include "cedar/auxiliaries/net/detail/transport/collated/YARPCollatedPortable.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

//!@cond SKIPPED_DOCUMENTATION
/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
template <typename T> 
class cedar::aux::net::detail::CollatedNetBase : virtual protected InterfaceOpenable
{
  //---------------------------------------------------------------------------
  // members
  //---------------------------------------------------------------------------
protected:
  yarp::os::Port mDataPort;

  //---------------------------------------------------------------------------
  // constructors and destructor
  //---------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  CollatedNetBase() 
    : mDataPort()
  {
#ifdef DEBUG_NETT
  cout << "  CollatedNetBase [CONSTRUCTOR]" << endl;
#endif
    mDataPort.enableBackgroundWrite(true);

  }

  //!@brief Destructor, virtual to be sure
  virtual ~CollatedNetBase()
  {
  }


  //---------------------------------------------------------------------------
  // public methods
  //---------------------------------------------------------------------------
public:
  bool open()
  {
    if (getFullPortName().find_first_of(" \r\n") != std::string::npos)
    {
      return false;
    }
    return mDataPort.open(getFullPortName().c_str());
  }

  bool close()
  {
    mDataPort.close(); // returns void
    return true;
  }


};
//!@endcond

#endif
