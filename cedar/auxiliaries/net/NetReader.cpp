#include "internals/namespace.h"
#include "NetReader.h"
#include "internals/transport/simple/SimpleNetReader.h"

#include "internals/transport/collated/CollatedNetReader.h"
#include "internals/datatypes/opencv/cvMatHelper.h"

using namespace cedar::aux::net::detail;

namespace cedar {
  namespace aux {
    namespace net {


// explizite instantiierung:
template class NetReader<int>;
template class NetReader<float>;
template class NetReader<double>;
template class NetReader<cv::Mat>;

} } } // end namespaces

