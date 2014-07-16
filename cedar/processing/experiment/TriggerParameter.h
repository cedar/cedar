/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        TriggerParameter.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 05 10

    Description: Header file for the class cedar::proc::experiment::TriggerParameter.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_TRIGGER_PARAMETER_H
#define CEDAR_PROC_EXPERIMENT_TRIGGER_PARAMETER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Parameter.h"
// FORWARD DECLARATIONS
#include "cedar/processing/experiment/TriggerParameter.fwd.h"

// SYSTEM INCLUDES


/*!@brief a parameter to choose a certain trigger from the architecture
 *
 */
class cedar::proc::experiment::TriggerParameter : public cedar::aux::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  TriggerParameter(cedar::aux::Configurable *pOwner, const std::string& name);

  //!@brief Destructor
  virtual ~TriggerParameter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  //!@brief set this parameter to a value, read from a configuration node
  void readFromNode(const cedar::aux::ConfigurationNode& node);

  //!@brief write value to a configuration node
  void writeToNode(cedar::aux::ConfigurationNode& root) const;

  //!@brief set parameter to default
  void makeDefault();

  //! @brief If possible, copies the value from another parameter into this parameter.
  void copyValueFrom(cedar::aux::ConstParameterPtr other);

  //!@brief Checks if this parameter can copy its value from the given one.
  bool canCopyFrom(cedar::aux::ConstParameterPtr other) const;

  //!@brief Sets the trigger.
  void setTrigger(const std::string& triggerName);

  //!@brief Returns the current trigger.
  const std::string& getTrigger();

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
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //!@brief The name of the trigger
  std::string mTrigger;

}; // class cedar::proc::experiment::TriggerParameter

#endif // CEDAR_PROC_EXPERIMENT_TRIGGER_PARAMETER_H

