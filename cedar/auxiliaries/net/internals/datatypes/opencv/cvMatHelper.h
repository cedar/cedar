#ifndef _NETT_CV_MAT_HELPER_
#define _NETT_CV_MAT_HELPER_

#include "../../namespace.h"
#include "../interfaces/InterfaceCollatedData.h"
#include "../CollatedTraits.h"
#include "cvMatNetHeader.h"
#include <opencv/cv.h>

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {

class cvMatHelper  : virtual protected InterfaceCollatedData<cv::Mat>
{
public:
  typedef cvMatNetHeader HeaderType;

private:
  HeaderType mCheckHeader;

  void init_externalheader(cv::Mat &mat, HeaderType &extheader);
  void init_checkheader(HeaderType &extheader);

protected:
  bool check_collateddata_for_write(cv::Mat &mat, HeaderType &extheader);
  bool check_collateddata_for_read(HeaderType &extheader);

public:
  cvMatHelper();
  virtual ~cvMatHelper();
};

} } } } // end namespaces

#endif
