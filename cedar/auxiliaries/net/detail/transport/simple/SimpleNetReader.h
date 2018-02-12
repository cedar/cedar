/*============================================================================

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

    File:        SimpleNetReader.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Tue 19 Jul 2011 

    Description: To transport a primitive type (int, short, etc.) we
                 rely on yarp::os::BinPortable.
                 This class implements the user-accessible read() function.

    Credits:

=============================================================================*/

#ifndef CEDAR_SIMPLE_NET_READER_H
#define CEDAR_SIMPLE_NET_READER_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/transport/simple/SimpleNetReader.fwd.h"
#include "cedar/auxiliaries/net/detail/transport/AbstractNetReader.h"
#include "cedar/auxiliaries/net/exceptions.h"

// PROJECT INCLUDES
#include <yarp/os/BufferedPort.h>
#include <yarp/os/BinPortable.h>

// SYSTEM INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

//!@cond SKIPPED_DOCUMENTATION
/*!@brief implements reading (RIAA over network) simple variable types
 *
 * use in Reader
 */
template <typename T, bool BLOCK= false>
class cedar::aux::net::detail::SimpleNetReader : public AbstractNetReader<T>
{
  //---------------------------------------------------------------------------
  // members
  //---------------------------------------------------------------------------
private:
  yarp::os::BufferedPort< yarp::os::BinPortable<T> > mBufferIn;

  //---------------------------------------------------------------------------
  // constructors and destructor
  //---------------------------------------------------------------------------

private:
  //!@brief The standard constructor. dont use
  SimpleNetReader();
  // SimpleNetReader(const SimpleNetReader &S); // parent is not copyable
  // SimpleNetReader &operator=(const SimpleNetReader &S); // parent is not copyable

public:
  explicit SimpleNetReader(const std::string &myPortName) 
                                     : AbstractNetReader<T>(myPortName),
                                       mBufferIn()
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
    AbstractNetReader<T>::earlyDestruct();
#ifdef DEBUG_NETT
    cout << "  ~SimpleNetReader [DESTRUCTOR]" << endl;
#endif
  }


  //---------------------------------------------------------------------------
  // protected methods
  //---------------------------------------------------------------------------
protected:
  //!@brief open the port. called by AbstractNetBase
  bool open()
  {
    return mBufferIn.open( AbstractNetBase::getFullPortName().c_str() ); 
  }

  //!@brief close the port. called by AbstractNetBase
  bool close()
  {
    if (!mBufferIn.isClosed())
      mBufferIn.close(); // is void

    return true;
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
    yarp::os::BinPortable<T> *p_bin_in;

    p_bin_in= mBufferIn.read( BLOCK ); // strict mode!
    if (p_bin_in != NULL)
    {
      return p_bin_in->content();
    }
    else // you never know ...
    {
      CEDAR_THROW( cedar::aux::net::NetNoNewDataException,
                   "YARP: currently no data in buffer (reading)" );
      return 0;
    }
  }

};
//!@endcond

#endif
