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

    File:        Condition.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 02 06

    Description: Source file for the class cedar::proc::experiment::gui::Condition.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/gui/Condition.h"
#include "cedar/auxiliaries/gui/Parameter.h"

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::gui::Condition::Condition()
{
  mLayout = new QHBoxLayout;
  this->setLayout(mLayout);
}

cedar::proc::experiment::gui::Condition::~Condition()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::experiment::gui::Condition::setCondition(cedar::proc::experiment::ConditionPtr condition)
{
  this->mCondition = condition;
  this->update();
}

void cedar::proc::experiment::gui::Condition::clear()
{
  QLayoutItem *child;
  while ((child = this->mLayout->takeAt(0)) != 0) {
    delete child->widget();
    delete child;
  }
}
void cedar::proc::experiment::gui::Condition::update()
{
  clear();
  redraw();
}
