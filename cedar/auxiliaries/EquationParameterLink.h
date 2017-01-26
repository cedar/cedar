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

    File:        EquationParameterLink.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 02 27

    Description: Header file for the class cedar::aux::EquationParameterLink.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_EQUATION_PARAMETER_LINK_H
#define CEDAR_AUX_EQUATION_PARAMETER_LINK_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/ParameterLink.h"
#include "cedar/auxiliaries/ArithmeticExpression.fwd.h"
#include "cedar/auxiliaries/StringParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/EquationParameterLink.fwd.h"

// SYSTEM INCLUDES


/*!@brief A parameter link that uses a (linear) equation to determine how to link parameters.
 */
class cedar::aux::EquationParameterLink : public cedar::aux::ParameterLink
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  EquationParameterLink();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

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

  void sourceChanged();

  void targetChanged();

  bool checkIfLinkable(cedar::aux::ConstParameterPtr source, cedar::aux::ConstParameterPtr target) const;

private slots:
  void equationChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Expression of the form right = f(left)
  cedar::aux::ArithmeticExpressionPtr mForwardExpression;

  //! Expression of the form left = f(right)
  cedar::aux::ArithmeticExpressionPtr mBackwardExpression;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! Equation linking the two parameters. Must have the form right = f(left), where f is some linear function.
  cedar::aux::StringParameterPtr _mEquation;

}; // class cedar::aux::EquationParameterLink

#endif // CEDAR_AUX_EQUATION_PARAMETER_LINK_H

