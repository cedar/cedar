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

    File:        AtTime.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 03 19

    Description: Header file for the class cedar::proc::experiment::AtTime.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_CONDITION_AT_TIME_H
#define CEDAR_PROC_EXPERIMENT_CONDITION_AT_TIME_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/condition/Condition.h"
#include "cedar/auxiliaries/TimeParameter.h"
// FORWARD DECLARATIONS
#include "cedar/processing/experiment/condition/AtTime.fwd.h"

// SYSTEM INCLUDES


/*!@brief Checks if a the current trial time is greater than the time provided by this condition
 *
 *        This condition is only activated once after the trial time reaches the value
 */
class cedar::proc::experiment::condition::AtTime : public cedar::proc::experiment::condition::Condition
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  AtTime();

  //!@brief Destructor
  virtual ~AtTime();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //@brief Checks if a the current trial time is greater than the time provided by this condition
  bool check() const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //!@brief The time that has to been reached by the trial time
  cedar::aux::TimeParameterPtr _mTime;

}; // class cedar::proc::experiment::AtTime

#endif // CEDAR_PROC_EXPERIMENT_CONDITION_AT_TIME_H

