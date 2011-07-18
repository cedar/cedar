#ifndef _NETT_ABSTRACT_NET_WRITER_
#define _NETT_ABSTRACT_NET_WRITER_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "../namespace.h"
#include "AbstractNetBase.h"

using namespace std;
using namespace yarp::os;

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {

template <typename T>
class AbstractNetWriter : public AbstractNetBase
{
private:

  void check_for_nameserver()
  {
    if (!AbstractNetBase::network.checkNetwork())
    {
#ifdef DEBUG
      cout << "no YARP nameserver found - auto running [WRITER] TODO ..." << endl;
#endif
    }
  }

protected:
  void late_construct()
  {
    AbstractNetBase::late_construct();
  }

  void late_destruct()
  {
    AbstractNetBase::late_destruct();
  }

#define WRITER_PORT_NAME(x) ( (x) + PORT_DELIMINATOR + PORT_SUFFIX_OUT )
public:
  AbstractNetWriter(string myPortName)  
                   : AbstractNetBase( WRITER_PORT_NAME(myPortName) )
  {
#ifdef DEBUG
  cout << "  AbstractNetWriter [CONSTRUCTOR]" << endl;
#endif
  }

  ~AbstractNetWriter()
  {
#ifdef DEBUG
  cout << "  ~AbstractNetWriter [DESTRUCTOR]" << endl;
#endif
  }

  virtual void write(T t) = 0;
};

} } } }  // end namespaces

#endif
