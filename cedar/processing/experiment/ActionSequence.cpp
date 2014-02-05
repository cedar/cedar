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

    File:        ActionSequence.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.ruhr-uni-bochum.de
    Date:        2014 01 22

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/ActionSequence.h"

// SYSTEM INCLUDES
#include <boost/bind.hpp>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::experiment::ActionSequence::ActionSequence()
:
_conditionSet(boost::shared_ptr<Configurable>(new Configurable())),
_actionSet((boost::shared_ptr<Configurable>(new Configurable())))
{
	this->addConfigurableChild("ConditionSet",_conditionSet);
	this->addConfigurableChild("ActionSet",_actionSet);
}
cedar::proc::experiment::ActionSequence::~ActionSequence()
{

}




//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::experiment::ActionSequence::addAction(cedar::proc::experiment::ActionPtr action)
{
	this->_actionSet->addConfigurableChild("Action",action);
}

void cedar::proc::experiment::ActionSequence::addCondition(cedar::proc::experiment::ConditionPtr condition)
{
	this->_conditionSet->addConfigurableChild("Condition",condition);
}
