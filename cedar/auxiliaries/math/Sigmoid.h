/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        Sigmoid.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 05

    Description: Sigmoid functions

    Credits:

======================================================================================================================*/
#ifndef CEDAR_AUX_MATH_SIGMOID_H
#define CEDAR_AUX_MATH_SIGMOID_H

// LOCAL INCLUDES
#include "auxiliaries/math/namespace.h"
#include "auxiliaries/Configurable.h"
#include "auxiliaries/NumericParameter.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

/*!@brief Abstract description of the sigmoid base class.
 *
 * More detailed description of the sigmoid base class.
 */
class cedar::aux::math::Sigmoid : public Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Sigmoid(double threshold = 0.0)
  :
  mThreshold(new DoubleParameter("threshold", threshold, -1000.0, 1000.0))
  {
    this->registerParameter(mThreshold);
  }

  //!@brief Destructor
  virtual ~Sigmoid()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief this function calculates the sigmoid function for a given double value.
   * All inherited classes have to implement this function.
   */
  virtual double compute(double value) = 0;

  /*!@brief this function calculates the sigmoid function for a given float value.
   * Included for backward-compatibility
   */
  virtual float compute(float value)
  {
    return static_cast<float>(compute(static_cast<double>(value)));
  }

  /*!@brief this function calculates the sigmoid function for a matrix.
   */
  template<typename T>
  cv::Mat compute(const cv::Mat& values)
  {
    cv::Mat result = values.clone();
    for (int col = 0; col < values.cols; col++)
    {
      for (int row = 0; row < values.rows; row++)
      {
        result.at<T>(row,col) = static_cast<T>(compute(static_cast<double>(values.at<T>(row, col))));
      }
    }
    return result;
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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief threshold of the sigmoid
  cedar::aux::DoubleParameterPtr mThreshold;
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet
};


#endif  // CEDAR_AUX_MATH_SIGMOID_H
