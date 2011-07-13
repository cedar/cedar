#ifndef _NETT_COLLATED_NET_BASE_
#define _NETT_COLLATED_NET_BASE_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "../../namespace.h"
#include "../../datatypes/interfaces/InterfaceCollatedData.h"
#include "../interfaces/InterfaceOpenable.h"

#include "../../datatypes/CollatedTraits.h"
#include "../../datatypes/CollatedType.h"
#include "CollatedNetPortable.h"

using namespace std;

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {

template <typename T> 
class CollatedNetBase : virtual protected InterfaceOpenable,
                        virtual protected InterfaceCollatedData<T>
{
protected:
  yarp::os::Port dataPort;

protected:

public:
  void open()
  {
    dataPort.open( getFullPortName().c_str() );
  }

  void close()
  {
    dataPort.close();
  }

public:
  CollatedNetBase() : dataPort()
  {
#ifdef DEBUG
//  cout << "  CollatedNetBase [CONSTRUCTOR]" << endl;
#endif
    dataPort.enableBackgroundWrite(true);
    // ... 

  }

  virtual ~CollatedNetBase()
  {
  }
};

} } } }  // end namespaces

#endif
