/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        MatrixPadding.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 01 13

    Description: Header file for the class cedar::proc::steps::MatrixPadding.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_MATRIX_PADDING_H
#define CEDAR_PROC_STEPS_MATRIX_PADDING_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/processing/steps/MatrixPadding.fwd.h"

// SYSTEM INCLUDES


/*!@brief A processing step that pads borders onto an input matrix.
 */
class cedar::proc::steps::MatrixPadding : public cedar::proc::Step
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
  MatrixPadding();

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
  void compute(const cedar::proc::Arguments&);

  void compute2D();

  template <typename T> void compute3D();

  template <typename T> void computeND();

  void inputConnectionChanged(const std::string& inputName);

private slots:
  void updateOutputSize();

  void recompute();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::aux::ConstMatDataPtr mInput;

  cedar::aux::MatDataPtr mPadded;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // How much border is added in each dimension
  cedar::aux::UIntVectorParameterPtr _mPaddedSize;

  cedar::aux::EnumParameterPtr _mBorderType;

}; // class cedar::proc::steps::MatrixPadding

#endif // CEDAR_PROC_STEPS_MATRIX_PADDING_H

