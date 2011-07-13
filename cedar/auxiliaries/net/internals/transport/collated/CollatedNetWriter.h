#ifndef _NETT_COLLATED_NET_WRITER_
#define _NETT_COLLATED_NET_WRITER_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "../../namespace.h"
#include "CollatedNetBase.h"
#include "../../datatypes/CollatedType.h"
#include "CollatedNetPortable.h"
#include "../../../exceptions/NetException.h"

#include "../AbstractNetWriter.h"
#include <yarp/os/PortWriterBuffer.h>

#include <yarp/os/PortReport.h>

using namespace std;

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {


template <typename T>
class CollatedNetWriter : 
                          public CollatedNetBase<T>,
                          public AbstractNetWriter<T>
{
private:
  PortWriterBuffer< CollatedNetPortable< T > > elemWrapper;

public:
  CollatedNetWriter(string myPortName) : 
                                         CollatedNetBase<T>(),
                                         AbstractNetWriter<T>(myPortName),
                                         elemWrapper()
  {
#ifdef DEBUG
//  cout << "  CollatedNetWriter [CONSTRUCTOR]" << endl;
#endif
    AbstractNetWriter<T>::late_construct();

    elemWrapper.attach( CollatedNetBase<T>::dataPort );
  }

  ~CollatedNetWriter()
  {
    AbstractNetWriter<T>::late_destruct();
#ifdef DEBUG
  cout << "  ~CollatedNetWriter [DESTRUCTOR]" << endl;
#endif
  }

public:
  void write(T t) // may drop packets (non blocking)
  {
    // TODO: wenn keine connection/network
    //       dann nichts machen

    // Matrixgroesse verifizieren
    if (!check_collateddata_for_write(t,
                                      elemWrapper.prepare().header() ) )
    {
      // TODO: fehler verarbeiten oder irgenwas tun ...
cout << "  FEHLER: Matrix mit unterschiedlicher Groesse" << endl;
      return;
    }

    // uebertragen ...
    elemWrapper.prepare().content() = t;
    elemWrapper.write(true); // TRUE IS IMPORTANT
  }

};

} } } } // end namespaces

#endif

