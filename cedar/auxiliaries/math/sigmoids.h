/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik

 ----- File:        sigmoids.h

 ----- Author:      INI robotics group, maintained by HR
 ------Email:       ar@ini.rub.de
 ----- Date:        2010 11 11

 ----- Description: sigmoid functions

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/
#ifndef CEDAR_AUX_MATH_SIGMOIDS_H
#define CEDAR_AUX_MATH_SIGMOIDS_H

// LOCAL INCLUDES
#include "namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <math.h>
#include <vector>

/*
 *  different sigmoid functions for different data formats
 *  default is the exponential sigmoid, just called sigmoid()
 *  other kinds of sigmoids have more specific names
 */
namespace cedar
{
  namespace aux
  {
    namespace math
    {
      //! an exponential-based sigmoid for a single double value
      double sigmoid(const double x, const double beta, const double threshold = 0);

      //! an abs-based sigmoid for a single double value
      double sigmoidAbs(const double x, const double beta, const double threshold = 0);

      //! a sigmoid for a vector of doubles
      std::vector<double> sigmoid(const std::vector<double>& x, const double beta, const double threshold = 0);
  
      //! a sigmoid that rises smoothly in an interval and is exactly zero resp. one outside that interval
      double sigmoidInterval(const double value, const double t1, const double t2, const bool decreasing = false);
    };
  };
};
#endif  // CEDAR_AUX_MATH_SIG_NAMESPACE_H
