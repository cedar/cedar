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

    File:        ConvertToValueRange.h

    Maintainer:  Jan Tekülve
    Email:       jan.tekuelve@ini.rub.de
    Date:        2022 08 02

    Description: Header file for the class cedar::proc::steps::ConvertToValueRange.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_CONVERT_TO_VALUE_RANGE_H
#define CEDAR_PROC_STEPS_CONVERT_TO_VALUE_RANGE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/DoubleParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/steps/ConvertToValueRange.fwd.h"

// SYSTEM INCLUDES


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::steps::ConvertToValueRange : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ConvertToValueRange();

  //!@brief Destructor
  virtual ~ConvertToValueRange();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

public slots:
    //!@brief This slot is connected to the valueChanged() event of the gain value parameter.
    void rangeChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief the amplitude of the Gaussian function
  cedar::aux::DoubleParameterPtr _mInputLowerBound;
  cedar::aux::DoubleParameterPtr _mInputUpperBound;
  cedar::aux::DoubleParameterPtr _mOutputLowerBound;
  cedar::aux::DoubleParameterPtr _mOutputUpperBound;

private:
  // none yet

}; // class cedar::proc::steps::ConvertToValueRange

#endif // CEDAR_PROC_STEPS_CONVERT_TO_VALUE_RANGE_H

