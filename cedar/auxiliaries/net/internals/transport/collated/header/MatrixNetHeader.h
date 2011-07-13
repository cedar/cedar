#ifndef _MATRIX_NET_HEADER_H_
#define _MATRIX_NET_HEADER_H_

#include "../../../namespace.h"

#include "CollatedNetHeader.h"

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {

#include <yarp/os/begin_pack_for_net.h>
struct MatrixNetHeader : CollatedNetHeader
{
public:
  yarp::os::NetInt32 rows;
  yarp::os::NetInt32 cols;
  yarp::os::NetUint32 elemSize;
} PACKED_FOR_NET;

} } } }  // end namespaces

#endif
