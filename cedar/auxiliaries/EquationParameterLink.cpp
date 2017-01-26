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

    File:        EquationParameterLink.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 02 27

    Description: Source file for the class cedar::aux::EquationParameterLink.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/EquationParameterLink.h"
#include "cedar/auxiliaries/NumericParameterHelper.h"
#include "cedar/auxiliaries/ArithmeticExpression.h"

// SYSTEM INCLUDES
#include <map>

//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool registerType()
  {
    cedar::aux::ParameterLinkFactoryManagerSingleton::getInstance()->registerType<cedar::aux::EquationParameterLinkPtr>();
    return true;
  }

  bool registered = registerType();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EquationParameterLink::EquationParameterLink()
:
_mEquation(new cedar::aux::StringParameter(this, "equation", "target = source"))
{
  this->equationChanged();
  QObject::connect(this->_mEquation.get(), SIGNAL(valueChanged()), this, SLOT(equationChanged()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::EquationParameterLink::equationChanged()
{
  cedar::aux::ArithmeticExpression expr(this->_mEquation->getValue());
  this->mForwardExpression = expr.solveFor("target");
  this->mBackwardExpression = expr.solveFor("source");
}

void cedar::aux::EquationParameterLink::sourceChanged()
{
  std::map<std::string, double> variables;
  variables["source"] = cedar::aux::NumericParameterHelper::getValue(this->getSource());
  double new_value = this->mForwardExpression->getRight()->evaluate(variables);
  cedar::aux::NumericParameterHelper::setValue(this->getTarget(), new_value);
}

void cedar::aux::EquationParameterLink::targetChanged()
{
  std::map<std::string, double> variables;
  variables["target"] = cedar::aux::NumericParameterHelper::getValue(this->getTarget());
  double new_value = this->mBackwardExpression->getRight()->evaluate(variables);
  cedar::aux::NumericParameterHelper::setValue(this->getSource(), new_value);
}

bool cedar::aux::EquationParameterLink::checkIfLinkable
     (
       cedar::aux::ConstParameterPtr source,
       cedar::aux::ConstParameterPtr target
     )
     const
{
  return cedar::aux::NumericParameterHelper::isNumeric(source) && cedar::aux::NumericParameterHelper::isNumeric(target);
}
