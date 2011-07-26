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

    File:        CollatedNetReader.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Tue 19 Jul 2011 05:44:27 PM CEST

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_COLLATEDNETREADER_H
#define CEDAR_COLLATEDNETREADER_H

// LOCAL INCLUDES
#include "../../namespace.h"
#include "CollatedNetBase.h"
#include "../AbstractNetReader.h"
#include "../../datatypes/CollatedType.h"
#include "CollatedNetPortable.h"
#include "../../../exceptions/NetException.h"

// PROJECT INCLUDES
#include <yarp/os/PortReaderBuffer.h>

// SYSTEM INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {


/*!@brief implements reading (RIAA) of a complex data type (header + matrix)
 *
 * 'collated types' are types where a header-information and data-field
 * (which represents all of the matrix elements) are stuck together
 * and transmitted.
 *
 * partly implemented in CollatedNetBase
 */
template <typename T, bool block= false>
class CollatedNetReader : public CollatedNetBase<T>,
                          public AbstractNetReader<T>
{
  //---------------------------------------------------------------------------
  // members
  //---------------------------------------------------------------------------
private:
  yarp::os::PortReaderBuffer< CollatedNetPortable< T > > mElemWrapper;

  //---------------------------------------------------------------------------
  // constructors and destructor
  //---------------------------------------------------------------------------
private:
  //!@brief The standard constructor. dont use
  CollatedNetReader();
  CollatedNetReader(const CollatedNetReader &C); // not copyable
  CollatedNetReader &operator=(const CollatedNetReader &C);

public:
  //!@brief use this constructor
  explicit CollatedNetReader(const std::string &myPortName) 
                                       : CollatedNetBase<T>(),
                                         AbstractNetReader<T>(myPortName),
                                         mElemWrapper()
  {
#ifdef DEBUG
    cout << "  CollatedNetReader [CONSTRUCTOR]" << endl;
#endif
    mElemWrapper.setStrict(false); // false= drop old messages, when new
                                   // ones come in

    AbstractNetReader<T>::lateConstruct();
      // explanation: lateConstruct() needs to be called outside of
      //              AbstractNetBases/Readers constructors, so we
      //              can access virtual functions

    mElemWrapper.attach( CollatedNetBase<T>::mDataPort );
  }

  // no need for virtual here, as pointers of this will never be passed around
  ~CollatedNetReader()
  {
#ifdef DEBUG
    cout << "  ~CollatedNetReader [DESTRUCTOR] begin" << endl;
#endif
    AbstractNetReader<T>::lateDestruct();
#ifdef DEBUG
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
    T retMat;
    CollatedNetPortable< T > *pNetPortable;

    if (!AbstractNetReader<T>::isConnected())
    {
#ifdef DEBUG
      cout << "  tryring reconnect" << endl;
#endif

      // try to (re)connect at the latest possible time
      if (!AbstractNetReader<T>::reconnect())
      {
        // do something
        CEDAR_THROW( cedar::aux::exc::NetWaitingForWriterException,
                     "YARP: cannot connect reader port; waiting for "
                     "writer" );
      }
    }

    pNetPortable= mElemWrapper.read( block ); // Argument: shouldWait
 
    if ( pNetPortable == NULL )
    {
      CEDAR_THROW( cedar::aux::exc::NetUnexpectedDataException,
                   "YARP: unexpectedly read NULL from port" );
    }
    else
    {
      retMat= pNetPortable->content();

      // thoughts on reference counters:
      // 1.) YARP creates an instance of CollatedNetPortable in the background,
      //     we get a pointer to that here
      // 2.) ->content() gives us a reference to the data (e.g. cv::Mat)
      //                 so no change there.
      // 3.) retMat= ...; is a O(1) operation with cv::Mat and
      //                  actually doesnt copy anything, but
      //                  internally increases a reference counter
      // 4.) return retMat; is another O(1) copy operation and
      //                  and counter increase
      // 5.) when pNetPortable is freed (by yarp)
      //     and the local retMat is deleted
      //     there is one reference left to the matrix data, which
      //     is the one we returned to the user code. GOOD!

      // verify matrix size (or generate the header for further checks)
#if 1
      if (!check_collateddata_for_read( pNetPortable->header() ) )
      {
        CEDAR_THROW( cedar::aux::exc::NetUnexpectedDataException,
                     "matrix has wrong size - you wrote matrices of "
                     "different size/type before!" );
      }
#endif

      return retMat;
    }
  }
};

} } } }  // end namespaces

#endif

