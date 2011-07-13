#include "cvMatHelper.h"

#include "../interfaces/InterfaceCollatedData.h"
#include "cvMatNetHeader.h"
#include "../../transport/collated/header/MatrixNetHeaderAccessor.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <opencv/cv.h>
#include <yarp/os/NetInt32.h>
#include <yarp/os/NetFloat64.h>

using namespace std;


#define MAGIC_NUMBER 0xabc

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {


/*class cvMatHelper : virtual protected InterfaceCollatedData<cv::Mat>
{
};*/

void cvMatHelper::init_checkheader(HeaderType &header)
{
  // einmalig Header-Infos fuellen 
  mCheckHeader.magicNumber= yarp::os::NetInt32(MAGIC_NUMBER);
  mCheckHeader.rows= header.rows;
  mCheckHeader.cols= header.cols;
  mCheckHeader.elemSize= header.elemSize;
  mCheckHeader.cvMatType= header.cvMatType;
}

void cvMatHelper::cvMatHelper::init_externalheader(cv::Mat &mat,
                                                   HeaderType &header)
{
  header.cols= mat.cols;
  header.rows= mat.rows;
  header.elemSize= mat.elemSize();
  header.cvMatType= mat.type();
}

bool cvMatHelper::check_collateddata_for_write(cv::Mat &mat,
                                               HeaderType &extheader)
{
    if (!mCheckHeader.cols || !mCheckHeader.rows)
    {
      init_externalheader(mat, extheader); // get info from mat!

      init_checkheader(extheader);
      return true;
    }
    else
    {
      if (mCheckHeader.cols != mat.cols
          || mCheckHeader.rows != mat.rows
          || mCheckHeader.elemSize != mat.elemSize() 
          || mCheckHeader.cvMatType != mat.type() )
      {
        // TODO: Fehler, wenn falsche Groesse?
        return false;
      }
      else
      {
        init_externalheader(mat, extheader);
        return true;
      }
    }
}

bool cvMatHelper::check_collateddata_for_read(HeaderType &extheader)
{
   if (!mCheckHeader.cols || !mCheckHeader.rows)
    {
       init_checkheader(extheader); // get info from header!
       // TODO: Test magic number

       return true;
    }
    else
    {
      if (mCheckHeader.cols != extheader.cols
          || mCheckHeader.rows != extheader.rows
          || mCheckHeader.elemSize != extheader.elemSize
          || mCheckHeader.cvMatType != extheader.cvMatType )
      {
        // TODO: Fehler, wenn falsche Groesse?
        return false;
      }

      return true;
    }
}

cvMatHelper::cvMatHelper() : mCheckHeader()
{
#ifdef DEBUG
//  cout << "  cvMatHelper [CONSTRUCTOR]" << endl;
#endif
    mCheckHeader.rows= 0;
    mCheckHeader.cols= 0;
}

cvMatHelper::~cvMatHelper()
{
#ifdef DEBUG
  cout << "  ~cvMatHelper [DESTRUCTOR]" << endl;
#endif
}

} } } } // end namespaces

