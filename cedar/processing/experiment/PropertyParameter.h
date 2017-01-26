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

    File:        PropertyParameter.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2015 01 16

    Description: Header file for the class cedar::proc::experiment::PropertyParameter.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_PROPERTY_PARAMETER_H
#define CEDAR_PROC_EXPERIMENT_PROPERTY_PARAMETER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/ConnectableParameter.fwd.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/processing/Connectable.fwd.h"
#include "cedar/processing/DataRole.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/PropertyParameter.fwd.h"

// SYSTEM INCLUDES
#include <QObject>
#include <vector>
#include <string>


/*!@brief A parameter to set a property of a certain connectable.
 *
 *          The property could be a parameter, data, actions (implemented in child classes).
 */
class cedar::proc::experiment::PropertyParameter : public cedar::aux::Parameter
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
  PropertyParameter
  (
    cedar::aux::Configurable* pOwner,
    const std::string& name,
    cedar::proc::experiment::ConnectableParameterPtr connectableParameter
  );

  //!@brief Destructor
  virtual ~PropertyParameter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Checks the validity of the parameter.
  virtual bool checkValidity(std::string& errors) const = 0;

  //!@brief Set parameter to default
  void makeDefault();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Get the connectable (for all child classes)
  cedar::proc::ConnectablePtr getConnectable();

  //!@brief Get the connectable (for all child classes, const)
  cedar::proc::ConstConnectablePtr getConnectable() const;

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
  //! Pointer to the associated ConnectableParameter.
  cedar::proc::experiment::ConnectableParameterPtr mConnectableParameter;
}; // class cedar::proc::experiment::PropertyParameter

#endif // CEDAR_PROC_EXPERIMENT_PROPERTY_PARAMETER_H

