#ifndef _COLLATED_TYPE_H_
#define _COLLATED_TYPE_H_

#include "../namespace.h"
#include "CollatedTraits.h"

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {

template <typename T>
struct CollatedType
{
public:
  typedef collated_traits<T>                traits_type;
  typedef typename traits_type::data_type   data_type;
  typedef typename traits_type::header_type header_type;

  header_type header;
  data_type   data;   

  CollatedType() : header(), data()
  {
    // Member liegen im stack
    header= header_type();
    data=   data_type(); // wichtig: Groesse der Matrix wird spaeter gesetzt
  }

  // inline weil von innerer Schleife aufgerufen
  inline void* contentAt(int index, int iElemSize)
  {
    // this should not compile
    return NULL;
  }

  inline T late_init_data_from_header()
  {
    return 0;
  }
};


} } } } // end namespaces


// Spezialisierungen: ...
#include "opencv/CollatedType.h"

#endif
