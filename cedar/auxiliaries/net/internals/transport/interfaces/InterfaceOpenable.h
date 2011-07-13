#ifndef _NETT_IF_OPENABLE_
#define _NETT_IF_OPENABLE_

#include "../../namespace.h"
#include <string>

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {

class InterfaceOpenable
{
protected:
  virtual void open() = 0;
  virtual void close() = 0;
  virtual std::string getFullPortName() = 0;
};

} } } } // end namespace

#endif
