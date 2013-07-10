/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        TransferFunction.h

    Maintainer:  Oliver Lomp

    Email:       oliver.lomp@ini.ruhr-uni-bochum.de

    Date:        2013 07 03

    Description: This class provides the processing with the configurable sigmoid function.

    Credits:     Originally implemented by Michael Berger.

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_TRANSFER_FUNCTION_H
#define CEDAR_PROC_STEPS_TRANSFER_FUNCTION_H

// LOCAL INCLUDES
#include "cedar/processing/steps/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include "cedar/auxiliaries/math/namespace.h"
#include "cedar/auxiliaries/math/Sigmoid.h"
#include "cedar/auxiliaries/ObjectParameterTemplate.h"
#include "cedar/auxiliaries/ObjectListParameterTemplate.h"
#include "cedar/auxiliaries/namespace.h"
#include "cedar/processing/Step.h"


/*!@brief Applies a transfer function to its input.
 *
 *        The transfer function can be chosen from the list of all available ones registered with cedar.
 */
class cedar::proc::steps::TransferFunction : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief a parameter for sigmoid objects
  typedef cedar::aux::ObjectParameterTemplate<cedar::aux::math::TransferFunction> TransferFunctionParameter;

  //!@cond SKIPPED_DOCUMENTATION
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(TransferFunctionParameter);
  //!@endcond

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  TransferFunction();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief do this if the input changes
  void inputConnectionChanged(const std::string& inputName);

  //!@brief determine if a given Data is a valid input
  cedar::proc::DataSlot::VALIDITY determineInputValidity
                                  (
                                    cedar::proc::ConstDataSlotPtr,
                                    cedar::aux::ConstDataPtr
                                  ) const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief compute
  void compute(const cedar::proc::Arguments&);


  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief input member
  cedar::aux::ConstMatDataPtr mInput;

  //!@brief sigmoidal output member
  cedar::aux::MatDataPtr mOutput;

private:

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief any sigmoid function
  TransferFunctionParameterPtr _mTransferFunction;


private:
  // none yet

}; // class cedar::proc::steps::TransferFunction

#endif // CEDAR_PROC_STEPS_TRANSFER_FUNCTION_H
