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

    File:        AbstractNetBase.cpp

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        Tue 19 Jul 2011 02:57:06 PM CEST

    Description: Shared code between the AbstractNetReader and -Writer.
                 Also, note that this class is not template-based, but
                 the classes extending it (AbstractNetWriter, AbstractNetReader)
                 are.

    Credits:

=============================================================================*/

// LOCAL INCLUDES
#include "AbstractNetBase.h"
#include "auxiliaries/net/exceptions/NetException.h"

// PROJECT INCLUDES
#include <yarp/conf/version.h>
#include <yarp/os/impl/NameConfig.h>
#include <yarp/os/impl/NameClient.h>


// SYSTEM INCLUDES
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>

using namespace std;
using namespace yarp::os;

namespace cedar {
  namespace aux {
    namespace net {
      namespace detail {

// constants:
const std::string AbstractNetBase::PORT_PREFIX("/CEDAR"); // static
const std::string AbstractNetBase::PORT_DELIMINATOR("/"); // static  
const std::string AbstractNetBase::PORT_SUFFIX_OUT("out"); // static
const std::string AbstractNetBase::PORT_SUFFIX_IN("in"); // static

// static variables:

// YARP-bug. dont be static: Network AbstractNetBase::mNetwork; // static


//-----------------------------------------------------------------------------
// constructors and destructor
//-----------------------------------------------------------------------------

AbstractNetBase::AbstractNetBase(const std::string &myPortNameWithSuffix) 
          : mNetwork(), mFullPortName(), mIsConnected(false)
{
#ifdef DEBUG_NETT
  cout << "  AbstractNetBase [CONSTRUCTOR]" << endl;
#else
  // if we are not in DEBUG-mode
  mNetwork.setVerbosity(-1); // tells YARP to inhibit messages
#endif
  mFullPortName= PORT_PREFIX + PORT_DELIMINATOR
                 + myPortNameWithSuffix;
}

AbstractNetBase::~AbstractNetBase()
{
#ifdef DEBUG_NETT
  cout << "  ~AbstractNetBase [DESTRUCTOR]" << endl;
#endif
  // we need to kill the child-process that runs an (automatically
  // started) YARP name server.
  // else processes, that wait() for all children of this program
  // will never resume
  if (mServerPID)
  {
    kill( mServerPID, SIGKILL );
#ifdef DEBUG_NETT
  cout << "  killed name server with pid " << mServerPID << endl;
#endif  
  }
}


//-----------------------------------------------------------------------------
// methods
//-----------------------------------------------------------------------------
string AbstractNetBase::getFullPortName()
{
  return mFullPortName;
}

bool AbstractNetBase::startNameServer()
{
  pid_t pid;

  pid= vfork(); // NEW PROCESS!

  // (1) clean YARPs local "nameserver config file", which effectively
  // serves as cache and just makes problems for us, when the nameserver
  // changes. see (2)
  yarp::os::impl::NameConfig nc;
  nc.toFile(true); // clean configfile
 
  if ( pid < 0 )
  {
    // ERROR
    return false;
  }
  else if (pid == 0)
  {
    // FORKED CHILD ...
    // this will block!  
    //if ( yarp::os::Network::runNameServer(0, 0) != 0 ) // needs YARP v2.3.6
    //system("yarp server"); // starts command in a shell

#ifdef DEBUG_NETT
    cout << "  executing yarp server" << endl;
#endif

#if 1
    // swallow the output of the following yarp server command
    // (this redirect stdout of this process to /dev/null)
    int fh;
    ::fflush(stdout);
    fh = ::open("/dev/null", O_WRONLY);
    ::dup2(fh, 1);
    ::close(fh);
#endif

    if (execlp("yarp", "yarp", "server", NULL) == -1)
    {
      // this works with all yarp versions
      // this does not create a zombie process. better!

#ifdef DEBUG_NETT
      cout << "  could not start yarp server!" << endl;
#endif

    }
    exit(EXIT_SUCCESS);
  }
  else
  {
    sleep(1); // ugly, but no better idea for now. we need to wait for the
              // child process to start the name server.
              // (fork doesnt guarantee the order of execution
              //  between child and parent)
              // it works okay, as we will only land here ONCE.

    // (2) Yarp: this seems to be necessary so that we dont fail
    //       the first time we look for the nameserver. see (1)
    yarp::os::impl::NameClient& client = 
               yarp::os::impl::NameClient::getNameClient();
    client.updateAddress();

    mServerPID= pid; // remember child PID, see Destructor
    // PARENT ...
    return yarp::os::NetworkBase::checkNetwork();
  }

}

bool AbstractNetBase::checkNameServer()
{
  // look for existing name server
  bool found= true;

  found= yarp::os::NetworkBase::checkNetwork();

  if (!found)
  {
    // check if requested

    if (!startNameServer())  
    {
      CEDAR_THROW( cedar::aux::exc::NetMissingRessourceException,
                   "YARP: no yarp name server found and cannot auto "
                          "start one" );
    }
  }
  return true;
}

void AbstractNetBase::lateConstruct()
{
#ifdef DEBUG_NETT
  cout << "  AbstractNetBase [lateConstruct()]" << endl;
#endif
  // only call from the "non abstract" constructor class,
  // i.e. the final implementation class (derived fromt this class) 
  // not from this class(!) as we want to call virtual functions
  // and this ist forbidden from inside a base-class constructor


  if (!open()) // open with the virtual function open() 
  {            // implemented in a derived class
      CEDAR_THROW( cedar::aux::exc::NetMissingRessourceException,
                   "YARP could not open the port" );
  
  }
}

void AbstractNetBase::lateDestruct()
{
#ifdef DEBUG_NETT
  cout << "  AbstractNetBase [lateDestruct]" << endl;
#endif
  close(); // see lateConstruct()
           // dont use the returnvalue here and dont throw an exception
  mIsConnected= false;
}

// this connection may fail (without throwing an exception!)
bool AbstractNetBase::connectTwo(const std::string &writerPort, const std::string &readerPort)
{
#ifdef DEBUG_NETT
//  cout << "  AbstractNetBase [connectTwo]" << endl;
#endif

  if (mIsConnected)
    return false;

  if (!mNetwork.connect( writerPort.c_str(),
                         readerPort.c_str(),
                         0,
                         true ) ) // 4. Argument: quiet = true
  {
    mIsConnected= false;

    // we need to fail here without throwing an exception
    // exceptions will be thrown further up in the function stack
    
    return false;
  }

#ifdef DEBUG_NETT
  cout << "  connection OK" << endl;
#endif
  mIsConnected= true;
  return true;
}

// this connection may fail (without throwing an exception!)
bool AbstractNetBase::connectTo(const std::string &writerPort)
{
#ifdef DEBUG_NETT
  cout << "connect " <<  getFullPortName() << " to "  << writerPort  << endl;
#endif
  return connectTwo( writerPort,
                     getFullPortName() );

}

} } } } // end namespaces

