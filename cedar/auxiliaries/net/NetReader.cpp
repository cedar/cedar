#include "internals/namespace.h"
#include "NetReader.h"
#include "internals/transport/simple/SimpleNetReader.h"

#include "internals/transport/collated/CollatedNetReader.h"
#include "internals/datatypes/opencv/cvMatHelper.h"

using namespace _NM_FULL_;

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {


// explizite instantiierung:
template class NetReader<int>;
template class NetReader<float>;
template class NetReader<double>;
template class NetReader<cv::Mat>;

} } } // end namespaces

