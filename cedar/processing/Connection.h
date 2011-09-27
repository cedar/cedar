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

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 28

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_CONNECTION_H
#define CEDAR_PROC_CONNECTION_H

// LOCAL INCLUDES
#include "processing/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::proc::Connection
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Connection(
              cedar::proc::StepPtr source,
              const std::string& sourceName,
              cedar::proc::StepPtr target,
              const std::string& targetName
            );
  Connection(
              cedar::proc::TriggerPtr source,
              cedar::proc::StepPtr target
            );
  Connection(
              cedar::proc::TriggerPtr source,
              cedar::proc::TriggerPtr target
            );
  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  bool contains(cedar::proc::StepPtr step);
  bool contains(cedar::proc::TriggerPtr trigger);
  void deleteConnection();
  
  cedar::proc::StepPtr getSource();
  cedar::proc::ConstStepPtr getSource() const;
  const std::string& getSourceName() const;
  cedar::proc::StepPtr getTarget();
  cedar::proc::ConstStepPtr getTarget() const;
  const std::string& getTargetName() const;

  cedar::proc::ConstTriggerPtr getSourceTrigger() const;
  cedar::proc::ConstTriggerPtr getTargetTrigger() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  cedar::proc::TriggerPtr mTrigger;
  cedar::proc::TriggerPtr mTargetTrigger;
  cedar::proc::StepPtr mSource;
  std::string mSourceName;
  cedar::proc::StepPtr mTarget;
  std::string mTargetName;
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::Connection

#endif // CEDAR_PROC_CONNECTION_H

