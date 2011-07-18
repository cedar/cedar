#ifndef _NETT_ABSTRACT_NET_BASE_
#define _NETT_ABSTRACT_NET_BASE_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <boost/utility.hpp>

#include "../namespace.h"
#include "interfaces/InterfaceOpenable.h"
#include <yarp/os/Network.h>

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {

static const std::string PORT_PREFIX("/CEDAR");
static const std::string PORT_DELIMINATOR("/");
static const std::string PORT_SUFFIX_OUT("out");
static const std::string PORT_SUFFIX_IN("in");

class AbstractNetBase : virtual protected InterfaceOpenable,
                        boost::noncopyable
{
protected:
//  static 
  yarp::os::Network network;
private:
  static int mInstanceCounter;
  std::string mFullPortName;
  bool   mIsConnected;



public:
  AbstractNetBase(std::string myPortNameWithSuffix);
  virtual ~AbstractNetBase();
protected:
  int getInstanceCounter();
  std::string getFullPortName();
  void late_construct();
  void late_destruct();
  bool isConnected();

protected:
  bool connectTwo(const std::string &writerPort, const std::string &readerPort);
  bool connectTo(const std::string &writerPort);

private:
  bool startNameServer();

};



} } } } // end namespaces

#endif
