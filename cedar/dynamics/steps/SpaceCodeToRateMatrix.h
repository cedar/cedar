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

    File:        SpaceCodeToRateMatrix.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2014 06 11

    Description: Header file for the class cedar::dyn::SpaceCodeToRateMatrix.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DYN_SPACE_CODE_TO_RATE_MATRIX_H
#define CEDAR_DYN_SPACE_CODE_TO_RATE_MATRIX_H

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/math/tools.h"

// FORWARD DECLARATIONS
#include "cedar/dynamics/steps/SpaceCodeToRateMatrix.fwd.h"

// SYSTEM INCLUDES
#include <vector>

// SYSTEM INCLUDES


/*!@brief Transforms a space-feature coded activation matrix to a rate-coded matrix.
 *
 * @todo Put the equation here, and explain the concept in more detail.
 */
class cedar::dyn::SpaceCodeToRateMatrix : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  SpaceCodeToRateMatrix();

  //!@brief Destructor
  virtual ~SpaceCodeToRateMatrix();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet
public slots:
  //!@brief This slot is connected to the valueChanged() event of the limit parameters.
  void limitsChanged();
  //!@brief This slot takes care of size changes.
  void outputSizesChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Reacts to a change in the input connection.
  void inputConnectionChanged(const std::string& inputName);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Updates the output matrix.
  void compute(const cedar::proc::Arguments& arguments);

  inline void setLowerLimit(double limit)
  {
    _mLowerLimit->setValue(limit);
  }

  inline void setUpperLimit(double limit)
  {
    _mUpperLimit->setValue(limit);
  }

  inline double getLowerLimit()
  {
    return _mLowerLimit->getValue();
  }

  inline double getUpperLimit()
  {
    return _mUpperLimit->getValue();
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief MatrixData representing the input. Storing it like this saves time during computation.
  cedar::aux::ConstMatDataPtr mInput;

  //!@brief The data containing the output.
  cedar::aux::MatDataPtr mOutput;

  //!@brief The weight matrix.
  cedar::aux::MatDataPtr mWeights;
private:
  double mInterval;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief the lower limit of the mapped interval
  cedar::aux::DoubleParameterPtr _mLowerLimit;

  //!@brief the upper limit of the mapped interval
  cedar::aux::DoubleParameterPtr _mUpperLimit;

private:
  // none yet

}; // class cedar::dyn::SpaceCodeToRateMatrix

#endif // CEDAR_DYN_SPACE_CODE_TO_RATE_MATRIX_H

