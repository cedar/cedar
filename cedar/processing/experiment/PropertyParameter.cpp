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

    File:        PropertyParameter.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2015 01 16

    Description: Source file for the class cedar::proc::experiment::PropertyParameter.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/PropertyParameter.h"
#include "cedar/processing/experiment/ConnectableParameter.h"
#include "cedar/processing/experiment/Supervisor.h"
#include "cedar/processing/Connectable.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/ParameterDeclaration.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::PropertyParameter::PropertyParameter
(
  cedar::aux::Configurable* pOwner,
  const std::string& name,
  cedar::proc::experiment::ConnectableParameterPtr connectableParameter
)
:
cedar::aux::Parameter(pOwner, name, false),
mConnectableParameter(connectableParameter)
{
}

cedar::proc::experiment::PropertyParameter::~PropertyParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::experiment::PropertyParameter::makeDefault()
{
//  this->mConnectableParameter.reset();
}

cedar::proc::ConnectablePtr cedar::proc::experiment::PropertyParameter::getConnectable()
{
  return this->mConnectableParameter->getConnectable();
}

cedar::proc::ConstConnectablePtr cedar::proc::experiment::PropertyParameter::getConnectable() const
{
  return this->mConnectableParameter->getConnectable();
}
