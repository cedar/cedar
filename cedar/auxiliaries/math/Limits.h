/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        Limits.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 11 11

    Description: Header for the @em cedar::aux::math::Limits class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_MATH_LIMITS_H
#define CEDAR_AUX_MATH_LIMITS_H

#ifndef NOMINMAX
#define NOMINMAX // to avoid Windows issues
#endif

// CEDAR INCLUDES
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/units/Acceleration.h"
#include "cedar/units/AngularVelocity.h"
#include "cedar/units/Frequency.h"
#include "cedar/units/Length.h"
#include "cedar/units/PlaneAngle.h"
#include "cedar/units/Time.h"
#include "cedar/units/Velocity.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/math/Limits.fwd.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/numeric/conversion/bounds.hpp>
  #include <boost/units/base_dimension.hpp>
  #include <boost/units/derived_dimension.hpp>
  #include <boost/units/get_dimension.hpp>
  #include <boost/units/quantity.hpp>
  #include <boost/units/io.hpp>
#endif
#include <limits>
#include <iostream>
#include <vector>

namespace cedar
{
  namespace aux
  {
    namespace math
    {
      /*! Helper class for dealing with units. Via template-specialization, this class can be used to inform
       *  the limits about the default units for certain quantities.
       */
      template<class Unit>
      class UnitHelper
      {
        public:
        //! Returns the default quantity for the given unit.
        static inline Unit unit()
        {
          BOOST_STATIC_ASSERT(sizeof(Unit) == 0);
        }
      };

      //! Template specialization of cedar::aux::math::UnitHelper for cedar::unit::Time.
      template<>
      class UnitHelper<cedar::unit::Time>
      {
      public:
        //! Returns the default quantity for times.
        static inline cedar::unit::Time unit()
        {
          return cedar::unit::DEFAULT_TIME_UNIT;
        }
      };

      //! Template specialization of cedar::aux::math::UnitHelper for cedar::unit::Length.
      template<>
      class UnitHelper<cedar::unit::Length>
      {
      public:
        //! Returns the default quantity for lengths.
        static inline cedar::unit::Length unit()
        {
          return cedar::unit::DEFAULT_LENGTH_UNIT;
        }
      };

      //! Template specialization of cedar::aux::math::UnitHelper for cedar::unit::Velocity.
      template<>
      class UnitHelper<cedar::unit::Velocity>
      {
      public:
        //! Returns the default quantity for velocities.
        static inline cedar::unit::Velocity unit()
        {
          return cedar::unit::DEFAULT_VELOCITY_UNIT;
        }
      };

      //! Template specialization of cedar::aux::math::UnitHelper for cedar::unit::Acceleration.
      template<>
      class UnitHelper<cedar::unit::Acceleration>
      {
      public:
        //! Returns the default quantity for accelerations.
        static inline cedar::unit::Acceleration unit()
        {
          return cedar::unit::DEFAULT_ACCELERATION_UNIT;
        }
      };

      //! Template specialization of cedar::aux::math::UnitHelper for cedar::unit::Frequency.
      template<>
      class UnitHelper<cedar::unit::Frequency>
      {
      public:
        //! Returns the default quantity for Frequency.
        static inline cedar::unit::Frequency unit()
        {
          return cedar::unit::DEFAULT_FREQUENCY_UNIT;
        }
      };

      //! Template specialization of cedar::aux::math::UnitHelper for cedar::unit::AngularVelocity.
      template<>
      class UnitHelper<cedar::unit::AngularVelocity>
      {
      public:
        //! Returns the default quantity for AngularVelocity.
        static inline cedar::unit::AngularVelocity unit()
        {
          return cedar::unit::DEFAULT_ANGULAR_VELOCITY_UNIT;
        }
      };

      //! Template specialization of cedar::aux::math::UnitHelper for cedar::unit::PlaneAngle.
      template<>
      class UnitHelper<cedar::unit::PlaneAngle>
      {
      public:
        //! Returns the default quantity for PlaneAngle.
        static inline cedar::unit::PlaneAngle unit()
        {
          return cedar::unit::DEFAULT_PLANE_ANGLE_UNIT;
        }
      };

