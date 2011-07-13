#include "../../../namespace.h"
#include "MatrixNetHeader.h"
#include "MatrixNetHeaderAccessor.h"

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {

unsigned int MatrixNetHeaderAccessor::getDataSize(MatrixNetHeader &header)
{
  return header.elemSize * header.rows * header.cols;
}

unsigned int MatrixNetHeaderAccessor::getTotalElements(MatrixNetHeader &header)
{
  return header.rows * header.cols;
}

unsigned int MatrixNetHeaderAccessor::getElemSize(MatrixNetHeader &header)
{
  return header.elemSize;
}


} } } } // end namespaces 

