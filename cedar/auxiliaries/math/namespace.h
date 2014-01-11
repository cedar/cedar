/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

    This file is part of cedar.

    cedar is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    cedar is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with cedar. If not, see <http://www.gnu.org/licenses/>.

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        namespace.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 11 11

    Description: Namespace file for cedar::aux::math.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_MATH_NAMESPACE_H
#define CEDAR_AUX_MATH_NAMESPACE_H

// CEDAR INCLUDES
#include "cedar/defines.h"
#include "cedar/auxiliaries/Factory.h"
#include "cedar/units/Length.h"
#include "cedar/units/Time.h"
#include "cedar/units/Velocity.h"
#include "cedar/units/Acceleration.h"
#include "cedar/units/PlaneAngle.h"
#include "cedar/units/AngularVelocity.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/smart_ptr.hpp>
#endif


namespace cedar
{
  namespace aux
  {
    /*!@brief Namespace for all math classes. */
    namespace math
    {
      //!@brief a templated class for representing limits (i.e. an interval) of some type
      template <typename T> class Limits;

      //!@cond SKIPPED_DOCUMENTATION
      CEDAR_DECLARE_AUX_CLASS(Sigmoid);
      CEDAR_DECLARE_AUX_CLASS(TransferFunction);
      CEDAR_DECLARE_AUX_CLASS(TransferFunctionDeclaration);
      //!@endcond

      //!@brief a templated declaration for sigmoid function implementation
      template <class DerivedClass> class TransferFunctionDeclarationT;
      //!@brief a template specialization for sigmoid factories
      typedef cedar::aux::Factory<TransferFunctionPtr> TransferFunctionFactory;

      template <class T> class LimitsParameter;
      //!@brief a template specialization for double-based limits
      typedef LimitsParameter<double> DoubleLimitsParameter;
      //!@brief a template specialization for int-based limits
      typedef LimitsParameter<int> IntLimitsParameter;
      //!@brief a template specialization for unsigned int-based limits
      typedef LimitsParameter<unsigned int> UIntLimitsParameter;

      typedef LimitsParameter<cedar::unit::Length> LengthLimitsParameter;
      typedef LimitsParameter<cedar::unit::Time> TimeLimitsParameter;
      typedef LimitsParameter<cedar::unit::Velocity> VelocityLimitsParameter;
      typedef LimitsParameter<cedar::unit::Acceleration> AccelerationLimitsParameter;
      typedef LimitsParameter<cedar::unit::PlaneAngle> PlaneAngleLimitsParameter;
      typedef LimitsParameter<cedar::unit::AngularVelocity> AngularVelocityLimitsParameter;

      //!@cond SKIPPED_DOCUMENTATION
      CEDAR_GENERATE_POINTER_TYPES(TransferFunctionFactory);
      CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(DoubleLimitsParameter);
      CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(IntLimitsParameter);
      CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(UIntLimitsParameter);
      CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(LengthLimitsParameter);
      CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(TimeLimitsParameter);
      CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(VelocityLimitsParameter);
      CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(AccelerationLimitsParameter);
      CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(PlaneAngleLimitsParameter);
      CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(AngularVelocityLimitsParameter);
      //!@endcond
    }
  }
}

#endif // CEDAR_AUX_MATH_NAMESPACE_H