      /*!@brief Helper class for getting several limit numbers of numbers of different c++ types.
       *
       *        This is mainly necessary in order to be able to deal with units.
       */
      template <typename T>
      class NumericHelper
      {
      public:
        //! Returns the zero value of the type.
        static inline T zero()
        {
          return static_cast<T>(0);
        }

        //! Returns the smallest negative number representable by the type.
        static inline T smallestNegative()
        {
          if (std::numeric_limits<T>::is_integer)
          {
            return static_cast<T>(-1);
          }
          else
          {
            return static_cast<T>(-boost::numeric::bounds<T>::smallest());
          }
        }

        //! Returns the smallest positive number representable by the type.
        static inline T smallestPositive()
        {
          if (std::numeric_limits<T>::is_integer)
          {
            return static_cast<T>(1);
          }
          else
          {
            return static_cast<T>(boost::numeric::bounds<T>::smallest());
          }
        }
      };

      //! Specialization of cedar::aux::math::NumericHelper for quantities.
      template<class Unit, class T>
      class NumericHelper<boost::units::quantity<Unit, T> >
      {
        typedef boost::units::quantity<Unit, T> Quantity;
        public:
        //! Returns a quantity that is zero.
        static inline Quantity zero()
        {
          return static_cast<double>(0) * UnitHelper<Quantity>::unit();
        }

        //! Returns the smallest negative quantity representable by the data type.
        static inline Quantity smallestNegative()
        {
          return -1 * boost::numeric::bounds<T>::smallest() * UnitHelper<Quantity>::unit();
        }

        //! Returns the smallest positive quantity representable by the data type.
        static inline Quantity smallestPositive()
        {
          return boost::numeric::bounds<T>::smallest() * UnitHelper<Quantity>::unit();
        }
      };
    }
  }
}

/*!@brief Structure representing the limits of an interval.
 */
