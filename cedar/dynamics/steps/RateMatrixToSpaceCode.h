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

    File:        RateMatrixToSpaceCode.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2012 08 28

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DYN_RATE_MATRIX_TO_SPACE_CODE_H
#define CEDAR_DYN_RATE_MATRIX_TO_SPACE_CODE_H

// CEDAR INCLUDES
#include "cedar/dynamics/namespace.h"
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"

// SYSTEM INCLUDES


/*!@brief
 */
class cedar::dyn::RateMatrixToSpaceCode : public cedar::proc::Step
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
  RateMatrixToSpaceCode();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

public slots:
  //!@brief This slot is connected to the valueChanged() event of the limit parameters.
  void limitsChanged();
  void outputSizesChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Reacts to a change in the input connection.
  void inputConnectionChanged(const std::string& inputName);

  //!@brief Determines whether the data item can be connected to the slot.
  cedar::proc::DataSlot::VALIDITY determineInputValidity
                                  (
                                    cedar::proc::ConstDataSlotPtr slot,
                                    cedar::aux::ConstDataPtr data
                                  ) const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Updates the output matrix.
  void compute(const cedar::proc::Arguments& arguments);

  void interpolate();

  inline unsigned int getNumberOfBins()
  {
    return _mNumberOfBins->getValue();
  }

  inline double getLowerLimit()
  {
    return _mLowerLimit->getValue();
  }

  inline double getUpperLimit()
  {
    return _mUpperLimit->getValue();
  }

  inline void setLowerLimit(double limit)
  {
    _mLowerLimit->setValue(limit);
  }

  inline void setUpperLimit(double limit)
  {
    _mUpperLimit->setValue(limit);
  }

  inline int interpolateBin(double value)
  {
    double interpolated = (value - this->getLowerLimit()) / mInterval;
    if (interpolated < 0.0 || interpolated > 1.0)
    {
      return -1;
    }
    else
    {
      return static_cast<int>(round(interpolated * (this->getNumberOfBins() - 1)));
    }
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief MatrixData representing the input. Storing it like this saves time during computation.
  cedar::aux::ConstMatDataPtr mInput;

  //!@brief The data containing the output.
  cedar::aux::MatDataPtr mOutput;
private:
  unsigned int mDimensionality;
  std::vector<double> mInterpolatedCenters;
  double mInterval;
  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief
  cedar::aux::DoubleParameterPtr _mLowerLimit;
  cedar::aux::DoubleParameterPtr _mUpperLimit;
  cedar::aux::UIntParameterPtr _mNumberOfBins;

private:

}; // class cedar::dyn::RateMatrixToSpaceCode

#endif // CEDAR_DYN_RATE_MATRIX_TO_SPACE_CODE_H
