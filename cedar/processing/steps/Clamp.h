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

    File:        Clamp.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 08 13

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_CLAMP_H
#define CEDAR_PROC_STEPS_CLAMP_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/MatData.h"

// FORWARD DECLARATIONS
#include "cedar/processing/steps/Clamp.fwd.h"

// SYSTEM INCLUDES

/*!@brief Applies a threshold to its input.*/

class cedar::proc::steps::Clamp : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Clamp();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public slots:
  /// none

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private slots:
  void applyLowerThesholdChanged();

  void applyUpperThesholdChanged();

  void recalculate();

private:
  void compute(const cedar::proc::Arguments&);

  void inputConnectionChanged(const std::string& inputName);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // inputs
  //! Input image (cached for performance).
  cedar::aux::ConstMatDataPtr mInputImage;

  // outputs
  //! Input image after all selected thresholds have been applied.
  cedar::aux::MatDataPtr mClampedImage;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! Whether or not to apply the lower threshold.
  cedar::aux::BoolParameterPtr mApplyLowerClamp;

  //! Whether or not to apply the upper threshold.
  cedar::aux::BoolParameterPtr mApplyUpperClamp;

  //! Lower threshold.
  cedar::aux::DoubleParameterPtr _mLowerClampValue;

  //! Upper threshold.
  cedar::aux::DoubleParameterPtr _mUpperClampValue;

  cedar::aux::BoolParameterPtr mReplaceLower;
  cedar::aux::BoolParameterPtr mReplaceUpper;

  cedar::aux::DoubleParameterPtr mLowerReplacement;
  cedar::aux::DoubleParameterPtr mUpperReplacement;

}; // class cedar::proc::steps::Clamp

#endif // CEDAR_PROC_STEPS_CLAMP_H
