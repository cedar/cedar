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

    File:        AbstractNetWriter.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Tue 19 Jul 2011

    Description: Part of the functionality a Writer class should have.
                 What is missing is the datatype (simple/primitiv vs
                 collated/matrix)-dependant functionality.
                 We are mainly concerned with initiating connections, here.
                 This class inherits from AbstractNetWriter and will be
                 extended in either SimpleNetWriter or CollatedNetWriter,
                 adding the datatype-dependant operations.

    Credits:

=============================================================================*/

#ifndef CEDAR_ABSTRACT_NET_WRITER_H
#define CEDAR_ABSTRACT_NET_WRITER_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/transport/AbstractNetWriter.fwd.h"
#include "cedar/auxiliaries/net/detail/transport/AbstractNetBase.h"

// PROJECT INCLUDES

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
/*!@brief Abstract Writer. function write() is virtual, handles basic connection
 *
 * Abstract Writer functionality (write datatype over network, RIIA)
 * Most of AbstractNetWriters functionality resides in AbstractNetBase.
 * write() is virtual and needs to be implemented in the child.
 */
template <typename T>
class AbstractNetWriter : public AbstractNetBase
{

  //---------------------------------------------------------------------------
  // constructors and destructor
  //---------------------------------------------------------------------------
private:
  //!@brief The standard constructor. Dont use
  AbstractNetWriter();
  // AbstractNetWriter(const AbstractNetWriter &A); // parent is not copyable
  // AbstractNetWriter &operator=(const AbstractNetWriter &A); // parent is not copyable



public:
#define CEDAR_NETT_WRITER_PORT_NAME(x) ( (x) + PORT_DELIMINATOR + PORT_SUFFIX_OUT )
  //!@brief Use this constructor. Pass the user-defined port name as argument.
  //        The effective port name will be different.
  explicit AbstractNetWriter(const std::string &myPortName)  
                   : AbstractNetBase( CEDAR_NETT_WRITER_PORT_NAME(myPortName) )
  {
#ifdef DEBUG_NETT
    cout << "  AbstractNetWriter [CONSTRUCTOR]" << endl;
#endif
  }

  //!@brief Destructor virtual (to be sure)
  virtual ~AbstractNetWriter()
  {
#ifdef DEBUG_NETT
    cout << "  ~AbstractNetWriter [DESTRUCTOR]" << endl;
#endif
  }

  //---------------------------------------------------------------------------
  // protected methods
  //---------------------------------------------------------------------------
protected:


  //---------------------------------------------------------------------------
  // private methods
  //---------------------------------------------------------------------------
private:


  //---------------------------------------------------------------------------
  // public methods
  //---------------------------------------------------------------------------
public:

  //!@brief virtual write() needs to be implemented in child
  virtual void write(const T &t) = 0;
};
//!@endcond

} } } }  // end namespaces

#endif
