/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        Limits.h

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 11 11

 ----- Description: Header for the @em cedar::aux::math::Limits struct.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_MATH_LIMITS_H
#define CEDAR_AUX_MATH_LIMITS_H

// LOCAL INCLUDES
#include "namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*!@brief Abstract description of the struct.
 *
 * More detailed description of the struct.
 */
//template <typename T>
struct cedar::aux::math::Limits
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  //!@brief The standard constructor
  Limits() {};
  //!@brief Constructor that takes a minimum and maximum value
//  Limits(const T& newMin, const T& newMax) : min(newMin), max(newMax) {};
  Limits(const double& newMin, const double& newMax) : min(newMin), max(newMax) {};

  //!@brief Copy constructor
//  template <typename U>
//  Limits(const Limits<U> &otherLimits) : min(otherLimits.min), max(otherLimits.max) {};
  Limits(const Limits &otherLimits) : min(otherLimits.min), max(otherLimits.max) {};

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
  //! lower limit
//  T min;
  double min;
  //! upper limit
//  T max;
  double max;

}; // class cedar::aux::math::Limits

//template struct cedar::aux::math::Limits<double>;
//template struct cedar::aux::math::Limits<int>;
//template struct cedar::aux::math::Limits<unsigned int>;

#endif // CEDAR_AUX_MATH_LIMITS_H
