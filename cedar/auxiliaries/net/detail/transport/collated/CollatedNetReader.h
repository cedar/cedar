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

    File:        CollatedNetReader.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Tue 19 Jul 2011

    Description: Implementation for the user-accessible reading
                 of a matrix-like type. 
                 We use a yarp::os::Portable-derived type 
                 called 'YARPCollatedPortable' and YARP will handle
                 the passing of data between us an it.
                 Here we check for consistency before passing
                 the data to the user.

    Credits:

=============================================================================*/

#ifndef CEDAR_COLLATED_NET_READER_H
#define CEDAR_COLLATED_NET_READER_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/transport/collated/CollatedNetReader.fwd.h"
#include "cedar/auxiliaries/net/detail/transport/collated/CollatedNetBase.h"
#include "cedar/auxiliaries/net/detail/transport/AbstractNetReader.h"
#include "cedar/auxiliaries/net/detail/datatypesupport/MatrixTypeWrapper.h"
#include "cedar/auxiliaries/net/detail/transport/collated/YARPCollatedPortable.h"
#include "cedar/auxiliaries/net/exceptions.h"

// PROJECT INCLUDES
#include <yarp/os/PortReaderBuffer.h>

// SYSTEM INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

//!@cond SKIPPED_DOCUMENTATION
/*!@brief implements reading (RIAA) of a complex data type (header + matrix)
 *
 * 'collated types' are types where a header-information and data-field
 * (which represents all of the matrix elements) are stuck together
 * and transmitted.
 *
 * partly implemented in CollatedNetBase
 */
template <typename T, bool BLOCK= false>
class cedar::aux::net::detail::CollatedNetReader : public CollatedNetBase<T>,
                                                   public AbstractNetReader<T>
{
  //---------------------------------------------------------------------------
  // members
  //---------------------------------------------------------------------------
private:
  yarp::os::PortReaderBuffer< YARPCollatedPortable< T > > mElementWrapper;

  //---------------------------------------------------------------------------
  // constructors and destructor
  //---------------------------------------------------------------------------
private:
  //!@brief The standard constructor. dont use
  // CollatedNetReader();
  CollatedNetReader(const CollatedNetReader &C); // not copyable
  CollatedNetReader &operator=(const CollatedNetReader &C); // not copyable

  virtual bool checkCollatedDataForRead(const typename CollatedTraits<T>::HeaderType &header) = 0;

public:
  //!@brief use this constructor
  explicit CollatedNetReader(const std::string &myPortName) 
                                       : CollatedNetBase<T>(),
                                         AbstractNetReader<T>(myPortName),
                                         mElementWrapper()
  {
#ifdef DEBUG_NETT
    cout << "  CollatedNetReader [CONSTRUCTOR]" << endl;
#endif
    mElementWrapper.setStrict(false); // false= drop old messages, when new
                                   // ones come in

    AbstractNetReader<T>::lateConstruct();
      // explanation: lateConstruct() needs to be called outside of
      //              AbstractNetBases/Readers constructors, so we
      //              can access virtual functions

    mElementWrapper.attach( CollatedNetBase<T>::mDataPort );
  }

  // no need for virtual here, as pointers of this will never be passed around
  ~CollatedNetReader()
  {
#ifdef DEBUG_NETT
    cout << "  ~CollatedNetReader [DESTRUCTOR] begin" << endl;
#endif
    AbstractNetReader<T>::earlyDestruct();
#ifdef DEBUG_NETT
    cout << "  ~CollatedNetReader [DESTRUCTOR] end" << endl;
#endif
  }

  //---------------------------------------------------------------------------
  // public methods
  //---------------------------------------------------------------------------
public:
  /*!@brief implementation of reading a collated variable type
   *
   * the header is pre-prepared and will be resent every time
   * the matrix content will be sent as whole data-chunk
   *
   *
   * will throw NetWaitingForWriterException if Writer hasnt been 
   * started yet. Catch this exception if you want to poll or wait
   * for writer creation.
   *
   */
  T read()
  {  
    T ret_mat;
    YARPCollatedPortable< T > *p_net_portable;

    if (!AbstractNetReader<T>::isConnected())
    {
#ifdef DEBUG_NETT
      cout << "  tryring reconnect" << endl;
#endif

      // try to (re)connect at the latest possible time
      if (!AbstractNetReader<T>::reconnect())
      {
        // do something
        // user may catch this and choose to retry later on ...
        CEDAR_THROW( cedar::aux::net::NetWaitingForWriterException,
                     "YARP: cannot connect reader port; waiting for "
                     "writer" );
      }
    }

    p_net_portable = mElementWrapper.read(BLOCK); // Argument: shouldWait

 
    if (p_net_portable == nullptr)
    {
      if (CollatedNetBase<T>::mDataPort.getInputCount() == 0)
      {
        CEDAR_THROW(cedar::aux::net::NetWaitingForWriterException, "YARP: currently no data in buffer (writer disconnected)");
      }
      else
      {
        CEDAR_THROW(cedar::aux::net::NetNoNewDataException, "YARP: currently no data in buffer (reading)");
      }
    }
    else
    {
      ret_mat = p_net_portable->content();

      // thoughts on reference counters:
      // 1.) YARP creates an instance of YARPCollatedPortable in the background,
      //     we get a pointer to that here (p_net_portable)
      // 2.) ->content() gives us a reference to the data (e.g. cv::Mat)
      //                 so no change there.
      // 3.) ret_mat= ...; is a O(1) operation with cv::Mat and
      //                  actually doesnt copy anything, but
      //                  internally increases a reference counter
      // 4.) return ret_mat; is another O(1) copy operation and
      //                  and counter increase
      // 5.) when p_net_portable is freed (by yarp)
      //     and the local ret_mat is deleted (when leaving this function)
      //     there is one reference left to the matrix data, which
      //     is the one we returned to the user code. GOOD!

      // verify matrix size (or generate the header for further checks)
#if 1
      if (!checkCollatedDataForRead( p_net_portable->header() ) )
      {
        CEDAR_THROW( cedar::aux::net::NetUnexpectedDataException,
                     "matrix has wrong size - you wrote matrices of "
                     "different size/type before! Try to reset." );
      }
#endif

      return ret_mat;
    }
  }
};
//!@endcond

#endif
