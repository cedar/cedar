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

    File:        Connection.h

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
#include "cedar/processing/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*!@brief Data structure that represents a connection between data slots of processing::Step objects.
 *
 * @todo The various combinations of connections (i.e., slot to slot, trigger to step, trigger to trigger) should be
 *       done with subclassing, i.e., there needs to be a Connection class, and relevant subclasses, e.g.,
 *       DataConnection. Alternatively, data slots, steps and triggers should maybe inherit from a common base class,
 *       e.g., Connectable.
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
  //!@brief create connection from Step to Step (data connection)
  Connection(
              cedar::proc::StepPtr source,
              const std::string& sourceName,
              cedar::proc::StepPtr target,
              const std::string& targetName
            );
  //!@brief create connection from Trigger to Step
  Connection(
              cedar::proc::TriggerPtr source,
              cedar::proc::StepPtr target
            );
  //!@brief create connection from Trigger to Trigger
  Connection(
              cedar::proc::TriggerPtr source,
              cedar::proc::TriggerPtr target
            );

  //!@brief The destructor.
  ~Connection();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Tests, whether the step is part of this connection (either source or target).
  bool contains(cedar::proc::StepPtr step);
  //! Tests, whether the trigger is part of this connection (either source or target).
  bool contains(cedar::proc::TriggerPtr trigger);

  //! Deletes the connection.
  void deleteConnection();
  
  //! returns the source step instance
  cedar::proc::StepPtr getSource();
  //! returns the source step instance as const
  cedar::proc::ConstStepPtr getSource() const;
  //! returns the source step's name
  const std::string& getSourceName() const;
  //! returns the target step instance
  cedar::proc::StepPtr getTarget();
  //! returns the target step instance as const
  cedar::proc::ConstStepPtr getTarget() const;
  //! returns the target step's name
  const std::string& getTargetName() const;

  //! returns the source trigger instance
  cedar::proc::ConstTriggerPtr getSourceTrigger() const;
  //! returns the target trigger instance
  cedar::proc::ConstTriggerPtr getTargetTrigger() const;

  //! are source and target still valid? (i.e. the weak pointer can be casted to shared pointer)
  bool isValid() const;

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
  //!@brief the source trigger
  boost::weak_ptr<cedar::proc::Trigger> mTrigger;
  //!@brief the target trigger
  boost::weak_ptr<cedar::proc::Trigger> mTargetTrigger;
  //!@brief the source step
  boost::weak_ptr<cedar::proc::Step> mSource;
  //!@brief the source step's name
  std::string mSourceName;
  //!@brief the target step
  boost::weak_ptr<cedar::proc::Step> mTarget;
  //!@brief the target step's name
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

