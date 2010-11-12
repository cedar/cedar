/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        Limits.h

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 11 11

 ----- Description: Header for the \em cedar::aux::math::Limits struct.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_MATH_LIMITS_H
#define CEDAR_AUX_MATH_LIMITS_H

// LOCAL INCLUDES
#include "Namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*!@brief Abstract description of the struct.
 *
 * More detailed description of the struct.
 */
template <typename T_MIN, typename T_MAX>
struct cedar::aux::math::Limits
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  //!@brief The standard constructor
  Limits() {};
  //!@brief Constructor that takes a minimum and maximum value
  Limits(const T_MIN& newMin, const T_MAX& newMax) : min(newMin), max(newMax) {};
  
  //!@brief Copy constructor
  template <typename U, typename V>
  Limits(const Limits<U, V> &otherLimits) : min(otherLimits.min), max(otherLimits.max) {};

  //--------------------------------------------------------------------------------------------------------------------
  // methods
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
  //! minimum limit
  T_MIN min;
  //! maximum limit
  T_MAX max;

}; // class cedar::aux::math::Limits

template struct cedar::aux::math::Limits<double, double>;
template struct cedar::aux::math::Limits<int, int>;
template struct cedar::aux::math::Limits<unsigned int, unsigned int>;

#endif // CEDAR_AUX_MATH_LIMITS_H
