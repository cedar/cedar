#ifndef _NETT_NET_BLOCKINGREADER_H_
#define _NETT_NET_BLOCKINGREADER_H_

#include "internals/namespace.h"
#include "internals/transport/simple/SimpleNetReader.h"

#include "internals/transport/collated/CollatedNetReader.h"
#include "internals/datatypes/opencv/cvMatHelper.h"
#include <opencv/cv.h>

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {

// this template cannot be instantiated if there is not a
// valid specialization
template <typename T>
class NetBlockingReader
{
   NetBlockingReader()
   {
     // this will be triggered if this type is instantiated
     BOOST_STATIC_ASSERT(sizeof(T) == 0); 
   }
};


template <>
class NetBlockingReader<int> : public _NM_FULL_::SimpleNetReader<int, true>
{ 
  // dummy
public:
  explicit NetBlockingReader(std::string s) : _NM_FULL_::SimpleNetReader<int, true>(s)
  {
  }
};

template <>
class NetBlockingReader<float> : public _NM_FULL_::SimpleNetReader<float, true>
{ 
  // dummy
public:
  explicit NetBlockingReader(std::string s) : _NM_FULL_::SimpleNetReader<float, true>(s)
  {
  }
};

template <>
class NetBlockingReader<double> : public _NM_FULL_::SimpleNetReader<double, true>
{ 
  // dummy
public:
  explicit NetBlockingReader(std::string s) : _NM_FULL_::SimpleNetReader<double, true>(s)
  {
  }
};

template <>
class NetBlockingReader<cv::Mat> : public _NM_FULL_::cvMatHelper,
                                   public _NM_FULL_::CollatedNetReader<cv::Mat, true> 
{
public:
  explicit NetBlockingReader(std::string myPortName) 
                         : _NM_FULL_::cvMatHelper(), 
                           _NM_FULL_::CollatedNetReader<cv::Mat, true>(myPortName)
  {
  }
};

} } } // end namespaces

#endif
