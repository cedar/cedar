#include "internals/namespace.h"
#include <cedar/namespace.h>
#include <opencv/cv.h>
#include <boost/static_assert.hpp>

#include "internals/transport/simple/SimpleNetWriter.h"
#include <opencv/cv.h>

#include "internals/transport/collated/CollatedNetWriter.h"
#include "internals/datatypes/opencv/cvMatHelper.h"

namespace cedar {
  namespace aux {
    namespace net {

// this template cannot be instantiated if there is not a
// valid specialization
template <typename T>
class NetWriter
{
  NetWriter()
  {
   // this will be triggered if this type is instantiated
   BOOST_STATIC_ASSERT(sizeof(T) == 0); 
  }
};


template <>
class NetWriter<char> : public cedar::aux::net::detail::SimpleNetWriter<char>
{ 
  // dummy
public:
  explicit NetWriter(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<char>(s)
  {
  }
};

template <>
class NetWriter<unsigned char> : public cedar::aux::net::detail::SimpleNetWriter<unsigned char>
{ 
  // dummy
public:
  explicit NetWriter(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<unsigned char>(s)
  {
  }
};

template <>
class NetWriter<short> : public cedar::aux::net::detail::SimpleNetWriter<short>
{ 
  // dummy
public:
  explicit NetWriter(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<short>(s)
  {
  }
};

template <>
class NetWriter<unsigned short> : public cedar::aux::net::detail::SimpleNetWriter<unsigned short>
{ 
  // dummy
public:
  explicit NetWriter(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<unsigned short>(s)
  {
  }
};

template <>
class NetWriter<unsigned int> : public cedar::aux::net::detail::SimpleNetWriter<unsigned int>
{ 
  // dummy
public:
  explicit NetWriter(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<unsigned int>(s)
  {
  }
};

template <>
class NetWriter<int> : public cedar::aux::net::detail::SimpleNetWriter<int>
{ 
  // dummy
public:
  explicit NetWriter(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<int>(s)
  {
  }
};

template <>
class NetWriter<long> : public cedar::aux::net::detail::SimpleNetWriter<long>
{ 
  // dummy
public:
  explicit NetWriter(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<long>(s)
  {
  }
};

template <>
class NetWriter<unsigned long> : public cedar::aux::net::detail::SimpleNetWriter<unsigned long>
{ 
  // dummy
public:
  explicit NetWriter(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<unsigned long>(s)
  {
  }
};

template <>
class NetWriter<bool> : public cedar::aux::net::detail::SimpleNetWriter<bool>
{ 
  // dummy
public:
  explicit NetWriter(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<bool>(s)
  {
  }
};

template <>
class NetWriter<float> : public cedar::aux::net::detail::SimpleNetWriter<float>
{ 
  // dummy
public:
  explicit NetWriter(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<float>(s)
  {
  }
};

template <>
class NetWriter<double> : public cedar::aux::net::detail::SimpleNetWriter<double>
{ 
  // dummy
public:
  explicit NetWriter(const std::string &s) 
                           : cedar::aux::net::detail::SimpleNetWriter<double>(s)
  {
  }
};


template <>
class NetWriter<cv::Mat> : 
                           public cedar::aux::net::detail::cvMatHelper<cv::Mat>,
                           public cedar::aux::net::detail::CollatedNetWriter<cv::Mat>
{
private:

public:
  explicit NetWriter(const std::string &myPortName) 
                      : cedar::aux::net::detail::cvMatHelper<cv::Mat>(),
                        cedar::aux::net::detail::CollatedNetWriter<cv::Mat>(myPortName)
  {
#ifdef DEBUG
  cout << "  NetWriter (cv::Mat) [CONSTRUCTOR]" << endl;
#endif
  }
};

template <>
class NetWriter< cv::Mat_<float> > : 
                           public cedar::aux::net::detail::cvMatHelper< cv::Mat_<float> >,
                           public cedar::aux::net::detail::CollatedNetWriter< cv::Mat_<float> >
{
private:

public:
  explicit NetWriter(const std::string &myPortName) 
                      : cedar::aux::net::detail::cvMatHelper< cv::Mat_<float> >(),
                        cedar::aux::net::detail::CollatedNetWriter< cv::Mat_<float> >(myPortName)
  {
  }
};

} } } // end namespaces