template <typename T>
class cedar::aux::math::Limits
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  //!@brief formatted output of a Limit to a stream
  friend std::ostream& operator<<(std::ostream& stream, const cedar::aux::math::Limits<T>& limits)
  {
    stream << "[" << limits.getLower() << ", " << limits.getUpper() << "]";
    return stream;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  //!@brief Default constructor
  Limits()
  :
  mLowerLimit(NumericHelper<T>::zero()),
  mUpperLimit(NumericHelper<T>::smallestPositive())
  {
  }

  //!@brief Constructor that takes a minimum and maximum value
  Limits(const T& newLowerLimit, const T& newUpperLimit)
  :
  mLowerLimit(newLowerLimit),
  mUpperLimit(newUpperLimit)
  {
  }
  
  //!@brief Copy constructor
  template <typename U>
  Limits(const Limits<U> &otherLimits)
  :
  mLowerLimit(otherLimits.getLower()),
  mUpperLimit(otherLimits.getUpper())
  {
  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Returns the length of the interval, i.e., upper - lower.
  inline T getLength() const
  {
    CEDAR_DEBUG_ASSERT(this->getUpper() >= this->getLower());

    return this->getUpper() - this->getLower();
  }

  /*!@brief Checks whether a given value is within the limits.
   * @param[in] number the value to be checked
   */
  inline bool includes(const T& number)
  {
    return number >= this->getLower() && number <= this->getUpper();
  }

  //!@brief Returns the lower bound of the limits.
  inline const T& getLower() const
  {
    return this->mLowerLimit;
  }

  //!@brief Sets the lower bound of the limits.
  inline void setLower(const T& value)
  {
    this->mLowerLimit = value;
  }

  //!@brief Returns the lower bound of the limits.
  inline const T& getUpper() const
  {
    return this->mUpperLimit;
  }

  //!@brief Sets the upper bound of the limits.
  inline void setUpper(const T& value)
  {
    this->mUpperLimit = value;
  }

  //!@brief Constructs a limits object that covers only the positive interval (including 0).
  static Limits positiveZero(const T& upper = boost::numeric::bounds<T>::highest())
  {
    return Limits(NumericHelper<T>::zero(), upper);
  }

  //!@brief Constructs a limits object that covers only the positive interval, excluding 0.
  static Limits positive(const T& upper = boost::numeric::bounds<T>::highest())
  {
    return Limits(NumericHelper<T>::smallestPositive(), upper);
  }

  //!@brief Constructs a limits object that covers only the negative interval (including 0).
  static Limits negativeZero(const T& lower = boost::numeric::bounds<T>::lowest())
  {
    return Limits(lower, NumericHelper<T>::zero());
  }

  //!@brief Constructs a limits object that covers only the negative interval, excluding 0.
  static Limits negative(const T& lower = boost::numeric::bounds<T>::lowest())
  {
    return Limits(lower, NumericHelper<T>::smallestNegative());
  }

  //! Returns an interval from lower to infinity, i.e., the interval [lower, inf)
  static Limits fromLower(const T& lower)
  {
    return Limits(lower, boost::numeric::bounds<T>::highest());
  }

  /*! Returns an interval from -infinity to upper, i.e., the interval (-Inf, lower]
   *
   * @remarks For unsigned values, -Inf is replaced by zero.
   */
  static Limits toUpper(const T& upper)
  {
    return Limits(boost::numeric::bounds<T>::lowest(), upper);
  }

  //!@brief Returns a limits object that covers the full range of values.
  static Limits full()
  {
    return Limits(boost::numeric::bounds<T>::lowest(), boost::numeric::bounds<T>::highest());
  }


  /*!@brief Tresholds a value if it is outside of the limits.
   * If the value is below the lower limit, the method will set the value to the lower limit.
   * If the value is above the upper limit, the method will set the value to the upper limit.
   * If the value is within the limits, the method will not change the value.
   * @param[in,out] value the value to be thresholded
   */
  inline const T& limit(const T& value) const
  {
    return limit(value, this->getLower(), this->getUpper());
  }

  /*! Restricts a value so that it lies within the given minimum and maximum.
   *
   * @remarks Minimum and maximum are inclusive, i.e., minimum <= return value <= maximum
   */
  static inline const T& limit(const T& value, const T& minimum, const T& maximum)
  {
    if (value < minimum)
    {
      return minimum;
    }
    else if (value > maximum)
    {
      return maximum;
    }

    return value;
  }

  /*!@brief Tresholds a vector of values against the limits.
   * @param[in,out] values the vector of values to be thresholded
   */
  inline void limit(std::vector<T>& values) const
  {
    for
    (
      typename std::vector<T>::iterator it = values.begin();
      it != values.end();
      ++it
    )
    {
      *it = this->limit(*it);
    }
  }

  //--------------------------------------------------------------------------------------------------------------------
  // operators
  //--------------------------------------------------------------------------------------------------------------------

  //! Compares two limits.
  bool operator!= (const Limits<T>& other) const
  {
    return !(other == *this);
  }

  //! Compares two limits.
  bool operator== (const Limits<T>& other) const
  {
    return (other.getLower() == this->getLower()) && (other.getUpper() == this->getUpper());
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! minimum limit
  T mLowerLimit;
  //! maximum limit
  T mUpperLimit;
}; // class cedar::aux::math::Limits

namespace boost
{
  namespace numeric
  {
    /*!@brief Specialization of boost::numeric::bounds for unit types.
     *
     * This is necessary to determine the different default limits in cedar::aux::math::Limits.
     */
    template<class Unit, class T>
    struct bounds<boost::units::quantity<Unit, T> >
    {
      //! Returns the lowest possible value.
      static boost::units::quantity<Unit, T> lowest()
      {
        return boost::numeric::bounds<T>::lowest() * cedar::aux::math::UnitHelper<boost::units::quantity<Unit, T> >::unit();
      }

      //! Returns the highest possible value.
      static boost::units::quantity<Unit, T> highest()
      {
        return boost::numeric::bounds<T>::highest() * cedar::aux::math::UnitHelper<boost::units::quantity<Unit, T> >::unit();
      }

      //! Returns the smallest, non-zero value.
      static boost::units::quantity<Unit, T> smallest()
      {
        return boost::numeric::bounds<T>::smallest() * cedar::aux::math::UnitHelper<boost::units::quantity<Unit, T> >::unit();
      }
    };
  }
}

#endif // CEDAR_AUX_MATH_LIMITS_H
