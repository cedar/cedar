/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik

 ----- File:        tools.h

 ----- Author:      INI robotics group, maintained by HR
 ------Email:       ar@ini.rub.de
 ----- Date:        2010 11 11

 ----- Description: math tools

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/
#ifndef CEDAR_AUX_MATH_TOOLS_H
#define CEDAR_AUX_MATH_TOOLS_H

// LOCAL INCLUDES
#include "namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <cv.h>

/*
 *  this file has very generic math functions that do not fall under some other category
 *  more specific functions should be put in specialized files
 *  some precompiler macros are also defined here
 */

/* epsilon surrounding for near zero values */
#ifndef EQN_EPS
#define EQN_EPS 1e-9
#endif

#ifndef IsZero
#define IsZero(x) ((x) > -EQN_EPS && (x) < EQN_EPS)
#endif
namespace cedar
{
  namespace aux
  {
    namespace math
    {
      //! normalizes an angle to the representative interval (-pi, pi]
      double normalizeAngle(double value);

      //! writes the matrix into the shell properly organized by columns and rows
      //!\todo move write(cv::Mat) to aux::utilities
      //!\todo rework (template for copy & paste code)
      //!\todo add log file capability
      void write(cv::Mat matrix);
    };
  };
};
#endif  // CEDAR_AUX_MATH_TOOLS_H
