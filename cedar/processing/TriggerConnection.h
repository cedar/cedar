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

    File:        TriggerConnection.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 11 21

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_TRIGGER_CONNECTION_H
#define CEDAR_PROC_TRIGGER_CONNECTION_H

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/processing/Trigger.fwd.h"
#include "cedar/processing/Triggerable.fwd.h"
#include "cedar/processing/TriggerConnection.fwd.h"

// SYSTEM INCLUDES


/*!@brief A connection from a cedar::proc::Trigger to a cedar::proc::Triggerable.
 */
class cedar::proc::TriggerConnection
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  TriggerConnection(cedar::proc::TriggerPtr source, cedar::proc::TriggerablePtr target);
  //!@brief Destructor
  ~TriggerConnection();
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief test if the given source and target are the same for this connection (i.e. are source and target connected
  // via this TriggerConnection)
  bool equals(cedar::proc::TriggerPtr source, cedar::proc::TriggerablePtr target) const;

  //!@brief get the source of this connection
  cedar::proc::ConstTriggerPtr getSourceTrigger() const;

  //!@brief get the target of this connection
  cedar::proc::ConstTriggerablePtr getTarget() const;
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
  //!@brief source of this connection
  cedar::proc::TriggerWeakPtr mSourceTrigger;
  //!@brief target of this connection
  cedar::proc::TriggerableWeakPtr mTarget;
private:
  // none yet
}; // class cedar::proc::TriggerConnection

#endif // CEDAR_PROC_TRIGGER_CONNECTION_H

