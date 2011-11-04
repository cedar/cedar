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

    File:        SimpleNetWriter.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Tue 19 Jul 2011 05:36:10 PM CEST

    Description: To transport a primitiv type (int, short, etc.) we
                 rely on yarp::os::BinPortable.
                 This class implements the user-accessible write() function.

    Credits:

=============================================================================*/

#ifndef CEDAR_SIMPLENETWRITER_H
#define CEDAR_SIMPLENETWRITER_H

// LOCAL INCLUDES
#include "../../namespace.h"
#include "../AbstractNetWriter.h"

// PROJECT INCLUDES
#include <yarp/os/BufferedPort.h>
#include <yarp/os/BinPortable.h>

// SYSTEM INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;
using namespace yarp::os;

namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {


/*!@brief implements writing (RIAA over network) simple variable types
 *
 * use in NetWriter
 */
template <typename T>
class SimpleNetWriter : public AbstractNetWriter<T>
{
  //---------------------------------------------------------------------------
  // members
  //---------------------------------------------------------------------------
private:
  BufferedPort< BinPortable<T> > BufferOut;

  //---------------------------------------------------------------------------
  // constructors and destructor
  //---------------------------------------------------------------------------
private:
  //!@brief The standard constructor. dont use
  SimpleNetWriter();
  SimpleNetWriter(const SimpleNetWriter &S); // not copyable
  SimpleNetWriter &operator=(const SimpleNetWriter &S);

public:
  explicit SimpleNetWriter(const string &myPortName) 
                                     : AbstractNetWriter<T>(myPortName),
                                       BufferOut()
  {
#ifdef DEBUG_NETT
    cout << "  SimpleNetWriter [CONSTRUCTOR]" << endl;
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
    AbstractNetWriter<T>::lateDestruct();
#ifdef DEBUG_NETT
    cout << "  ~SimpleNetWriter [DESTRUCTOR]" << endl;
#endif
  }


  //---------------------------------------------------------------------------
  // protected methods
  //---------------------------------------------------------------------------
protected:
  //!@brief open the port. called by AbstractNetBase
  bool open()
  {
    return BufferOut.open( AbstractNetBase::getFullPortName().c_str() ); 
  }

  //!@brief close the port. called by AbstractNetBase
  bool close()
  {
    if (!BufferOut.isClosed())
      BufferOut.close(); // is void

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
    BinPortable<T> &BinOut= BufferOut.prepare();

    BinOut.content()= t;
    BufferOut.write();  
  }

};

} } } } // end namespaces


#endif
