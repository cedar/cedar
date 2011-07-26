#ifndef _NETT_NET_READER_H_
#define _NETT_NET_READER_H_

#include <opencv/cv.h>
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
class NetReader<char> : public cedar::aux::net::detail::SimpleNetReader<char, false>
{ 
  // dummy
public:
  explicit NetReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<char>(s)
  {
  }
};

template <>
class NetReader<unsigned char> : public cedar::aux::net::detail::SimpleNetReader<unsigned char, false>
{ 
  // dummy
public:
  explicit NetReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned char>(s)
  {
  }
};

template <>
class NetReader<short> : public cedar::aux::net::detail::SimpleNetReader<short, false>
{ 
  // dummy
public:
  explicit NetReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<short>(s)
  {
  }
};

template <>
class NetReader<unsigned short> : public cedar::aux::net::detail::SimpleNetReader<unsigned short, false>
{ 
  // dummy
public:
  explicit NetReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned short>(s)
  {
  }
};

template <>
class NetReader<int> : public cedar::aux::net::detail::SimpleNetReader<int, false>
{ 
  // dummy
public:
  explicit NetReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<int>(s)
  {
  }
};

template <>
class NetReader<unsigned int> : public cedar::aux::net::detail::SimpleNetReader<unsigned int, false>
{ 
  // dummy
public:
  explicit NetReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned int>(s)
  {
  }
};

template <>
class NetReader<long> : public cedar::aux::net::detail::SimpleNetReader<long, false>
{ 
  // dummy
public:
  explicit NetReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<long>(s)
  {
  }
};

template <>
class NetReader<unsigned long> : public cedar::aux::net::detail::SimpleNetReader<unsigned long, false>
{ 
  // dummy
public:
  explicit NetReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<unsigned long>(s)
  {
  }
};

template <>
class NetReader<bool> : public cedar::aux::net::detail::SimpleNetReader<bool, false>
{ 
  // dummy
public:
  explicit NetReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<bool>(s)
  {
  }
};

template <>
class NetReader<float> : public cedar::aux::net::detail::SimpleNetReader<float, false>
{ 
  // dummy
public:
  explicit NetReader(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetReader<float>(s)
  {
  }
};

template <>
class NetReader<double> : public cedar::aux::net::detail::SimpleNetReader<double, false>
{ 
  // dummy
public:
  explicit NetReader(const std::string &s)
                        : cedar::aux::net::detail::SimpleNetReader<double>(s)
  {
  }
};


template <>
class NetReader<cv::Mat> : public cedar::aux::net::detail::cvMatHelper<cv::Mat>,
                           public cedar::aux::net::detail::CollatedNetReader<cv::Mat, false> 
{
public:
  explicit NetReader(const std::string &myPortName) 
                         : cedar::aux::net::detail::cvMatHelper<cv::Mat>(), 
                           cedar::aux::net::detail::CollatedNetReader<cv::Mat>(myPortName)
  {
  }
};

template <>
class NetReader< cv::Mat_<float> > : 
                           public cedar::aux::net::detail::cvMatHelper< cv::Mat_<float> >,
                           public cedar::aux::net::detail::CollatedNetReader< cv::Mat_<float> , false> 
{
public:
  explicit NetReader(const std::string &myPortName) 
                         : cedar::aux::net::detail::cvMatHelper< cv::Mat_<float> >(), 
                           cedar::aux::net::detail::CollatedNetReader< cv::Mat_<float> >(myPortName)
  {
  }
};


} } } // end namespaces

#endif

