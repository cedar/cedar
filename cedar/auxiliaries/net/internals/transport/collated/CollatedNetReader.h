#ifndef _NETT_COLLATED_NET_READER_
#define _NETT_COLLATED_NET_READER_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "../../namespace.h"
#include "CollatedNetBase.h"
#include "../../datatypes/CollatedType.h"
#include "CollatedNetPortable.h"
#include "../../../exceptions/NetException.h"

#include "../AbstractNetReader.h"
#include <yarp/os/PortReaderBuffer.h>

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {


template <typename T, bool block= false>
class CollatedNetReader : 
                          public CollatedNetBase<T>,
                          public AbstractNetReader<T>
{
private:
  PortReaderBuffer< CollatedNetPortable< T > > elemWrapper;

public:
  CollatedNetReader(std::string myPortName) : 
                                         CollatedNetBase<T>(),
                                         AbstractNetReader<T>(myPortName),
                                         elemWrapper()
  {
#ifdef DEBUG
  cout << "  CollatedNetReader [CONSTRUCTOR]" << endl;
#endif
    elemWrapper.setStrict();

    AbstractNetReader<T>::late_construct();

    elemWrapper.attach( CollatedNetBase<T>::dataPort );
  }

  ~CollatedNetReader()
  {
#ifdef DEBUG
    cout << "  ~CollatedNetReader [DESTRUCTOR] begin" << endl;
#endif
    AbstractNetReader<T>::late_destruct();
#ifdef DEBUG
    cout << "  ~CollatedNetReader [DESTRUCTOR] end" << endl;
#endif
  }

public:
  T read()
  {  
    cv::Mat retMat;
    CollatedNetPortable< T > *pNetPortable;

#ifdef DEBUG
    cout << "  tryring reconnect" << endl;
#endif
    if (!AbstractNetReader<T>::isConnected())
    {
#ifdef DEBUG
    cout << "  BLUBB tryring reconnect" << endl;
#endif
      if (!AbstractNetReader<T>::reconnect())
      {
        // do something
        CEDAR_THROW( cedar::aux::exc::NetWaitingForWriterException,
                     "YARP: cannot connect reader port; waiting for "
                     "writer" );
      }
    }

    pNetPortable= elemWrapper.read( block ); // Argument: shouldWait
 
    if ( pNetPortable == NULL )
    {
      CEDAR_THROW( cedar::aux::exc::NetUnexpectedDataException,
                   "YARP: unexpectedly read NULL from port" );
    }
    else
    {
      retMat= pNetPortable->content();
        // TODO: think about reference counter fuer Mat
      return retMat;
    }
  }
};

} } } }  // end namespaces

#endif

