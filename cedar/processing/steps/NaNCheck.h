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

    File:        NaNCheck.h

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2017 12 04

    Description: Header file for the class cedar::proc::steps::NaNCheck.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_NOOP_H
#define CEDAR_PROC_STEPS_NOOP_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include <cedar/processing/Step.h>
#include <cedar/processing/InputSlotHelper.h>
#include <cedar/auxiliaries/MatData.h>
#include <cedar/auxiliaries/BoolParameter.h>

// FORWARD DECLARATIONS
#include "cedar/processing/steps/NaNCheck.fwd.h"

// SYSTEM INCLUDES


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::steps::NaNCheck : public cedar::proc::Step
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
  NaNCheck();

  bool getCaughtNaN() const;
  void setCaughtNaN(bool b);
  bool getCaughtInf() const;
  void setCaughtInf(bool b);
  bool getCaughtEmpty() const;
  void setCaughtEmpty(bool b);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

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

  void compute(const cedar::proc::Arguments& arguments);
  void recompute();
  void reset();
  void onStart();

  void resetStates();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief MatrixData representing the input. Storing it like this saves time during computation.
  cedar::aux::ConstMatDataPtr mInput;

  //!@brief The output data.
  cedar::aux::MatDataPtr mOutput;

  bool mCaughtNaN;
  bool mCaughtInf;
  bool mCaughtEmpty;
  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::aux::BoolParameterPtr mCheckForNaN;
  cedar::aux::BoolParameterPtr mCheckForInf;
  cedar::aux::BoolParameterPtr mCheckForEmpty;
  cedar::aux::BoolParameterPtr mAutoReset;

public:
signals:
  void caughtNaNChangedSignal();

}; // class cedar::proc::steps::NaNCheck

#endif // CEDAR_PROC_STEPS_NOOP_H

