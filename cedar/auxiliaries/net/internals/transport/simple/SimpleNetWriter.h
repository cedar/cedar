#ifndef _NETT_SIMPLE_NET_WRITER_
#define _NETT_SIMPLE_NET_WRITER_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "../../namespace.h"
#include "../AbstractNetWriter.h"
#include <yarp/os/BufferedPort.h>
#include <yarp/os/BinPortable.h>

using namespace std;
using namespace yarp::os;

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {


template <typename T>
class SimpleNetWriter : public AbstractNetWriter<T>
{
private:
  BufferedPort< BinPortable<T> > BufferOut;

protected:
  void open()
  {
    BufferOut.open( AbstractNetBase::getFullPortName().c_str() ); 
  }

  void close()
  {
    if (!BufferOut.isClosed())
      BufferOut.close();
  }
 
public:
  SimpleNetWriter(string myPortName) : AbstractNetWriter<T>(myPortName),
                                       BufferOut()
  {
#ifdef DEBUG
//  cout << "  SimpleNetWriter [CONSTRUCTOR]" << endl;
#endif
    AbstractNetWriter<T>::late_construct();
  }

  ~SimpleNetWriter()
  {
    AbstractNetWriter<T>::late_destruct();
#ifdef DEBUG
  cout << "  ~SimpleNetWriter [DESTRUCTOR]" << endl;
#endif
  }

public:
  void write(T t) // may drop packets (non blocking)
  {
    BinPortable<T> &BinOut= BufferOut.prepare();

    BinOut.content()= t;
    BufferOut.write();  
  }

  // void commit(T t) // ?? may not drop packets (blocking)

};

} } } } // end namespaces


#endif
