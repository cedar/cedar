#ifndef _NETT_ABSTRACT_NET_READER_
#define _NETT_ABSTRACT_NET_READER_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "../namespace.h"
#include "AbstractNetBase.h"
#include "auxiliaries/net/exceptions/NetException.h"
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace yarp::os;

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {

template <typename T>
class AbstractNetReader : public AbstractNetBase
{
private:
  string mPortNameWriter;

public:
  bool reconnect()
  {
    return connectTo(mPortNameWriter); 
  }

protected:
  void late_construct() // see AbstractNetBase
  {
    AbstractNetBase::late_construct();

#if 0
    // start by trying to connect to a existing writer
    try
    {
      reconnect();
    }
    catch ( cedar::aux::exc::NetMissingRessourceException E )
//    catch ( cedar::aux::exc::NetWaitingForWriterException &E )
    {
      // do nothin
    }
#endif
    reconnect();
  }

  void late_destruct() // see AbstractNetBase
  {
    AbstractNetBase::late_destruct();
  }

#define READER_PORT_NAME(x) ( (x) + PORT_DELIMINATOR  \
                                  + PORT_SUFFIX_IN + "(" \
                                  + boost::lexical_cast<std::string>(getInstanceCounter()) \
                                  + ")" )
public:
  AbstractNetReader(string myPortName) 
                   : AbstractNetBase( READER_PORT_NAME(myPortName) ),
                     mPortNameWriter()
  {
#ifdef DEBUG
  cout << "  AbstractNetReader [CONSTRUCTOR]" << endl;
#endif
    mPortNameWriter= PORT_PREFIX + PORT_DELIMINATOR
                       + myPortName
                       + PORT_DELIMINATOR
                       + PORT_SUFFIX_OUT;
  }

  ~AbstractNetReader()
  {
#ifdef DEBUG
  cout << "  ~AbstractNetReader [DESTRUCTOR]" << endl;
#endif
  }

  virtual T read() = 0;

protected:
  string getPortNameWriter()
  {
    return mPortNameWriter;
  }
};

} } } } // end namespaces

#endif
