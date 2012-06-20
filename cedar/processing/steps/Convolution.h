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

    File:        Convolution.h

    Maintainer:  Ulja van Hengel
    Email:       ulja@ini.ruhr-uni-bochum.de
    Date:        2012 06 04

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_CONVOLUTION_H
#define CEDAR_PROC_STEPS_CONVOLUTION_H

// CEDAR INCLUDES
#include "cedar/processing/steps/namespace.h"
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/convolution/Convolution.h"
#include "cedar/auxiliaries/UIntParameter.h"

// SYSTEM INCLUDES

//TODO description
/*!@brief   This is a static gain step that allows multiplying a matrix input with a factor.
 *
 *          This step implements the cedar::proc::Step interface and allows a user to easily multiply any matrix with a
 *          gain factor. This step is called a static gain step because the gain factor is determined by a parameter,
 *          rather than being an input that might change over time.
 *
 * @remarks This step declares the following interface:
 *          input - any matrix data
 *          output - the output, i.e., input multiplied by gainFactor
 *
 *          Parameters of the step are:
 *          gainFactor - the gain factor.
 */
class cedar::proc::steps::Convolution : public cedar::proc::Step
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
  Convolution();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Updates the output matrix.
  void compute(const cedar::proc::Arguments& arguments);

  //!@brief Returns the border type of the current convolution.
  inline cedar::aux::conv::BorderType::Id getBorderType() const
  {
    return this->mConvolution->getBorderType();
  }

  //!@brief Returns the border type of the current convolution.
  inline cedar::aux::conv::Mode::Id getMode() const
  {
    return this->mConvolution->getMode();
  }

//  //!@brief Sets the current gain factor.
//  inline void setBorderType(double gainFactor)
//  {
//    this->mConvolution->setValue(gainFactor);
//  }

public slots:
  //!@brief Triggers a computation of a step.
  void recompute();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Determines whether the data item can be connected to the slot.
  cedar::proc::DataSlot::VALIDITY determineInputValidity
                                  (
                                    cedar::proc::ConstDataSlotPtr slot,
                                    cedar::aux::DataPtr data
                                  ) const;
  //!@brief Reacts to a change in the input connection.
  void inputConnectionChanged(const std::string& inputName);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief The Data containing the input matrix
  cedar::aux::ConstMatDataPtr mMatrix;
  //!@brief The Data containing the input kernel
  cedar::aux::ConstMatDataPtr mKernel;
  //!@brief The data containing the result of the convolution.
  cedar::aux::MatDataPtr mOutput;

  //!@brief Containing convolution methods and parameter
  cedar::aux::conv::ConvolutionPtr mConvolution;
private:

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief The factor by which the input is multiplied.


private:

}; // class cedar::proc::steps::Convolution

#endif // CEDAR_PROC_STEPS_CONVOLUTION_H
