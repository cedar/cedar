/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        Namespace.h

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 11 11

 ----- Description: Namespace file for cedar::aux::math.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_MATH_NAMESPACE_H
#define CEDAR_AUX_MATH_NAMESPACE_H

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "src/Namespace.h"

// SYSTEM INCLUDES


namespace cedar
{
  namespace aux
  {
    /*!@brief Namespace for all math classes. */
    namespace math
    {
      template <typename T_MIN, typename T_MAX> struct Limits;
    }
  }
}

#endif // CEDAR_AUX_MATH_NAMESPACE_H
