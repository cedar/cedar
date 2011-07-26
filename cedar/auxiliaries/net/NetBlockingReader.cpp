#include "internals/namespace.h"
#include "NetBlockingReader.h"
#include "internals/transport/simple/SimpleNetReader.h"

#include "internals/transport/collated/CollatedNetReader.h"
#include "internals/datatypes/opencv/cvMatHelper.h"

#include <opencv/cv.h>

using namespace cedar::aux::net::detail;

namespace cedar {
  namespace aux {
    namespace net {

template class NetBlockingReader<int>;
template class NetBlockingReader<float>;
template class NetBlockingReader<double>;
template class NetBlockingReader<cv::Mat>;

} } }  // end namespace

