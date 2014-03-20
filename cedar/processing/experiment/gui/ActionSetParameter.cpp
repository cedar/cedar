/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ActionSetParameter.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 03 07

    Description: Source file for the class cedar::proc::experiment::gui::ActionSetParameter.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/gui/ActionSetParameter.h"
#include "cedar/processing/experiment/ActionSetParameter.h"
#include "cedar/processing/experiment/ExperimentController.h"

// SYSTEM INCLUDES
#include <QLabel>

//----------------------------------------------------------------------------------------------------------------------
// associate experiment::gui::Action with the experiment::Action
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered = cedar::proc::experiment::gui::ActionFactorySingleton::getInstance()->add
      <
        cedar::proc::experiment::ActionSetParameter,
        cedar::proc::experiment::gui::ActionSetParameter
      >();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::gui::ActionSetParameter::ActionSetParameter()
:
mStepSelector(NULL)
{
}

cedar::proc::experiment::gui::ActionSetParameter::~ActionSetParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::experiment::gui::ActionSetParameter::redraw()
{
  if(mAction)
  {
   cedar::aux::ParameterPtr step_para = this->mAction->getParameter("StepProperty");
   mStepSelector = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(step_para)->allocateRaw();
   mStepSelector->setParent(this);
   mStepSelector->setParameter(step_para);
   this->layout()->addWidget(mStepSelector);
  }
}
