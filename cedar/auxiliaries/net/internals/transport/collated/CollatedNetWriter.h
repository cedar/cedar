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

    File:        CollatedNetWriter.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011 04:15:50 PM CEST

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_COLLATEDNETWRITER_H
#define CEDAR_COLLATEDNETWRITER_H

// LOCAL INCLUDES
#include "../../namespace.h"
#include "CollatedNetBase.h"
#include "../../datatypes/CollatedType.h"
#include "CollatedNetPortable.h"
#include "../../../exceptions/NetException.h"
#include "../AbstractNetWriter.h"

// PROJECT INCLUDES
#include <yarp/os/PortWriterBuffer.h>
#include <yarp/os/PortReport.h>

// SYSTEM INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {


/*!@brief implementation of matrix-over-network writer
 *
 * 
 */
template <typename T>
class CollatedNetWriter : 
                          public CollatedNetBase<T>,
                          public AbstractNetWriter<T>
{
  //---------------------------------------------------------------------------
  // members
  //---------------------------------------------------------------------------
protected:
  yarp::os::PortWriterBuffer< CollatedNetPortable< T > > mElemWrapper;

  //---------------------------------------------------------------------------
  // constructors and destructor
  //---------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  explicit CollatedNetWriter(std::string myPortName) : 
                                         CollatedNetBase<T>(),
                                         AbstractNetWriter<T>(myPortName),
                                         mElemWrapper()
  {
#ifdef DEBUG_NETT
    cout << "  CollatedNetWriter [CONSTRUCTOR]" << endl;
#endif
    AbstractNetWriter<T>::lateConstruct();

    mElemWrapper.attach( CollatedNetBase<T>::mDataPort );
  }

  //!@brief Destructor
  ~CollatedNetWriter() // doesnt need to be virtual
  {
    AbstractNetWriter<T>::lateDestruct();
#ifdef DEBUG_NETT
    cout << "  ~CollatedNetWriter [DESTRUCTOR]" << endl;
#endif
  }


  //---------------------------------------------------------------------------
  // public methods
  //---------------------------------------------------------------------------
public:
  /*!@brief implementation of writing a collated variable type
   *
   * will throw NetUnexpectedDataException if the matrix size/type
   * changed from the earlier trasmissions.
   *
   */
  void write(const T &t) 
  {
#ifdef DEBUG_NETT
    cout << "  CollatedNetWriter [write()]" << endl;
#endif

    // verify matrix size (or generate the header for further checks)
    if (!check_collateddata_for_write(t,
                                      mElemWrapper.prepare().header() ) )
    {
      CEDAR_THROW( cedar::aux::exc::NetUnexpectedDataException,
                   "matrix has wrong size - you wrote matrices of "
                   "different size/type before!" );
      return;
    }

    // send it ...
    mElemWrapper.prepare().content() = t;
    mElemWrapper.write(true); // TRUE IS IMPORTANT
  }

};

} } } } // end namespaces

#endif

