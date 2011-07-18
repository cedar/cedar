#include "AbstractNetBase.h"
#include "auxiliaries/net/exceptions/NetException.h"

#include <yarp/conf/version.h>

#include <unistd.h>
#include <sys/types.h>

using namespace std;
using namespace yarp::os;

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {

//Network AbstractNetBase::network; // static
int AbstractNetBase::mInstanceCounter; // static

AbstractNetBase::AbstractNetBase(string myPortNameWithSuffix) : network(), mFullPortName(), mIsConnected(false)
{
#ifdef DEBUG
  cout << "  AbstractNetBase [CONSTRUCTOR]" << endl;
#endif
  mFullPortName= PORT_PREFIX + PORT_DELIMINATOR
                 + myPortNameWithSuffix;

  mInstanceCounter++;
}

AbstractNetBase::~AbstractNetBase()
{
  mInstanceCounter--;
#ifdef DEBUG
cout << "  ~AbstractNetBase [DESTRUCTOR]" << endl;
#endif
}

int AbstractNetBase::getInstanceCounter()
{
  return mInstanceCounter + 1;
}

string AbstractNetBase::getFullPortName()
{
  return mFullPortName;
}

bool AbstractNetBase::startNameServer()
{
  pid_t pid;

  pid= fork(); // NEW PROCESS!

  if ( pid < 0 )
  {
    // ERROR
    return false;
  }
  else if (pid == 0)
  {
    // FORKED CHILD ...
    // this will block! and create a zombie process. this is intended.
    if ( yarp::os::Network::runNameServer(0, 0) != 0 )
    {
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

    // PARENT ...
    return yarp::os::NetworkBase::checkNetwork();
  }

}

void AbstractNetBase::late_construct()
{
#ifdef DEBUG
  cout << "  AbstractNetBase [laste_construct()]" << endl;
#endif
  // only call from the "non abstract" constructor class,
  // i.e. the final implementation class (derived fromt this class) 
  // not from this class(!) as we want to call virtual functions
  // and this ist forbidden from inside a base-class constructor

  // look for existing name server
  bool found= true;

  found= yarp::os::NetworkBase::checkNetwork();

  if (!found)
  {
    // the first NetReader or NetWriter to be initialized will
    // start a new nameserver (if none was running)

    //  YARP_VERSION >= "2.3.3"
#if 1
    if (!startNameServer())  
#endif      
    {
      CEDAR_THROW( cedar::aux::exc::NetMissingRessourceException,
                   "YARP: no yarp name server found and cannot auto "
                          "start one" );

    }
  }

  open(); // open with the virtual function open() 
          // implemented in a derived class
}

void AbstractNetBase::late_destruct()
{
#ifdef DEBUG
  cout << "  AbstractNetBase [late_destruct]" << endl;
#endif
  close(); // see late_construct()
  mIsConnected= false;
#ifdef DEBUG
  cout << "  AbstractNetBase [... exiting late_destruct]" << endl;
#endif
}

bool AbstractNetBase::connectTwo(const string &writerPort, const string &readerPort)
{
#ifdef DEBUG
  cout << "  AbstractNetBase [connectTwo]" << endl;
#endif

  if (mIsConnected)
    return false;

  if (!network.connect( writerPort.c_str(),
                   readerPort.c_str(),
                   0,
                   true ) ) // 4. Argument: quiet = true
  {
    mIsConnected= false;

    // dont throw the exception here
    // CEDAR_THROW( cedar::aux::exc::NetMissingRessourceException,
    //              "YARP: cannot connect ports / no yarp server running?" );
    
    return false;
  }

  mIsConnected= true;
  return true;
}

bool AbstractNetBase::connectTo(const string &writerPort)
{
#ifdef DEBUG
cout << "connect " <<  getFullPortName() << " to "  << writerPort  << endl;
#endif
  return connectTwo( writerPort,
                     getFullPortName() );

}

bool AbstractNetBase::isConnected()
{
  return mIsConnected;
}


} } } } // end namespaces

