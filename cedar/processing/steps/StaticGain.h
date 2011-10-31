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

    File:        StaticGain.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 10 28

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_STATIC_GAIN_H
#define CEDAR_PROC_STEPS_STATIC_GAIN_H

// LOCAL INCLUDES
#include "processing/steps/namespace.h"

// PROJECT INCLUDES
#include "processing/Step.h"
#include "auxiliaries/NumericParameter.h"
#include "auxiliaries/DataTemplate.h"

// SYSTEM INCLUDES


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
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
  //!@brief refreshes the internal matrix containing the input multiplied with the gain factor
  void compute(const cedar::proc::Arguments& arguments);


public slots:
  //!@brief a slot that is triggered if the gain factor is changed
  void gainChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief handle changes of input
  void inputConnectionChanged(const std::string& inputName);

  //!@brief checks the input validity of a connected input
  cedar::proc::DataSlot::VALIDITY determineInputValidity
                                  (
                                    cedar::proc::ConstDataSlotPtr slot,
                                    cedar::aux::DataPtr data
                                  ) const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief MatrixData representing the input.
  cedar::aux::MatDataPtr mInput;

  //!@brief the buffer containing the output
  cedar::aux::MatDataPtr mOutput;
private:

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief the factor by which the input is multiplied
  cedar::aux::DoubleParameterPtr _mGainFactor;

private:

}; // class cedar::proc::steps::StaticGain

#endif // CEDAR_PROC_STEPS_STATIC_GAIN_H

