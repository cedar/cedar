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

    File:        SpaceToRateCode.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2012 07 09

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DYN_SPACE_TO_RATE_CODE_H
#define CEDAR_DYN_SPACE_TO_RATE_CODE_H

// CEDAR INCLUDES
#include "cedar/dynamics/namespace.h"
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/DoubleParameter.h"

// SYSTEM INCLUDES


/*!@brief
 */
class cedar::dyn::SpaceToRateCode : public cedar::proc::Step
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
  SpaceToRateCode();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief
  inline double getLowerLimit() const
  {
    return this->_mLowerLimit->getValue();
  }

  //!@brief
  inline void setLowerLimit(double lowerLimit)
  {
    this->_mLowerLimit->setValue(lowerLimit);
  }

  //!@brief
  inline double getUpperLimit() const
  {
    return this->_mUpperLimit->getValue();
  }

  //!@brief
  inline void setUpperLimit(double upperLimit)
  {
    this->_mUpperLimit->setValue(upperLimit);
  }

  inline double getTau() const
  {
    return this->_mTau->getValue();
  }

  //!@brief
  inline void setTau(double tau)
  {
    this->_mTau->setValue(tau);
  }
public slots:
  //!@brief This slot is connected to the valueChanged() event of the limit parameters.
  void limitsChanged();

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

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief MatrixData representing the input. Storing it like this saves time during computation.
  cedar::aux::ConstMatDataPtr mInput;

  //!@brief The data containing the output.
  cedar::aux::MatDataPtr mOutput;
private:
  cv::Mat mRamp;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief
  cedar::aux::DoubleParameterPtr _mLowerLimit;
  cedar::aux::DoubleParameterPtr _mUpperLimit;
  cedar::aux::DoubleParameterPtr _mTau;

private:

}; // class cedar::dyn::SpaceToRateCode

#endif // CEDAR_DYN_SPACE_TO_RATE_CODE_H
