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

    File:        AbstractNetReader.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Tue 19 Jul 2011 04:46:11 PM CEST

    Description:

    Credits:

=============================================================================*/

#ifndef CEDAR_ABSTRACTNETREADER_H
#define CEDAR_ABSTRACTNETREADER_H

// LOCAL INCLUDES
#include "../namespace.h"
#include "AbstractNetBase.h"
#include "auxiliaries/net/exceptions/NetException.h"

// PROJECT INCLUDES
#include <boost/lexical_cast.hpp>
#include <yarp/os/Network.h>

// SYSTEM INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {

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
  AbstractNetReader(const AbstractNetReader &A); // not copyable
  AbstractNetReader &operator=(const AbstractNetReader &A);


public:
#define READER_PORT_NAME(x) ( (x) + PORT_DELIMINATOR  \
                                  + PORT_SUFFIX_IN + "(" \
                                  + boost::lexical_cast<std::string>(getReaderCounter()) \
                                  + ")" )
  //!@brief use this constructor. Parameter ist the user-defined port name
  explicit AbstractNetReader(const std::string &myPortName) 
                   : AbstractNetBase( READER_PORT_NAME(myPortName) ),
                     mPortNameWriter()
  {
#ifdef DEBUG
    cout << "  AbstractNetReader [CONSTRUCTOR]" << endl;
#endif

    // ersten freien Readerport im Netzwerk finden
    do
    {
      mPortNameWriter= PORT_PREFIX + PORT_DELIMINATOR
                       + myPortName
                       + PORT_DELIMINATOR
                       + PORT_SUFFIX_OUT;

      increaseReaderCounter();

    } while( !yarp::os::Network::exists( mPortNameWriter.c_str(),
                                         true ) // param: true = quiet
           );
  }

  //!@brief Destructor (virtual to be sure)
  virtual ~AbstractNetReader()
  {
#ifdef DEBUG
    cout << "  ~AbstractNetReader [DESTRUCTOR]" << endl;
#endif
  }

  //---------------------------------------------------------------------------
  // protected methods
  //---------------------------------------------------------------------------
protected:
  int getReaderCounter()
  {
    return mReaderCounter + 1;
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
  //!@brief (try) to connect to my writer. this operation may fail!
  //
  // returns TRUE for an established reconnection
  bool reconnect()
  {
    return connectTo(mPortNameWriter); 
  }

  //!@brief virtual read() needs to be implemented in child
  virtual T read() = 0;

};

template <class T>
int AbstractNetReader<T>::mReaderCounter; // static variable initialization

} } } } // end namespaces

#endif
