/*============================================================================

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

    File:        SimpleNetReader.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Tue 19 Jul 2011 05:10:54 PM CEST

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_SIMPLENETREADER_H
#define CEDAR_SIMPLENETREADER_H

// LOCAL INCLUDES
#include "../../namespace.h"
#include "../AbstractNetReader.h"
#include "../../../exceptions/NetException.h"

// PROJECT INCLUDES
#include <yarp/os/BufferedPort.h>
#include <yarp/os/BinPortable.h>

// SYSTEM INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {

/*!@brief implements reading (RIAA over network) simple variable types
 *
 * use in NetReader
 */
template <typename T, bool block= false>
class SimpleNetReader : public AbstractNetReader<T>
{
  //---------------------------------------------------------------------------
  // members
  //---------------------------------------------------------------------------
private:
  yarp::os::BufferedPort< yarp::os::BinPortable<T> > BufferIn;

  //---------------------------------------------------------------------------
  // constructors and destructor
  //---------------------------------------------------------------------------

private:
  //!@brief The standard constructor. dont use
  SimpleNetReader();
  SimpleNetReader(const SimpleNetReader &S); // not copyable
  SimpleNetReader &operator=(const SimpleNetReader &S);

public:
  explicit SimpleNetReader(const std::string &myPortName) 
                                     : AbstractNetReader<T>(myPortName),
                                       BufferIn()
  {
    AbstractNetReader<T>::lateConstruct();
      // explanation: lateConstruct() needs to be called outside of
      //              AbstractNetBases/Readers constructors, so we
      //              can access virtual functions from there
  }

  // no need for virtual here, as pointers of this will never be passed around
  //!@brief Destructor
  ~SimpleNetReader()
  {
    AbstractNetReader<T>::lateDestruct();
#ifdef DEBUG
    cout << "  ~SimpleNetReader [DESTRUCTOR]" << endl;
#endif
  }


  //---------------------------------------------------------------------------
  // protected methods
  //---------------------------------------------------------------------------
protected:
  //!@brief open the port. called by AbstractNetBase
  void open()
  {
    BufferIn.open( AbstractNetBase::getFullPortName().c_str() ); 
  }

  //!@brief close the port. called by AbstractNetBase
  void close()
  {
    if (!BufferIn.isClosed())
      BufferIn.close();
  }

  //---------------------------------------------------------------------------
  // public methods
  //---------------------------------------------------------------------------
public:

  /*!@brief implementation of reading a simple variably type
   *
   * uses YARP BinPortable and BufferedPort
   */
  T read()
  {
    yarp::os::BinPortable<T> *pBinIn;

    pBinIn= BufferIn.read( block ); // strict mode!
    if (pBinIn !=0)
    {
      return pBinIn->content();
    }
    else // you never know ...
    {
      CEDAR_THROW( cedar::aux::exc::NetUnexpectedDataException,
                   "YARP: unexpectedly read NULL from port" );
      return 0;
    }
  }

};

} } } } // end namespaces

#endif

