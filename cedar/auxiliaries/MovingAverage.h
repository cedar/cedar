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

    File:        MovingAverage.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 06 14

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_MOVING_AVERAGE_H
#define CEDAR_AUX_MOVING_AVERAGE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/assert.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MovingAverage.fwd.h"

// SYSTEM INCLUDES
#include <deque>
#include <iostream>


namespace cedar
{
  namespace aux
  {
    /*!@brief Holds a return type.
     *  By default, the return type is the same as the template argument of the class.
     */
    template <typename U>
    struct ReturnTypeHelper
    {
      //! the return type held by this class
      typedef U return_type;
    }; // class cedar::aux::ReturnTypeHelper

    /*!@brief Holds a return type.
     */
    template <>
    struct ReturnTypeHelper<int>
    {
      //! the return type held by this class
      typedef double return_type;
    }; // class cedar::aux::ReturnTypeHelper
  }
}

/*!@brief Calculates a moving average of a set of values.
 *
 *        This is a structure that maintains a limited list of numbers and allows to quickly calculate their mean value
 *        and sum.
 *
 *        Calculation of the mean value is done in constant time.
 */
template <typename T>
class cedar::aux::MovingAverage
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Type of the elements held in the average.
  typedef T ElementType;

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief The stream operator.
   */
  friend std::ostream& operator<<(std::ostream& stream, const cedar::aux::MovingAverage<T>& average)
  {
    stream << "{cedar::aux::MovingAverage " << &average << "; buffer: [";
    for (size_t i = 0; i < average.size(); ++i)
    {
      if (i > 0)
      {
        stream << ", ";
      }
      stream << average.at(i);
    }
    stream << "]";
    stream << ", count: " << average.size();
    if (average.size() > 0)
    {
      stream << ", element sum: " << average.getSum();
      stream << ", average value: " << average.getAverage();
    }
    stream << "}";
    return stream;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  MovingAverage(size_t maxNumSamples)
  :
  mMaxNumElements(maxNumSamples),
  mElementSum(0)
  {
    CEDAR_ASSERT(maxNumSamples > 0);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Gets the maximum number of elements.
   */
  size_t getMaximumNumberOfElements() const
  {
    return this->mMaxNumElements;
  }

  /*!@brief Sets the maximum number of allowed elements
   */
  void setMaximumNumberOfElements(size_t maxNumSamples)
  {
    this->mMaxNumElements = maxNumSamples;

    this->removeExcess();
  }

  /*!@brief Appends a new value to the average.
   */
  void append(const ElementType& value)
  {
    this->removeExcess();

    this->mElementSum += value;
    this->mBuffer.push_back(value);
  }

  /*!@brief Removes the last element in the buffer.
   */
  void removeLast()
  {
    this->mElementSum -= this->mBuffer.front();
    this->mBuffer.pop_front();
  }

  /*!@brief Returns the average value of the elements in the buffer.
   * This method uses template specialization for integer types in oder to return floating point
   * values instead of ElementType. See below the class for the implementation of the specializations.
   */
  typename cedar::aux::ReturnTypeHelper<ElementType>::return_type getAverage() const
  {
    return this->getSum() / static_cast<double>(this->size());
  }

  /*!@brief Returns the sum of the elements in the buffer.
   */
  ElementType getSum() const
  {
    CEDAR_ASSERT(this->size() > 0);

    return this->mElementSum;
  }

  /*!@brief Returns the newest element in the buffer.
   */
  ElementType getNewest() const
  {
    CEDAR_ASSERT(this->size() > 0);

    return this->mBuffer.back();
  }

  /*!@brief Returns the element at the given index.
   */
  const T& at(size_t index) const
  {
    return this->mBuffer.at(index);
  }

  /*!@brief Returns the number of elements in the buffer.
   */
  size_t size() const
  {
    return this->mBuffer.size();
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Removes as many elements as necessary to make the buffer size fit the set maximum number of elements.
   */
  void removeExcess()
  {
    while (this->mBuffer.size() >= this->mMaxNumElements)
    {
      this->removeLast();
    }
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! The buffer containing the elements.
  std::deque<ElementType> mBuffer;

  //! The maximum number of elements to be stored.
  size_t mMaxNumElements;

  //! The current sum of all elements in the buffer.
  ElementType mElementSum;

}; // class cedar::aux::MovingAverage

#endif // CEDAR_AUX_MOVING_AVERAGE_H

