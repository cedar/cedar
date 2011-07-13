#ifndef _COLLATED_NET_HEADER_H_
#define _COLLATED_NET_HEADER_H_

#include "../../../namespace.h"

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {

#include <yarp/os/begin_pack_for_net.h>
struct CollatedNetHeader
{
  // dummy for polymorphism
  yarp::os::NetUint32 magicNumber;
} PACKED_FOR_NET;

} } } } // end namespaces
      
#endif
