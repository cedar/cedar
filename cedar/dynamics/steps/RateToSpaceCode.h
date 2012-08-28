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

    File:        RateToSpaceCode.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2012 07 16

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DYN_RATE_TO_SPACE_CODE_H
#define CEDAR_DYN_RATE_TO_SPACE_CODE_H

// CEDAR INCLUDES
#include "cedar/dynamics/namespace.h"
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"

// SYSTEM INCLUDES


/*!@brief
 */
class cedar::dyn::RateToSpaceCode : public cedar::proc::Step
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
  RateToSpaceCode();

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
  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief
  cedar::aux::DoubleVectorParameterPtr _mLowerLimits;
  cedar::aux::DoubleVectorParameterPtr _mUpperLimits;
  cedar::aux::UIntVectorParameterPtr _mOutputSizes;
  cedar::aux::DoubleParameterPtr _mAmplitude;
  cedar::aux::DoubleVectorParameterPtr _mSigmas;
  cedar::aux::BoolParameterPtr _mIsCyclic;

private:

}; // class cedar::dyn::RateToSpaceCode

#endif // CEDAR_DYN_RATE_TO_SPACE_CODE_H
