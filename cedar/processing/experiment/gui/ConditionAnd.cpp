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

    File:        ConditionAnd.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 02 26

    Description: Source file for the class cedar::proc::experiment::gui::ConditionAnd.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/gui/ConditionAnd.h"
#include "cedar/processing/experiment/ConditionAnd.h"
#include "cedar/auxiliaries/gui/Parameter.h"

// SYSTEM INCLUDES
#include <QLabel>

//----------------------------------------------------------------------------------------------------------------------
// associate experiment::gui::Condition with the experiment::Condition
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered = cedar::proc::experiment::gui::ConditionFactorySingleton::getInstance()->add
      <
        cedar::proc::experiment::ConditionAnd,
        cedar::proc::experiment::gui::ConditionAnd
      >();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::gui::ConditionAnd::ConditionAnd()
:
mCondition1(NULL)
,
mCondition2(NULL)
,
mConditionRow1(new QHBoxLayout)
,
mConditionRow2(new QHBoxLayout)

{
  update();
}

cedar::proc::experiment::gui::ConditionAnd::~ConditionAnd()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::experiment::gui::ConditionAnd::redraw()
{
  if(!mCondition)
    return;


  QVBoxLayout* widgets = new QVBoxLayout;
  mLayout->addLayout(widgets);
  widgets->addLayout(mConditionRow1);

  cedar::aux::ParameterPtr condition = mCondition->getParameter("Condition1");
  cedar::aux::gui::Parameter* conditionSelector
          = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(condition)->allocateRaw();
  conditionSelector->setParent(this);
  conditionSelector->setParameter(condition);
  mConditionRow1->addWidget(conditionSelector);
  connect(condition.get(),SIGNAL(valueChanged()),this,SLOT(updateConditions()));

  QLabel* andstring = new QLabel(QString::fromStdString("AND"));
  widgets->addWidget(andstring);
  widgets->addLayout(mConditionRow2);

  condition = mCondition->getParameter("Condition2");
  conditionSelector = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(condition)->allocateRaw();
  conditionSelector->setParent(this);
  conditionSelector->setParameter(condition);
  mConditionRow2->addWidget(conditionSelector);
  connect(condition.get(),SIGNAL(valueChanged()),this,SLOT(updateConditions()));
  updateConditions();
}
void cedar::proc::experiment::gui::ConditionAnd::updateConditions()
{

  delete mCondition1;
  cedar::aux::ParameterPtr condition1 = mCondition->getParameter("Condition1");

  cedar::proc::experiment::Condition::ConditionParameterPtr conditionPara1 =
      boost::dynamic_pointer_cast<cedar::proc::experiment::Condition::ConditionParameter>(condition1);
  mCondition1 = cedar::proc::experiment::gui::ConditionFactorySingleton::getInstance()->get(conditionPara1->getValue())->allocateRaw();
  mCondition1->setCondition(conditionPara1->getValue());
  mConditionRow1->addWidget(mCondition1);


  delete mCondition2;
  cedar::aux::ParameterPtr condition2 = mCondition->getParameter("Condition2");
  cedar::proc::experiment::Condition::ConditionParameterPtr conditionPara2 =
      boost::dynamic_pointer_cast<cedar::proc::experiment::Condition::ConditionParameter>(condition2);
  mCondition2 = cedar::proc::experiment::gui::ConditionFactorySingleton::getInstance()->get(conditionPara2->getValue())->allocateRaw();
  mCondition2->setCondition(conditionPara2->getValue());
  mConditionRow2->addWidget(mCondition2);
}
