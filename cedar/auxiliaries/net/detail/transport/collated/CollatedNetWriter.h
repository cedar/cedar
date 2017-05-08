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

    File:        CollatedNetWriter.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Wed 20 Jul 2011

    Description: Implementation for the user-accessible writing 
                 of a matrix-like type. 
                 We use a yarp::os::Portable-derived type 
                 called 'YARPCollatedPortable' and YARP will handle
                 the passing of data between us an it.
                 Here we check for consistency of the data the user
                 passed to us.

    Credits:

=============================================================================*/

#ifndef CEDAR_COLLATED_NET_WRITER_H
#define CEDAR_COLLATED_NET_WRITER_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/transport/collated/CollatedNetWriter.fwd.h"
#include "cedar/auxiliaries/net/detail/transport/collated/CollatedNetBase.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/MatrixTypeWrapper.h"
#include "cedar/auxiliaries/net/detail/transport/collated/YARPCollatedPortable.h"
#include "cedar/auxiliaries/net/exceptions.h"
#include "cedar/auxiliaries/net/detail/transport/AbstractNetWriter.h"

// PROJECT INCLUDES
#include <yarp/os/PortWriterBuffer.h>
#include <yarp/os/PortReport.h>

// SYSTEM INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

//!@cond SKIPPED_DOCUMENTATION
/*!@brief implementation of matrix-over-network writer
 *
 * @see CollatedNetReader
 */
template <typename T>
class cedar::aux::net::detail::CollatedNetWriter : public CollatedNetBase<T>,
                          public AbstractNetWriter<T>
{
  //---------------------------------------------------------------------------
  // members
  //---------------------------------------------------------------------------
protected:
  yarp::os::PortWriterBuffer< YARPCollatedPortable< T > > mElementWrapper;

  //---------------------------------------------------------------------------
  // constructors and destructor
  //---------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  explicit CollatedNetWriter(std::string myPortName) 
    : CollatedNetBase<T>(),
      AbstractNetWriter<T>(myPortName),
      mElementWrapper()
  {
#ifdef DEBUG_NETT
    cout << "  CollatedNetWriter [CONSTRUCTOR]" << endl;
#endif
    AbstractNetWriter<T>::lateConstruct(); // see CollatedNetReader for comment

    mElementWrapper.attach( CollatedNetBase<T>::mDataPort );
  }

  //!@brief Destructor
  ~CollatedNetWriter() // doesnt need to be virtual
  {
    AbstractNetWriter<T>::earlyDestruct();
#ifdef DEBUG_NETT
    cout << "  ~CollatedNetWriter [DESTRUCTOR]" << endl;
#endif
  }

private:
  virtual bool checkCollatedDataForWrite(const typename CollatedTraits<T>::DataType &data,
                                         typename CollatedTraits<T>::HeaderType &header) = 0;

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
    if (!checkCollatedDataForWrite(t, mElementWrapper.prepare().header()))
    {
      CEDAR_THROW(cedar::aux::net::NetUnexpectedDataException,
                  "matrix has wrong size - you wrote matrices of "
                  "different size/type before! Try to reset.");
      return;
    }

    // send it ...
    mElementWrapper.prepare().content() = t;
    mElementWrapper.write(true); // TRUE IS IMPORTANT
  }

};
//!@endcond

#endif
