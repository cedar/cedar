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
#include "cedar/dynamics/Dynamics.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/TimeParameter.h"

// FORWARD DECLARATIONS
#include "cedar/dynamics/steps/SpaceToRateCode.fwd.h"

// SYSTEM INCLUDES


/*!@brief A step that converts space code to rate code.
 */
class cedar::dyn::SpaceToRateCode : public cedar::dyn::Dynamics
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
  //!@brief convenience function for accessing the lower limit
  inline double getLowerLimit() const
  {
    return this->_mLowerLimit->getValue();
  }

  //!@brief convenience function for setting the lower limit
  inline void setLowerLimit(double lowerLimit)
  {
    this->_mLowerLimit->setValue(lowerLimit);
  }

  //!@brief convenience function for accessing the upper limit
  inline double getUpperLimit() const
  {
    return this->_mUpperLimit->getValue();
  }

  //!@brief convenience function for setting the upper limit
  inline void setUpperLimit(double upperLimit)
  {
    this->_mUpperLimit->setValue(upperLimit);
  }

  //!@brief convenience function for accessing tau
  inline const cedar::unit::Time& getTau() const
  {
    return this->_mTau->getValue();
  }

  //!@brief convenience function for setting tau
  inline void setTau(const cedar::unit::Time& tau)
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
  void eulerStep(const cedar::unit::Time& time);

  //!@brief Resets the step.
  void reset();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief MatrixData representing the input. Storing it like this saves time during computation.
  cedar::aux::ConstMatDataPtr mInput;

  //!@brief The data containing the output.
  cedar::aux::MatDataPtr mOutput;

  //!@brief Contains the current fixed point (mainly used for visualization)
  cedar::aux::MatDataPtr mFixPoint;
private:
  cv::Mat mRamp;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief the relaxation parameter of the dynamical system
  cedar::aux::TimeParameterPtr _mTau;

  //!@brief the lower limit of the mapped interval
  cedar::aux::DoubleParameterPtr _mLowerLimit;
  
  //!@brief the upper limit of the mapped interval
  cedar::aux::DoubleParameterPtr _mUpperLimit;


private:

}; // class cedar::dyn::SpaceToRateCode

#endif // CEDAR_DYN_SPACE_TO_RATE_CODE_H
