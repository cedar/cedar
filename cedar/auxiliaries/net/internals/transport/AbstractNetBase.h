#ifndef _NETT_ABSTRACT_NET_BASE_
#define _NETT_ABSTRACT_NET_BASE_
#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <boost/utility.hpp>

#include "../namespace.h"
#include "interfaces/InterfaceOpenable.h"
#include <yarp/os/Network.h>

using namespace std;
using namespace yarp::os;

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {

static const string PORT_PREFIX("/CEDAR");
static const string PORT_DELIMINATOR("/");
static const string PORT_SUFFIX_OUT("out");
static const string PORT_SUFFIX_IN("in");

class AbstractNetBase : virtual protected InterfaceOpenable,
                        boost::noncopyable
{
protected:
//  static 
  Network network;
private:
  static int mInstanceCounter;
  string mFullPortName;
  bool   mIsConnected;


public:
  AbstractNetBase(string myPortNameWithSuffix);
  virtual ~AbstractNetBase();
protected:
  int getInstanceCounter();
  string getFullPortName();
  void late_construct();
  void late_destruct();
  bool isConnected();

protected:
  bool connectTwo(const string &writerPort, const string &readerPort);
  bool connectTo(const string &writerPort);

};


} } } } // end namespaces

#endif
