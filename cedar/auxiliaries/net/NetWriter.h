#include "internals/namespace.h"
#include <cedar/namespace.h>
#include <opencv/cv.h>
#include <boost/static_assert.hpp>

#include "internals/transport/simple/SimpleNetWriter.h"
#include <opencv/cv.h>

#include "internals/transport/collated/CollatedNetWriter.h"
#include "internals/datatypes/opencv/cvMatHelper.h"

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {

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
class NetWriter<char> : public _NM_FULL_::SimpleNetWriter<char>
{ 
  // dummy
public:
  explicit NetWriter(const std::string &s) 
                           : _NM_FULL_::SimpleNetWriter<char>(s)
  {
  }
};

template <>
class NetWriter<unsigned char> : public _NM_FULL_::SimpleNetWriter<unsigned char>
{ 
  // dummy
public:
  explicit NetWriter(const std::string &s) 
                           : _NM_FULL_::SimpleNetWriter<unsigned char>(s)
  {
  }
};

template <>
class NetWriter<short> : public _NM_FULL_::SimpleNetWriter<short>
{ 
  // dummy
public:
  explicit NetWriter(const std::string &s) 
                           : _NM_FULL_::SimpleNetWriter<short>(s)
  {
  }
};

template <>
class NetWriter<unsigned short> : public _NM_FULL_::SimpleNetWriter<unsigned short>
{ 
  // dummy
public:
  explicit NetWriter(const std::string &s) 
                           : _NM_FULL_::SimpleNetWriter<unsigned short>(s)
  {
  }
};

template <>
class NetWriter<unsigned int> : public _NM_FULL_::SimpleNetWriter<unsigned int>
{ 
  // dummy
public:
  explicit NetWriter(const std::string &s) 
                           : _NM_FULL_::SimpleNetWriter<unsigned int>(s)
  {
  }
};

template <>
class NetWriter<int> : public _NM_FULL_::SimpleNetWriter<int>
{ 
  // dummy
public:
  explicit NetWriter(const std::string &s) 
                           : _NM_FULL_::SimpleNetWriter<int>(s)
  {
  }
};

template <>
class NetWriter<long> : public _NM_FULL_::SimpleNetWriter<long>
{ 
  // dummy
public:
  explicit NetWriter(const std::string &s) 
                           : _NM_FULL_::SimpleNetWriter<long>(s)
  {
  }
};

template <>
class NetWriter<unsigned long> : public _NM_FULL_::SimpleNetWriter<unsigned long>
{ 
  // dummy
public:
  explicit NetWriter(const std::string &s) 
                           : _NM_FULL_::SimpleNetWriter<unsigned long>(s)
  {
  }
};

template <>
class NetWriter<bool> : public _NM_FULL_::SimpleNetWriter<bool>
{ 
  // dummy
public:
  explicit NetWriter(const std::string &s) 
                           : _NM_FULL_::SimpleNetWriter<bool>(s)
  {
  }
};

template <>
class NetWriter<float> : public _NM_FULL_::SimpleNetWriter<float>
{ 
  // dummy
public:
  explicit NetWriter(const std::string &s) 
                           : _NM_FULL_::SimpleNetWriter<float>(s)
  {
  }
};

template <>
class NetWriter<double> : public _NM_FULL_::SimpleNetWriter<double>
{ 
  // dummy
public:
  explicit NetWriter(const std::string &s) 
                           : _NM_FULL_::SimpleNetWriter<double>(s)
  {
  }
};


template <>
class NetWriter<cv::Mat> : 
                           public _NM_FULL_::cvMatHelper<cv::Mat>,
                           public _NM_FULL_::CollatedNetWriter<cv::Mat>
{
private:

public:
  explicit NetWriter(const std::string &myPortName) 
                      : _NM_FULL_::cvMatHelper<cv::Mat>(),
                        _NM_FULL_::CollatedNetWriter<cv::Mat>(myPortName)
  {
#ifdef DEBUG
  cout << "  NetWriter (cv::Mat) [CONSTRUCTOR]" << endl;
#endif
  }
};

template <>
class NetWriter< cv::Mat_<float> > : 
                           public _NM_FULL_::cvMatHelper< cv::Mat_<float> >,
                           public _NM_FULL_::CollatedNetWriter< cv::Mat_<float> >
{
private:

public:
  explicit NetWriter(const std::string &myPortName) 
                      : _NM_FULL_::cvMatHelper< cv::Mat_<float> >(),
                        _NM_FULL_::CollatedNetWriter< cv::Mat_<float> >(myPortName)
  {
  }
};

} } } // end namespaces

