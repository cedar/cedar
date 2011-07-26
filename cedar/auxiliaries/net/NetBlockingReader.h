#ifndef _NETT_NET_BLOCKINGREADER_H_
#define _NETT_NET_BLOCKINGREADER_H_

#include "internals/namespace.h"
#include "internals/transport/simple/SimpleNetReader.h"

#include "internals/transport/collated/CollatedNetReader.h"
#include "internals/datatypes/opencv/cvMatHelper.h"
#include <opencv/cv.h>

namespace cedar {
  namespace aux {
    namespace net {

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
class NetBlockingReader<char> : public cedar::aux::net::detail::SimpleNetReader<char, false>
{ 
  // dummy
public:
  explicit NetBlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<char>(s)
  {
  }
};

template <>
class NetBlockingReader<unsigned char> : public cedar::aux::net::detail::SimpleNetReader<unsigned char, false>
{ 
  // dummy
public:
  explicit NetBlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned char>(s)
  {
  }
};

template <>
class NetBlockingReader<short> : public cedar::aux::net::detail::SimpleNetReader<short, false>
{ 
  // dummy
public:
  explicit NetBlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<short>(s)
  {
  }
};

template <>
class NetBlockingReader<unsigned short> : public cedar::aux::net::detail::SimpleNetReader<unsigned short, false>
{ 
  // dummy
public:
  explicit NetBlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned short>(s)
  {
  }
};

template <>
class NetBlockingReader<int> : public cedar::aux::net::detail::SimpleNetReader<int, false>
{ 
  // dummy
public:
  explicit NetBlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<int>(s)
  {
  }
};

template <>
class NetBlockingReader<unsigned int> : public cedar::aux::net::detail::SimpleNetReader<unsigned int, false>
{ 
  // dummy
public:
  explicit NetBlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned int>(s)
  {
  }
};

template <>
class NetBlockingReader<long> : public cedar::aux::net::detail::SimpleNetReader<long, false>
{ 
  // dummy
public:
  explicit NetBlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<long>(s)
  {
  }
};

template <>
class NetBlockingReader<unsigned long> : public cedar::aux::net::detail::SimpleNetReader<unsigned long, false>
{ 
  // dummy
public:
  explicit NetBlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned long>(s)
  {
  }
};

template <>
class NetBlockingReader<bool> : public cedar::aux::net::detail::SimpleNetReader<bool, false>
{ 
  // dummy
public:
  explicit NetBlockingReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<bool>(s)
  {
  }
};

template <>
class NetBlockingReader<float> : public cedar::aux::net::detail::SimpleNetReader<float, true>
{ 
  // dummy
public:
  explicit NetBlockingReader(const std::string &s) 
                       : cedar::aux::net::detail::SimpleNetReader<float, true>(s)
  {
  }
};

template <>
class NetBlockingReader<double> : public cedar::aux::net::detail::SimpleNetReader<double, true>
{ 
  // dummy
public:
  explicit NetBlockingReader(const std::string &s) 
                       : cedar::aux::net::detail::SimpleNetReader<double, true>(s)
  {
  }
};

template <>
class NetBlockingReader<cv::Mat> : public cedar::aux::net::detail::cvMatHelper<cv::Mat>,
                                   public cedar::aux::net::detail::CollatedNetReader<cv::Mat, true> 
{
public:
  explicit NetBlockingReader(const std::string &myPortName) 
                         : cedar::aux::net::detail::cvMatHelper<cv::Mat>(), 
                           cedar::aux::net::detail::CollatedNetReader<cv::Mat, true>(myPortName)
  {
  }
};

template <>
class NetBlockingReader< cv::Mat_<float> > 
                         : public cedar::aux::net::detail::cvMatHelper< cv::Mat_<float> >,
                           public cedar::aux::net::detail::CollatedNetReader< cv::Mat_<float> , true> 
{
public:
  explicit NetBlockingReader(const std::string &myPortName) 
                         : cedar::aux::net::detail::cvMatHelper< cv::Mat_<float> >(), 
                           cedar::aux::net::detail::CollatedNetReader< cv::Mat_<float> , true>(myPortName)
  {
  }
};

} } } // end namespaces

#endif
