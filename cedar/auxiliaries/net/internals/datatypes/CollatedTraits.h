#ifndef _NETT_COLLATED_TRAITS_H_
#define _NETT_COLLATED_TRAITS_H_

#include "../namespace.h"

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {

template<class T>
struct collated_traits
{
  // default traits struct is empty
  // TODO: this should not compile
};


} } } } // end namespaces 

// specializations (these will compile):
#include "opencv/CollatedTraits.h"



#endif

