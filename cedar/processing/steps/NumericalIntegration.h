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

    File:        NumericalIntegration.h

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        

    Description: Header file for the class cedar::proc::steps::NumericalIntegration.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS__NUMERICAL_INTEGRATION_H
#define CEDAR_PROC_STEPS__NUMERICAL_INTEGRATION_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include <cedar/processing/Step.h>
#include <cedar/processing/InputSlotHelper.h>
#include <cedar/auxiliaries/MatData.h>
#include <cedar/auxiliaries/DoubleParameter.h>
#include <opencv2/opencv.hpp>

// FORWARD DECLARATIONS
#include "cedar/processing/steps/NumericalIntegration.fwd.h"

// SYSTEM INCLUDES


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::steps::NumericalIntegration : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  NumericalIntegration();

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
  void inputConnectionChanged(const std::string& inputName);

  void compute(const cedar::proc::Arguments& arguments);
  void recompute();
  void reset();
  void reinitialize();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief MatrixData representing the input. Storing it like this saves time during computation.
  cedar::aux::ConstMatDataPtr mInput;
  cedar::aux::ConstMatDataPtr mDelayOptional;
  cedar::aux::ConstMatDataPtr mInitialOptional;

  //!@brief The output data.
  cedar::aux::MatDataPtr mOutput;

  cv::Mat mOneBack;
  cv::Mat mTwoBack;
  cv::Mat mThreeBack;
  cv::Mat mFourBack;

  cv::Mat mLastState;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:

private:
  cedar::unit::Time mLastTime;

  cedar::aux::BoolParameterPtr mInitializeOnReset;
  cedar::aux::BoolParameterPtr mUseBDF5;

}; // class cedar::proc::steps::NumericalIntegration

#endif // CEDAR_PROC_STEPS__NUMERICAL_INTEGRATION_H

