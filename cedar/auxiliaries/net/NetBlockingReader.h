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
class NetBlockingReader<char> : public _NM_FULL_::SimpleNetReader<char, false>
{ 
  // dummy
public:
  explicit NetBlockingReader(const std::string &s) 
                           : _NM_FULL_::SimpleNetReader<char>(s)
  {
  }
};

template <>
class NetBlockingReader<unsigned char> : public _NM_FULL_::SimpleNetReader<unsigned char, false>
{ 
  // dummy
public:
  explicit NetBlockingReader(const std::string &s) 
                           : _NM_FULL_::SimpleNetReader<unsigned char>(s)
  {
  }
};

template <>
class NetBlockingReader<short> : public _NM_FULL_::SimpleNetReader<short, false>
{ 
  // dummy
public:
  explicit NetBlockingReader(const std::string &s) 
                           : _NM_FULL_::SimpleNetReader<short>(s)
  {
  }
};

template <>
class NetBlockingReader<unsigned short> : public _NM_FULL_::SimpleNetReader<unsigned short, false>
{ 
  // dummy
public:
  explicit NetBlockingReader(const std::string &s) 
                           : _NM_FULL_::SimpleNetReader<unsigned short>(s)
  {
  }
};

template <>
class NetBlockingReader<int> : public _NM_FULL_::SimpleNetReader<int, false>
{ 
  // dummy
public:
  explicit NetBlockingReader(const std::string &s) 
                           : _NM_FULL_::SimpleNetReader<int>(s)
  {
  }
};

template <>
class NetBlockingReader<unsigned int> : public _NM_FULL_::SimpleNetReader<unsigned int, false>
{ 
  // dummy
public:
  explicit NetBlockingReader(const std::string &s) 
                           : _NM_FULL_::SimpleNetReader<unsigned int>(s)
  {
  }
};

template <>
class NetBlockingReader<long> : public _NM_FULL_::SimpleNetReader<long, false>
{ 
  // dummy
public:
  explicit NetBlockingReader(const std::string &s) 
                           : _NM_FULL_::SimpleNetReader<long>(s)
  {
  }
};

template <>
class NetBlockingReader<unsigned long> : public _NM_FULL_::SimpleNetReader<unsigned long, false>
{ 
  // dummy
public:
  explicit NetBlockingReader(const std::string &s) 
                           : _NM_FULL_::SimpleNetReader<unsigned long>(s)
  {
  }
};

template <>
class NetBlockingReader<bool> : public _NM_FULL_::SimpleNetReader<bool, false>
{ 
  // dummy
public:
  explicit NetBlockingReader(const std::string &s) 
                           : _NM_FULL_::SimpleNetReader<bool>(s)
  {
  }
};

template <>
class NetBlockingReader<float> : public _NM_FULL_::SimpleNetReader<float, true>
{ 
  // dummy
public:
  explicit NetBlockingReader(const std::string &s) 
                       : _NM_FULL_::SimpleNetReader<float, true>(s)
  {
  }
};

template <>
class NetBlockingReader<double> : public _NM_FULL_::SimpleNetReader<double, true>
{ 
  // dummy
public:
  explicit NetBlockingReader(const std::string &s) 
                       : _NM_FULL_::SimpleNetReader<double, true>(s)
  {
  }
};

template <>
class NetBlockingReader<cv::Mat> : public _NM_FULL_::cvMatHelper<cv::Mat>,
                                   public _NM_FULL_::CollatedNetReader<cv::Mat, true> 
{
public:
  explicit NetBlockingReader(const std::string &myPortName) 
                         : _NM_FULL_::cvMatHelper<cv::Mat>(), 
                           _NM_FULL_::CollatedNetReader<cv::Mat, true>(myPortName)
  {
  }
};

template <>
class NetBlockingReader< cv::Mat_<float> > 
                         : public _NM_FULL_::cvMatHelper< cv::Mat_<float> >,
                           public _NM_FULL_::CollatedNetReader< cv::Mat_<float> , true> 
{
public:
  explicit NetBlockingReader(const std::string &myPortName) 
                         : _NM_FULL_::cvMatHelper< cv::Mat_<float> >(), 
                           _NM_FULL_::CollatedNetReader< cv::Mat_<float> , true>(myPortName)
  {
  }
};

} } } // end namespaces

#endif
