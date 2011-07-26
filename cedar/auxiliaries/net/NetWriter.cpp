#include "internals/namespace.h"
#include "NetWriter.h"

#include "internals/transport/simple/SimpleNetWriter.h"
#include <opencv/cv.h>

#include "internals/transport/collated/CollatedNetWriter.h"
#include "internals/datatypes/opencv/cvMatHelper.h"



namespace cedar {
  namespace aux {
    namespace net {

// explicit instatiation:
template class NetWriter<int>;
template class NetWriter<float>;
template class NetWriter<double>;
template class NetWriter<cv::Mat>;

} } } // end namespaces


