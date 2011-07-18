#ifndef _NETT_COLLATED_TRAITS_OPENCV_H_
#define _NETT_COLLATED_TRAITS_OPENCV_H_

// traits specializations:
#include "../../namespace.h"
#include <opencv/cv.h>


namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {
        template<> struct collated_traits<cv::Mat>;
        template<> struct collated_traits< cv::Mat_<float> >;
        template <typename CVT> class cvMatHelper;
        class cvMatNetHeader;
      }
    }
  }
}


template<>
struct _NM_FULL_::collated_traits<cv::Mat>
{
  typedef cv::Mat                   data_type;
  typedef _NM_FULL_::cvMatHelper<cv::Mat>    helper_type;
  typedef _NM_FULL_::cvMatNetHeader header_type;
};

template<>
struct _NM_FULL_::collated_traits< cv::Mat_<float> >
{
  typedef cv::Mat_<float>           data_type;
  typedef _NM_FULL_::cvMatHelper< cv::Mat_<float> >    helper_type;
  typedef _NM_FULL_::cvMatNetHeader header_type;
};


//} } } } // end namespaces 
     
#endif
