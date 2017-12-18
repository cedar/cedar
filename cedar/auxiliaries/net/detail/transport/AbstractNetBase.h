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

    File:        AbstractNetBase.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Mon 18 Jul 2011

    Description: Shared code between the AbstractNetReader and -Writer.
                 Also, note that this class is not template-based, but
                 the classes extending it (AbstractNetWriter, AbstractNetReader)
                 are.

    Credits:

=============================================================================*/

#ifndef CEDAR_ABSTRACT_NET_BASE_H
#define CEDAR_ABSTRACT_NET_BASE_H

#include "cedar/configuration.h"

#ifndef CEDAR_USE_YARP
// YARP was not compiled in!
// this may help to understand the error:
#error "YARP not compiled in but used in net::Writer/Reader/BlockingReader"
#else
// ... the rest of the file is if YARP was compiled in ...


// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/transport/AbstractNetBase.fwd.h"
#include "cedar/auxiliaries/net/detail/transport/interfaces/InterfaceOpenable.h"

// PROJECT INCLUDES
#include <yarp/os/Network.h>
#ifndef Q_MOC_RUN
  #include <boost/utility.hpp>
#endif
#include <QMutex>
#include <string>


// SYSTEM INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/types.h>

namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {

//!@cond SKIPPED_DOCUMENTATION
/*!@brief Shared functionality between AbstractNetWriter and Reader
 *
 * Basic inherit with shared functionality that Writer and Reader
 * will use (e.g. establishing connection with the network).
 *
 * this base class and all derived classes are not copyable.
 */
class AbstractNetBase : virtual protected InterfaceOpenable,
                          // virtual inheritance for CollatedNetBase/Writer/Reader
                        boost::noncopyable
                          // not copyable, because of RAII
{
  //---------------------------------------------------------------------------
  // members
  //---------------------------------------------------------------------------
protected:
  static QMutex mNetworkInitMutex;

  boost::shared_ptr<yarp::os::Network> mNetwork; // not static and not a singleton(!)
                              // (because that doesnt work with YARP)
                              // this is the YARP-documented way to use Network

private:
  std::string mFullPortName; 
    //the full (user-defines plus hardcoded suffixes) port name:
  bool   mIsConnected; // to speed up valid-network checks

protected:
  // these constants will be added to the user-defined port name
  static const std::string PORT_PREFIX;
  static const std::string PORT_DELIMINATOR;
  static const std::string PORT_SUFFIX_OUT;
  static const std::string PORT_SUFFIX_IN;

  //---------------------------------------------------------------------------
  // parameters
  //---------------------------------------------------------------------------
protected:
private:

  //---------------------------------------------------------------------------
  // constructors and destructor
  //---------------------------------------------------------------------------
public:
  //!@brief use this constructor with this port name 
  explicit AbstractNetBase(const std::string &myPortNameWithSuffix);
  //!@brief virtual destructor just to be sure
  virtual ~AbstractNetBase();
private:
  //!@brief dont use the standard constructor
  AbstractNetBase();
  // AbstractNetBase(const AbstractNetBase &A); // not copyable, see boost::noncopyable
  // AbstractNetBase &operator=(const AbstractNetBase &A); // not copyable, see boost::noncopyable




  //---------------------------------------------------------------------------
  // public methods
  //---------------------------------------------------------------------------
public:


  //---------------------------------------------------------------------------
  // protected methods
  //---------------------------------------------------------------------------
protected:
  //!@brief static counter (e.g. of Reader instances)
  int getReaderCounter();
  //!@brief increase the static counter
  void increaseReaderCounter();
  //!@brief the (full) name of the used YARP port. (!= user defined name)
  std::string getFullPortName();
  //!@brief will be called outside of constructor (C++ doesnt allow the
  //        use of virtual functions inside a constructor)
  void lateConstruct();
  //!@brief will be called outside of destructor. see lateConstruct()
  void earlyDestruct();
  //!@brief connection established and valid as far as we know
  bool isConnected();
  //!@brief helper function to connect to ports
  bool connectTwo(const std::string &writerPort, const std::string &readerPort);
  //!@brief helper function to connect a Reader to its Writer
  bool connectTo(const std::string &writerPort);

  //---------------------------------------------------------------------------
  // private methods
  //---------------------------------------------------------------------------
private:

}; // class end

// inline, as we will call this function for every read
inline bool AbstractNetBase::isConnected()
{
  return mIsConnected;
}

//!@endcond  
} } } } // end namespaces

#endif // CEDAR_USE_YARP
#endif // CEDAR_XXX_XXX_H

