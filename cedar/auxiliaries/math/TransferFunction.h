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

    File:        TransferFunction.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2012 07 19

    Description: TransferFunction functions

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_MATH_TRANSFER_FUNCTION_H
#define CEDAR_AUX_MATH_TRANSFER_FUNCTION_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/math/TransferFunction.fwd.h"

// SYSTEM INCLUDES

/*!@brief Basic interface for all TransferFunction functions.
 */
class cedar::aux::math::TransferFunction : public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  TransferFunction()
  :
  cedar::aux::Configurable()
  {
  }

  //!@brief Destructor
  virtual ~TransferFunction();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief this function calculates the TransferFunction function for a given double value.
   * All inherited classes have to implement this function.
   */
  virtual double compute(double value) const = 0;

  /*!@brief this function calculates the TransferFunction function for a given float value.
   * Included for backward-compatibility
   */
  virtual float compute(float value) const;

  /*!@brief this function calculates the TransferFunction function for an n-dimensional matrix.
   *
   * @todo write a non-templated function, which checks the type flag of cv::Mat and calls the correct templated compute
   * function
   *
   * @todo Make a virtual version of this function that can be overloaded for faster TransferFunction calculation.
   */
  template<typename T>
  cv::Mat compute(const cv::Mat& values) const
  {
    cv::Mat result = values.clone();
    cv::MatConstIterator_<T> iter_src = values.begin<T>();
    cv::MatIterator_<T> iter_dest = result.begin<T>();
    auto end = values.end<T>();
    for ( ; iter_src != end; ++iter_src, ++iter_dest)
    {
      *iter_dest = static_cast<T>(compute(static_cast<double>(*iter_src)));
    }
    return result;
  }

  virtual cv::Mat compute(const cv::Mat& values) const
  {
    cv::Mat result = values.clone();
    if (values.type() == CV_32F)
    {
      cv::MatConstIterator_<float> iter_src = values.begin<float>();
      cv::MatIterator_<float> iter_dest = result.begin<float>();
      auto end = values.end<float>();
      for ( ; iter_src != end; ++iter_src, ++iter_dest)
      {
        *iter_dest = static_cast<float>(compute(static_cast<double>(*iter_src)));
      }
    }
    else if (values.type() == CV_64F)
    {
      cv::MatConstIterator_<double> iter_src = values.begin<double>();
      cv::MatIterator_<double> iter_dest = result.begin<double>();
      auto end = values.end<double>();
      for ( ; iter_src != end; ++iter_src, ++iter_dest)
      {
        *iter_dest = compute(*iter_src);
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
  // none yet
private:
  // none yet
};

#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/Singleton.h"

namespace cedar
{
  namespace aux
  {
    namespace math
    {
      //!@brief The manager of all sigmoind instances
      typedef cedar::aux::FactoryManager<TransferFunctionPtr> TransferFunctionManager;

      //!@brief The singleton object of the TransferFunctionFactory.
      typedef cedar::aux::Singleton<TransferFunctionManager> TransferFunctionManagerSingleton;
    }
  }
}

CEDAR_AUX_EXPORT_SINGLETON(cedar::aux::math::TransferFunctionManager);

#endif  // CEDAR_AUX_MATH_TRANSFER_FUNCTION_H
