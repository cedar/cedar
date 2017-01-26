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

    File:        ConnectableParameter.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2015 01 16

    Description: Header file for the class cedar::proc::experiment::ConnectableParameter.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_CONNECTABLE_PARAMETER_H
#define CEDAR_PROC_EXPERIMENT_CONNECTABLE_PARAMETER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/processing/DataRole.h"

// FORWARD DECLARATIONS
#include "cedar/processing/Connectable.fwd.h"
#include "cedar/processing/experiment/ConnectableParameter.fwd.h"

// SYSTEM INCLUDES
#include <QObject>
#include <vector>
#include <string>


/*!@brief A parameter storing a connectable, on which PropertyParameter classes might apply a property.
 */
class cedar::proc::experiment::ConnectableParameter : public cedar::aux::Parameter
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
  ConnectableParameter(cedar::aux::Configurable* pOwner, const std::string& name);

  //!@brief Destructor
  virtual ~ConnectableParameter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Checks the validity of the parameter.
  bool checkValidity(std::string& errors) const;

  //!@brief read from a configuration node
  void readFromNode(const cedar::aux::ConfigurationNode& node);

  //!@brief write value to a configuration node
  void writeToNode(cedar::aux::ConfigurationNode& root) const;

  //!@brief Sets the connectable.
  void setConnectable(cedar::proc::ConnectablePtr connectable);

  //!@brief Sets the path of the connectable.
  void setConnectablePath(const std::string& connectable);

  //! Returns the path of the currently selected connectable.
  std::string getConnectablePath() const;

  //!@brief Returns the connectable
  cedar::proc::ConstConnectablePtr getConnectable() const;

  //!@brief Returns the connectable
  cedar::proc::ConnectablePtr getConnectable();

  //!@brief Set parameter to default
  void makeDefault();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

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
  //!@brief The path of the connectable.
  std::string mConnectablePath;

  //! Weak pointer to the connectable.
  cedar::proc::ConnectableWeakPtr mConnectable;
}; // class cedar::proc::experiment::ConnectableParameter

#endif // CEDAR_PROC_EXPERIMENT_CONNECTABLE_PARAMETER_H

