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

    File:        BaseSignal.h

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2017 12 04

    Description: Header file for the class cedar::proc::sources::BaseSignal.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_BASE_SIGNAL_H
#define CEDAR_PROC_STEPS_BASE_SIGNAL_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include <cedar/processing/Step.h>
#include <cedar/processing/InputSlotHelper.h>
#include <cedar/auxiliaries/MatData.h>
#include <cedar/auxiliaries/DoubleParameter.h>

// FORWARD DECLARATIONS
#include "cedar/processing/sources/BaseSignal.fwd.h"

// SYSTEM INCLUDES


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::sources::BaseSignal : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  BaseSignal();
  virtual ~BaseSignal();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void reset();
  void onStart();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void compute(const cedar::proc::Arguments& arguments);
  void recompute();

  void startClock();

  virtual double calculateSignal(double t, double period, double amplitude, double phase, double verticaloffset) = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief The output data.
  cedar::aux::MatDataPtr mOutput;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  cedar::unit::Time mStartTime;

  cedar::aux::DoubleParameterPtr mPeriod;
  cedar::aux::DoubleParameterPtr mAmplitude;
  cedar::aux::DoubleParameterPtr mPhaseShift;
  cedar::aux::DoubleParameterPtr mVerticalOffset;

private:

}; // class cedar::proc::sources::BaseSignal

#endif // CEDAR_PROC_STEPS_BASE_SIGNAL_H

