#ifndef _NETT_NET_READER_H_
#define _NETT_NET_READER_H_

#include <opencv/cv.h>
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
class NetReader
{
   explicit NetReader()
   {
     // this will be triggered if this type is instantiated
     BOOST_STATIC_ASSERT(sizeof(T) == 0); 
   }
};

// spezialisierungs-template!
template <>
class NetReader<int> : public _NM_FULL_::SimpleNetReader<int, false>
{ 
  // dummy
public:
  explicit NetReader(std::string s) : _NM_FULL_::SimpleNetReader<int>(s)
  {
  }
};

template <>
class NetReader<float> : public _NM_FULL_::SimpleNetReader<float, false>
{ 
  // dummy
public:
  explicit NetReader(std::string s) : _NM_FULL_::SimpleNetReader<float>(s)
  {
  }
};

template <>
class NetReader<double> : public _NM_FULL_::SimpleNetReader<double, false>
{ 
  // dummy
public:
  explicit NetReader(std::string s) : _NM_FULL_::SimpleNetReader<double>(s)
  {
  }
};


template <>
class NetReader<cv::Mat> : public _NM_FULL_::cvMatHelper<cv::Mat>,
                           public _NM_FULL_::CollatedNetReader<cv::Mat, false> 
{
public:
  explicit NetReader(std::string myPortName) 
                         : _NM_FULL_::cvMatHelper<cv::Mat>(), 
                           _NM_FULL_::CollatedNetReader<cv::Mat>(myPortName)
  {
  }
};

template <>
class NetReader< cv::Mat_<float> > : 
                           public _NM_FULL_::cvMatHelper< cv::Mat_<float> >,
                           public _NM_FULL_::CollatedNetReader< cv::Mat_<float> , false> 
{
public:
  explicit NetReader(std::string myPortName) 
                         : _NM_FULL_::cvMatHelper< cv::Mat_<float> >(), 
                           _NM_FULL_::CollatedNetReader< cv::Mat_<float> >(myPortName)
  {
  }
};


} } } // end namespaces

#endif

