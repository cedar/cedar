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

    File:        DistanceImageCoordinateTransform.fwd.h

    Maintainer:  Stephan Sehring
    Email:       stephan.sehring@rub.de
    Date:        2024 02 16

    Description: Caren camera to head centered transform inverse

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_INV_HEAD_CENTERED_TRANSFORM_H
#define CEDAR_PROC_STEPS_INV_HEAD_CENTERED_TRANSFORM_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/IntVectorParameter.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/processing/Step.h"

// FORWARD DECLARATIONS
#include "cedar/processing/steps/InvHeadCenteredTransform.fwd.h"
#include "cedar/auxiliaries/MatData.fwd.h"

// SYSTEM INCLUDES


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::steps::InvHeadCenteredTransform: public cedar::proc::Step
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
  InvHeadCenteredTransform();

  //!@brief Destructor
  virtual ~InvHeadCenteredTransform();

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

  cv::Mat computeEgocentricRepresentation(cv::Mat distanceImage,float rollRad,float tiltRad, float panRad);

  cv::Matx33f calculateRotationMatrix(float rollRad,float tiltRad, float panRad);
  cv::Matx33f calculateRotationMatrix2(float rollRad,float tiltRad, float panRad);
  cv::Matx44f calculateExtrinsicMatrix(float rollRad, float tiltRad, float panRad, cv::Vec3f translation);

private slots:
  void outputSizeChanged();
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
    cedar::aux::MatDataPtr mEgoOutput;

    cedar::aux::ConstMatDataPtr mAlloInput;
    cedar::aux::ConstMatDataPtr mPanInput;
    cedar::aux::ConstMatDataPtr mTiltInput;
    cedar::aux::ConstMatDataPtr mRollInput;

    std::string mAlloInputName = "Head-Centered Representation";
    std::string mPanInputName = "camera pan (deg)";
    std::string mTiltInputName = "camera tilt (deg)";
    std::string mRollInputName = "camera roll (deg)";
    std::string mEgoOutputName = "Cameracentric Output";

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
  cedar::aux::DoubleParameterPtr mTiltJointDistance;
  cedar::aux::DoubleVectorParameterPtr mCameraFrustrumAngleDeg;
  cedar::aux::DoubleParameterPtr mCameraFocalLength;

  cedar::aux::IntVectorParameterPtr mOutputSizes;

private:
  // none yet

}; // class cedar::proc::steps::InvHeadCenteredTransform

#endif // CEDAR_PROC_STEPS_INV_HEAD_CENTERED_TRANSFORM_H

