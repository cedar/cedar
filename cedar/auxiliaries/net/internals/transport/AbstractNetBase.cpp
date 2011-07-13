#include "AbstractNetBase.h"
#include "auxiliaries/net/exceptions/NetException.h"

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {

//Network AbstractNetBase::network; // static
int AbstractNetBase::mInstanceCounter; // static

AbstractNetBase::AbstractNetBase(string myPortNameWithSuffix) : network(), mFullPortName(), mIsConnected(false)
{
#ifdef DEBUG
//  cout << "  AbstractNetBase [CONSTRUCTOR]" << endl;
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

void AbstractNetBase::late_construct()
{
  // only call from the "non abstract" constructor class,
  // i.e. the final implementation class (derived fromt this class) 
  // not from this class(!) as we want to call virtual functions
  // and this ist forbidden from inside a base-class constructor

  open(); // open with the virtual function open() 
          // implemented in a derived class
}

void AbstractNetBase::late_destruct()
{
  close(); // see late_construct()
}

bool AbstractNetBase::connectTwo(const string &writerPort, const string &readerPort)
{
  if (!network.connect( writerPort.c_str(),
                   readerPort.c_str(),
                   0,
                   true ) ) // 4. Argument: quiet = true
  {
//    CEDAR_THROW( cedar::aux::exc::NetMissingRessourceException,
//                 "YARP: cannot connect ports / no yarp server running?" );

    mIsConnected= false;
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

