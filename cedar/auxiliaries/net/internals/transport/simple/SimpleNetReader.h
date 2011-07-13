#ifndef _NETT_SIMPLE_NET_READER_
#define _NETT_SIMPLE_NET_READER_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "../../namespace.h"
#include "../AbstractNetReader.h"
#include <yarp/os/BufferedPort.h>
#include <yarp/os/BinPortable.h>

using namespace std;
using namespace yarp::os;

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {

template <typename T, bool block= false>
class SimpleNetReader : public AbstractNetReader<T>
{
private:
  BufferedPort< BinPortable<T> > BufferIn;
public:
  SimpleNetReader(string myPortName) : AbstractNetReader<T>(myPortName),
                                       BufferIn()
  {
    AbstractNetReader<T>::late_construct();
  }

  ~SimpleNetReader()
  {
    //AbstractNetWriter<T>::late_destruct();
#ifdef DEBUG
  cout << "  ~NetReader [DESTRUCTOR]" << endl;
#endif
  }

protected:
  virtual void open()
  {
    BufferIn.open( AbstractNetBase::getFullPortName().c_str() ); 
  }

  virtual void close()
  {
    if (!BufferIn.isClosed())
      BufferIn.close();
  }

public:
  T read() // blocking!
  {
    BinPortable<T> *pBinIn;

    pBinIn= BufferIn.read( block ); // strict mode!
    if (pBinIn !=0)
    {
      return pBinIn->content();
    }
    else // TODO: hier kann man nicht landen
    {
cout << "read GOT NOTHING" << endl;
      return 0;
    }
  }

};

} } } } // end namespaces

#endif

