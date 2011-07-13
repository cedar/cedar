#ifndef _NETT_MATRIX_HEADER_NET_ACCESSOR_H_
#define _NETT_MATRIX_HEADER_NET_ACCESSOR_H_

#include "../../../namespace.h"
#include "MatrixNetHeader.h"

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {


class MatrixNetHeaderAccessor
{
public:
  static unsigned int getDataSize(MatrixNetHeader &header);
  static unsigned int getTotalElements(MatrixNetHeader &header);
  static unsigned int getElemSize(MatrixNetHeader &header);
};

} } } } // end namespaces

#endif
