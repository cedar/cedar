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
class NetWriter<int> : public _NM_FULL_::SimpleNetWriter<int>
{ 
  // dummy
public:
  explicit NetWriter(std::string s) : _NM_FULL_::SimpleNetWriter<int>(s)
  {
  }
};

template <>
class NetWriter<float> : public _NM_FULL_::SimpleNetWriter<float>
{ 
  // dummy
public:
  explicit NetWriter(std::string s) : _NM_FULL_::SimpleNetWriter<float>(s)
  {
  }
};
template <>
class NetWriter<double> : public _NM_FULL_::SimpleNetWriter<double>
{ 
  // dummy
public:
  explicit NetWriter(std::string s) : _NM_FULL_::SimpleNetWriter<double>(s)
  {
  }
};


template <>
class NetWriter<cv::Mat> : 
                           public _NM_FULL_::cvMatHelper,
                           public _NM_FULL_::CollatedNetWriter<cv::Mat>
{
private:

public:
  explicit NetWriter(std::string myPortName) 
                      : _NM_FULL_::cvMatHelper(),
                        _NM_FULL_::CollatedNetWriter<cv::Mat>(myPortName)
  {
#ifdef DEBUG
//  cout << "  NetWriter (cv::Mat) [CONSTRUCTOR]" << endl;
#endif
  }
  
  ~NetWriter()
  {
#ifdef DEBUG
  cout << "  ~NetWriter (cv::Mat) [DESTRUCTOR]" << endl;
#endif
  }
};


} } } // end namespaces

