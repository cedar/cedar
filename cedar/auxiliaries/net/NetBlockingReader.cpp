#include "internals/namespace.h"
#include "NetBlockingReader.h"
#include "internals/transport/simple/SimpleNetReader.h"

#include "internals/transport/collated/CollatedNetReader.h"
#include "internals/datatypes/opencv/cvMatHelper.h"

#include <opencv/cv.h>

using namespace _NM_FULL_;

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {

template class NetBlockingReader<int>;
template class NetBlockingReader<float>;
template class NetBlockingReader<double>;
template class NetBlockingReader<cv::Mat>;

} } }  // end namespace

