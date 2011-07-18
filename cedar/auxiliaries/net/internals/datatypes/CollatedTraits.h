#ifndef _NETT_COLLATED_TRAITS_H_
#define _NETT_COLLATED_TRAITS_H_

#include "../namespace.h"
#include <boost/static_assert.hpp>

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {

template<class T>
struct collated_traits
{
  // default traits struct is empty
  // this should not compile
  collated_traits()
  {
    BOOST_STATIC_ASSERT(sizeof(T) == 0); 
  }
};


} } } } // end namespaces 

// specializations (these will compile):
#include "opencv/CollatedTraits.h"



#endif

