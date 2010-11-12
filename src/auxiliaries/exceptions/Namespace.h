/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        Namespace.h

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 10 19

 ----- Description: Namespace file for cedar::aux::exc.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_EXC_NAMESPACE_H
#define CEDAR_AUX_EXC_NAMESPACE_H

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "src/Namespace.h"

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>


namespace cedar
{
  namespace aux
  {
    //!@brief Namespace for all exception classes.
    namespace exc
    {
      class ExceptionBase;
      typedef boost::shared_ptr<ExceptionBase> ExceptionBasePtr;

      class NullPointerException;
      typedef boost::shared_ptr<NullPointerException> NullPointerExceptionPtr;

      class IndexOutOfRangeException;
      typedef boost::shared_ptr<IndexOutOfRangeException> IndexOutOfRangePtr;
    }
  }
}

#endif // CEDAR_AUX_EXC_NAMESPACE_H
