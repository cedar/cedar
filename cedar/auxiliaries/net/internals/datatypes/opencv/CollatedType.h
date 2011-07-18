//
// openCV Spezialisierung fuer CollatedType
// 

#ifndef _NETT_COLL_TYPE_OPENCV_H_
#define _NETT_COLL_TYPE_OPENCV_H_

#include "../../namespace.h"
#include <opencv/cv.h>
#include "cvMatHelper.h"
#include "../CollatedTraits.h"

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {


/////// cv::Mat

template <> // for class template
inline void* CollatedType<cv::Mat>::contentAt(int index, int iElemSize) 
                // specialization for class template
{
  return (void*) &( data.ptr( index % header.rows )[ (index 
                                                     - ( index 
                                                        % header.rows) )
                                                     / header.rows 
                                                     * iElemSize] );

}

template <>
inline cv::Mat CollatedType<cv::Mat>::late_init_data_from_header()
{
  return cv::Mat( header.rows,
                    header.cols, 
                    header.cvMatType );
}

/////// cv::Mat_

template <> // for class template
inline void* CollatedType< cv::Mat_<float> >::contentAt(int index, int iElemSize) 
                // specialization for class template
{
  return (void*)  &data( ( index % header.rows ),
                         ( index 
                             - ( index 
                                 % header.rows) )
                         / header.rows 
                         );
}

template <>
inline cv::Mat_<float> CollatedType< cv::Mat_<float> >::late_init_data_from_header()
{
  return cv::Mat_<float>( header.rows,
                          header.cols );
}


} } } } // end namespaces

#endif
