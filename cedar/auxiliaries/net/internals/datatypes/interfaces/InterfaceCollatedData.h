#ifndef _NETT_IF_COLLATED_DATA_H_
#define _NETT_IF_COLLATED_DATA_H_

#include "../../namespace.h"
#include "../CollatedTraits.h"

namespace _NM_CEDAR_ {
  namespace _NM_AUX_ {
    namespace _NM_NET_ {
      namespace _NM_INTERNAL_ {


template<typename T> 
class InterfaceCollatedData
{
protected:
  typedef collated_traits<T>                traits_type;
  typedef typename traits_type::data_type   data_type;
  typedef typename traits_type::header_type header_type;

protected:
  virtual bool check_collateddata_for_write(data_type &data,
                                            header_type &header) = 0;
  virtual bool check_collateddata_for_read(header_type &header) = 0;
};


} } } } // end namespace

#endif
