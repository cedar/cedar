#ifndef _CV_MAT_NET_HEADER_H_
#define _CV_MAT_NET_HEADER_H_

#include "../../namespace.h"
#include "../../transport/collated/header/MatrixNetHeader.h"

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {

#include <yarp/os/begin_pack_for_net.h>
struct cvMatNetHeader : MatrixNetHeader
{
public:
  yarp::os::NetInt32 cvMatType;
} PACKED_FOR_NET;

} } } } // end namespaces

#endif
