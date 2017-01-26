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

    File:        ActionParameter.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2015 01 16

    Description: Header file for the class cedar::proc::experiment::ActionParameter.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_ACTION_PARAMETER_H
#define CEDAR_PROC_EXPERIMENT_ACTION_PARAMETER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/PropertyParameter.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/processing/DataRole.h"

// FORWARD DECLARATIONS
#include "cedar/processing/Connectable.fwd.h"
#include "cedar/processing/experiment/ActionParameter.fwd.h"

// SYSTEM INCLUDES
#include <QObject>
#include <vector>
#include <string>


/*!@brief A parameter to set a property of a certain step
 *
 *          The property could either be a parameter or an output or a buffer.
 *          The property type should be defined when creating an instance of this parameter.
 */
class cedar::proc::experiment::ActionParameter : public cedar::proc::experiment::PropertyParameter
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
  ActionParameter
  (
    cedar::aux::Configurable* pOwner,
    const std::string& name,
    cedar::proc::experiment::ConnectableParameterPtr connectableParameter
  );

  //!@brief Destructor
  virtual ~ActionParameter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Checks the validity of the parameter.
  bool checkValidity(std::string& errors) const;

  //!@brief set this parameter to a value, read from a configuration node
  void readFromNode(const cedar::aux::ConfigurationNode& node);

  //!@brief write value to a configuration node
  void writeToNode(cedar::aux::ConfigurationNode& root) const;

  //!@brief Sets the action.
  void setAction(const std::string& action);

  //! Returns the action.
  std::string getAction() const;

  //!@brief Calls the associated action at the associated element
  void callAction();

  void makeDefault();

  //!@brief get a list of actions associated with a given connectable
  std::vector<std::string> getListOfActions();

public slots:
  //!@brief if the connectable changes, this class has to adapt
  void connectableChanged();
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

signals:
  //!@brief signal that the list of actions has changed (this happens only if the connectable changes)
  void actionListChanged();
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
  //!@brief The action.
  std::string mAction;
}; // class cedar::proc::experiment::ActionParameter

#endif // CEDAR_PROC_EXPERIMENT_ACTION_PARAMETER_H

