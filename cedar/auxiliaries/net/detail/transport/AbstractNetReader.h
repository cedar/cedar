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

    File:        AbstractNetReader.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Tue 19 Jul 2011

    Description: Part of the functionality a Reader class should have.
                 What is missing is the datatype (simple/primitiv vs
                 collated/matrix) dependant functionality.
                 We are mainly concerned with initiating connections, here.
                 This class inherits from AbstractNetReader and will be
                 extended in either SimpleNetReader or CollatedNetReader,
                 adding the datatype-dependant operations.


    Credits:

=============================================================================*/

#ifndef CEDAR_ABSTRACT_NET_READER_H
#define CEDAR_ABSTRACT_NET_READER_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/transport/AbstractNetBase.h"
#include "cedar/auxiliaries/net/detail/transport/AbstractNetReader.fwd.h"
#include "cedar/auxiliaries/net/exceptions.h"

// PROJECT INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/lexical_cast.hpp>
#endif
#include <yarp/os/Network.h>

// SYSTEM INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {

//!@cond SKIPPED_DOCUMENTATION
/*!@brief Abstract Reader. function read() is virtual, handles basic connection
 *
 * Abstract Reader functionality (read datatype over network, RIIA)
 * Most of AbstractNetReaders functionality resides in AbstractNetBase.
 * read() is virtual and needs to be implemented in the child.
 */
template <typename T>
class AbstractNetReader : public AbstractNetBase
{
  //---------------------------------------------------------------------------
  // members
  //---------------------------------------------------------------------------
private:
  std::string mPortNameWriter;
  static int mReaderCounter; 


  //---------------------------------------------------------------------------
  // constructors and destructor
  //---------------------------------------------------------------------------

private:
  //!@brief The standard constructor. Dont use!
  AbstractNetReader();
  // AbstractNetReader(const AbstractNetReader &A); // parent is not copyable
  // AbstractNetReader &operator=(const AbstractNetReader &A); // parent is not copyable


public:
#define CEDAR_NETT_READER_PORT_NAME(x) ( (x) + PORT_DELIMINATOR  \
                                  + PORT_SUFFIX_IN + "(" \
                                  + boost::lexical_cast<std::string>(getReaderCounter()) \
                                  + ")" )
  //!@brief use this constructor. Parameter ist the user-defined port name
  explicit AbstractNetReader(const std::string &myPortName) 
                   : AbstractNetBase( CEDAR_NETT_READER_PORT_NAME(myPortName) ),
                     mPortNameWriter()
  {
#ifdef DEBUG_NETT
    cout << "  AbstractNetReader [CONSTRUCTOR]" << endl;
#endif

    // look for first unused reader port name in the network
    mPortNameWriter= PORT_PREFIX + PORT_DELIMINATOR
                       + myPortName
                       + PORT_DELIMINATOR
                       + PORT_SUFFIX_OUT;

    increaseReaderCounter();
    
    if( !yarp::os::Network::exists( mPortNameWriter.c_str(),
                                         true ) // param: true == quiet
           )
    {
      CEDAR_THROW( cedar::aux::net::NetWaitingForWriterException,
                   "YARP: no writer yet (at intantiation point of reader)" );
    }
  }

  //!@brief Destructor (virtual to be sure)
  virtual ~AbstractNetReader()
  {
#ifdef DEBUG_NETT
    cout << "  ~AbstractNetReader [DESTRUCTOR]" << endl;
#endif
  }

  //---------------------------------------------------------------------------
  // protected methods
  //---------------------------------------------------------------------------
protected:
  int getReaderCounter()
  {
    return mReaderCounter + 1; // start with 1
  }

  void increaseReaderCounter()
  {
    mReaderCounter++;
  }

  //!@brief the (fully configured) name of the writer port for this reader
  std::string getPortNameWriter()
  {
    return mPortNameWriter;
  }

  void lateConstruct() // see AbstractNetBase
  {
    AbstractNetBase::lateConstruct();
 
    reconnect(); // try connect at start to my writer
                 // if not possible yet (no writer yet) this has no 
                 // consequences.
                 // We will try again in read()
  }


  //---------------------------------------------------------------------------
  // public methods
  //---------------------------------------------------------------------------
public:
  //!@brief (try) to connect to my writer. this operation may silently fail!
  //
  // returns TRUE for an established reconnection
  // does not throw an exception
  bool reconnect()
  {
    return connectTo(mPortNameWriter); 
  }

  //!@brief virtual read() needs to be implemented in child
  virtual T read() = 0;

};

template <class T>
int AbstractNetReader<T>::mReaderCounter; // static variable initialization

//!@endcond

} } } } // end namespaces

#endif
