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

    File:        AbstractNetBase.cpp

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Tue 19 Jul 2011

    Description: Shared code between the AbstractNetReader and -Writer.
                 Also, note that this class is not template-based, but
                 the classes extending it (AbstractNetWriter, AbstractNetReader)
                 are.

    Credits:

=============================================================================*/

#include "cedar/configuration.h"

#ifdef CEDAR_USE_YARP

// LOCAL INCLUDES
#include "cedar/auxiliaries/net/detail/transport/AbstractNetBase.h"
#include "cedar/auxiliaries/net/exceptions.h"
#include "cedar/auxiliaries/Log.h"

// PROJECT INCLUDES
#include <yarp/conf/version.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/NameClient.h>
#include <yarp/os/Contact.h>
#include <yarp/os/Network.h>


// SYSTEM INCLUDES
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>

namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {
//!@cond SKIPPED_DOCUMENTATION

// constants:
const std::string AbstractNetBase::PORT_PREFIX("/CEDAR"); // static
const std::string AbstractNetBase::PORT_DELIMINATOR("/"); // static  
const std::string AbstractNetBase::PORT_SUFFIX_OUT("out"); // static
const std::string AbstractNetBase::PORT_SUFFIX_IN("in"); // static


// static variables:
QMutex AbstractNetBase::mNetworkInitMutex;


//-----------------------------------------------------------------------------
// constructors and destructor
//-----------------------------------------------------------------------------

AbstractNetBase::AbstractNetBase(const std::string &myPortNameWithSuffix) 
          : mNetwork(), mFullPortName(), mIsConnected(false)
{
  QMutexLocker locker(&mNetworkInitMutex);
  mNetwork = boost::shared_ptr<yarp::os::Network>(new yarp::os::Network());
#ifdef DEBUG_NETT
  std::cout << "  AbstractNetBase [CONSTRUCTOR]" << endl;
#else
  // if we are not in DEBUG-mode
  mNetwork->setVerbosity(-1); // tells YARP to inhibit messages
#endif
  locker.unlock();
  mFullPortName= PORT_PREFIX + PORT_DELIMINATOR
                 + myPortNameWithSuffix;
}

AbstractNetBase::~AbstractNetBase()
{
#ifdef DEBUG_NETT
  std::cout << "  ~AbstractNetBase [DESTRUCTOR]" << endl;
#endif
}


//-----------------------------------------------------------------------------
// methods
//-----------------------------------------------------------------------------
std::string AbstractNetBase::getFullPortName()
{
  return mFullPortName;
}

void AbstractNetBase::lateConstruct()
{
#ifdef DEBUG_NETT
  std::cout << "  AbstractNetBase [lateConstruct()]" << endl;
#endif
  // only call from the "non abstract" constructor class,
  // i.e. the final implementation class (derived fromt this class) 
  // not from this class(!) as we want to call virtual functions
  // and this ist forbidden from inside a base-class constructor


  if (!open()) // open with the virtual function open() 
  {            // implemented in a derived class
      CEDAR_THROW( cedar::aux::net::NetMissingRessourceException,
                   "YARP could not open the port" );
  
  }
}

void AbstractNetBase::earlyDestruct()
{
#ifdef DEBUG_NETT
  std::cout << "  AbstractNetBase [earlyDestruct]" << endl;
#endif
  close(); // see lateConstruct()
           // dont use the returnvalue here and dont throw an exception
  mIsConnected= false;
}

// this connection may fail (without throwing an exception!)
bool AbstractNetBase::connectTwo(const std::string &writerPort, const std::string &readerPort)
{
#ifdef DEBUG_NETT
//  std::cout << "  AbstractNetBase [connectTwo]" << endl;
#endif

  if (mIsConnected)
    return false;

  // check for name server settings. might be helpful
  yarp::os::Contact contact = yarp::os::Network::getNameServerContact();
  if (contact.getHost() == yarp::os::ConstString("127.0.0.1"))
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "You misconfigured YARP. Cannot use yarp name server at IP 127.0.0.1!",
      "cedar::aux::net::AbstractNetBase::connectTwo(std::string,std::string)"
    );
  }

  if (!mNetwork->connect( writerPort.c_str(),
                         readerPort.c_str(),
                         0, //"mcast",
                         true ) ) // 4. Argument: quiet = true
  {
    mIsConnected= false;

    // we need to fail here without throwing an exception
    // exceptions will be thrown further up in the function stack
    
    return false;
  }

#ifdef DEBUG_NETT
  std::cout << "  connection OK" << endl;
#endif
  mIsConnected= true;
  return true;
}

// this connection may fail (without throwing an exception!)
bool AbstractNetBase::connectTo(const std::string &writerPort)
{
#ifdef DEBUG_NETT
  std::cout << "connect " <<  getFullPortName() << " to "  << writerPort  << endl;
#endif
  return connectTwo( writerPort,
                     getFullPortName() );

}

} } } } // end namespaces

//!@endcond

#endif // CEDAR_USE_YARP

