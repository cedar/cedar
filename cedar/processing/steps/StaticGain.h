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

    File:        StaticGain.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 10 28

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_STATIC_GAIN_H
#define CEDAR_PROC_STEPS_STATIC_GAIN_H

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/DoubleParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/steps/StaticGain.fwd.h"

// SYSTEM INCLUDES


/*!@brief   This is a static gain step that allows multiplying a matrix input with a factor.
 *
 *          This step implements the cedar::proc::Step interface and allows a user to easily multiply any matrix with a
 *          gain factor. This step is called a static gain step because the gain factor is determined by a parameter,
 *          rather than being an input that might change over time.
 *
 * @remarks This step declares the following interface:
 *          input - any matrix data
 *          output - the output, i.e., input multiplied by gainFactor
 *
 *          Parameters of the step are:
 *          gainFactor - the gain factor.
 */
class cedar::proc::steps::StaticGain : public cedar::proc::Step
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
  StaticGain();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Returns the current gain factor.
  inline double getGainFactor() const
  {
    return this->_mGainFactor->getValue();
  }

  //!@brief Sets the current gain factor.
  inline void setGainFactor(double gainFactor)
  {
    this->_mGainFactor->setValue(gainFactor);
  }

public slots:
  //!@brief This slot is connected to the valueChanged() event of the gain value parameter.
  void gainChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Reacts to a change in the input connection.
  void inputConnectionChanged(const std::string& inputName);

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

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief The factor by which the input is multiplied.
  cedar::aux::DoubleParameterPtr _mGainFactor;

private:

}; // class cedar::proc::steps::StaticGain

#endif // CEDAR_PROC_STEPS_STATIC_GAIN_H
