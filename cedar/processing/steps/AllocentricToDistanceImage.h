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

    File:        AllocentricToDistanceImage.h

    Maintainer:  Jan Tekuelve
    Email:       jan.tekuelve@ini.rub.de
    Date:        2022 03 02

    Description: Header file for the class cedar::proc::steps::AllocentricToDistanceImage.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_ALLOCENTRIC_TO_DISTANCE_IMAGE_H
#define CEDAR_PROC_STEPS_ALLOCENTRIC_TO_DISTANCE_IMAGE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/IntVectorParameter.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/processing/Step.h"

// FORWARD DECLARATIONS
#include "cedar/processing/steps/AllocentricToDistanceImage.fwd.h"
#include "cedar/auxiliaries/MatData.fwd.h"

// SYSTEM INCLUDES


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::steps::AllocentricToDistanceImage: public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  AllocentricToDistanceImage();

  //!@brief Destructor
  virtual ~AllocentricToDistanceImage();

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
  void compute(const cedar::proc::Arguments& arguments);

  void inputConnectionChanged(const std::string& inputName);

  cv::Mat computeEgocentricRepresentation(cv::Mat allocentricImage,float rollRad,float tiltRad, float panRad);

  cv::Mat calculateRotationMatrix(float rollRad,float tiltRad, float panRad);

  std::vector<float> calculateTranslationFromJointToCam(float rollRad,float tiltRad,float panRad);

private slots:
  void outputSizeChanged();
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
    cedar::aux::MatDataPtr mEgoOutput;

    cedar::aux::ConstMatDataPtr mAllocentricInput;
    cedar::aux::ConstMatDataPtr mPanInput;
    cedar::aux::ConstMatDataPtr mTiltInput;
    cedar::aux::ConstMatDataPtr mRollInput;

    std::string mAllocentricInputName = "allocentric representation";
    std::string mPanInputName = "camera pan (deg)";
    std::string mTiltInputName = "camera tilt (deg)";
    std::string mRollInputName = "camera roll (deg)";
    std::string mEgoOutputName = "egocentric representation";

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
  cedar::aux::DoubleVectorParameterPtr mCameraTranslation;
  cedar::aux::DoubleParameterPtr mTiltJointDistance;
  cedar::aux::DoubleVectorParameterPtr mCameraFrustrumAngleDeg;

  cedar::aux::IntVectorParameterPtr mOutputSizes;
  cedar::aux::DoubleParameterPtr mOutputScaling;
  cedar::aux::DoubleVectorParameterPtr mOutputTranslation;
  cedar::aux::DoubleParameterPtr mAlloDetectionThreshold;

private:
  // none yet

}; // class cedar::proc::steps::AllocentricToDistanceImage

#endif // CEDAR_PROC_STEPS_ALLOCENTRIC_TO_DISTANCE_IMAGE_H
