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

    File:        LogTimeInterval.h

    Maintainer:  Jan Tekülve
    Email:       jan.tekuelve@ini.rub.de
    Date:        2018 04 18

    Description: Header file for the class cedar::dyn::steps::LogTimeInterval.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DYN_STEPS_LOG_TIME_INTERVAL_H
#define CEDAR_DYN_STEPS_LOG_TIME_INTERVAL_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES

#include "cedar/dynamics/Dynamics.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/FileParameter.h"
#include "cedar/auxiliaries/StringParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/dynamics/steps/LogTimeInterval.fwd.h"

// SYSTEM INCLUDES


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::dyn::steps::LogTimeInterval : public cedar::dyn::Dynamics
{
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  LogTimeInterval();

  //!@brief Destructor
  virtual ~LogTimeInterval();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void reset();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  cedar::proc::DataSlot::VALIDITY determineInputValidity(cedar::proc::ConstDataSlotPtr slot, cedar::aux::ConstDataPtr data) const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Reacts to a change in the input connection.
  void inputConnectionChanged(const std::string& inputName);

  //!@brief Updates the output matrix.
  void eulerStep(const cedar::unit::Time& time);

  void print(float loggedTime);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::aux::ConstMatDataPtr _mInputFirst;
  cedar::aux::ConstMatDataPtr _mInputSecond;

  std::string inputFirstName = "first input";
  std::string inputSecondName = "second input";

  float _mElapsedTime;
  bool _mIsLogging;
  bool _mHasLogged;
  std::ofstream _mFile;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::aux::FileParameterPtr _mLogPath;
  cedar::aux::StringParameterPtr _mLogPrefix;
  cedar::aux::DoubleParameterPtr _mLogThreshold;

}; // class cedar::dyn::steps::LogTimeInterval

#endif // CEDAR_dyn_STEPS_LOG_TIME_INTERVAL_H

