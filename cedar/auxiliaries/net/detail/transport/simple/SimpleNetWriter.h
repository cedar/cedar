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

    File:        SimpleNetWriter.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Tue 19 Jul 2011

    Description: To transport a primitiv type (int, short, etc.) we
                 rely on yarp::os::BinPortable.
                 This class implements the user-accessible write() function.

    Credits:

=============================================================================*/

#ifndef CEDAR_SIMPLE_NET_WRITER_H
#define CEDAR_SIMPLE_NET_WRITER_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/transport/simple/SimpleNetWriter.fwd.h"
#include "cedar/auxiliaries/net/detail/transport/AbstractNetWriter.h"

// PROJECT INCLUDES
#include <yarp/os/BufferedPort.h>
#include <yarp/os/BinPortable.h>

// SYSTEM INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

//!@cond SKIPPED_DOCUMENTATION
/*!@brief implements writing (RIAA over network) simple variable types
 *
 * use in Writer
 */
template <typename T>
class cedar::aux::net::detail::SimpleNetWriter : public AbstractNetWriter<T>
{
  //---------------------------------------------------------------------------
  // members
  //---------------------------------------------------------------------------
private:
  yarp::os::BufferedPort< yarp::os::BinPortable<T> > mBufferOut;

  //---------------------------------------------------------------------------
  // constructors and destructor
  //---------------------------------------------------------------------------
private:
  //!@brief The standard constructor. dont use
  SimpleNetWriter();
  SimpleNetWriter(const SimpleNetWriter &S); // not copyable
  SimpleNetWriter &operator=(const SimpleNetWriter &S);

public:
  explicit SimpleNetWriter(const std::string &myPortName) 
                                     : AbstractNetWriter<T>(myPortName),
                                       mBufferOut()
  {
#ifdef DEBUG_NETT
    std::cout << "  SimpleNetWriter [CONSTRUCTOR]" << endl;
#endif
    AbstractNetWriter<T>::lateConstruct();
      // explanation: lateConstruct() needs to be called outside of
      //              AbstractNetBases/Writers constructors, so we
      //              can access virtual functions from there
  }

  // no need for virtual here, as pointers of this will never be passed around
  //!@brief Destructor
  ~SimpleNetWriter()
  {
    AbstractNetWriter<T>::earlyDestruct();
#ifdef DEBUG_NETT
    std::cout << "  ~SimpleNetWriter [DESTRUCTOR]" << endl;
#endif
  }


  //---------------------------------------------------------------------------
  // protected methods
  //---------------------------------------------------------------------------
protected:
  //!@brief open the port. called by AbstractNetBase
  bool open()
  {
    return mBufferOut.open( AbstractNetBase::getFullPortName().c_str() ); 
  }

  //!@brief close the port. called by AbstractNetBase
  bool close()
  {
    if (!mBufferOut.isClosed())
      mBufferOut.close(); // is void

    return true;
  }
 
  //---------------------------------------------------------------------------
  // public methods
  //---------------------------------------------------------------------------
public:

  /*!@brief implementation of reading a writing variably type
   *
   * uses YARP BinPortable and BufferedPort
   */
  void write(const T &t) // may drop packets (non blocking)
  {
    yarp::os::BinPortable<T> &r_bin_out= mBufferOut.prepare();

    r_bin_out.content()= t;
    mBufferOut.write();  
  }

};
//!@endcond


#endif
