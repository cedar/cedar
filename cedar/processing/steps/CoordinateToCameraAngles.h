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

    File:        CoordinateToCameraAngles.h

    Maintainer:  Jan Tekuelve
    Email:       jan.tekuelve@ini.rub.de
    Date:        2022 03 23

    Description: Header file for the class cedar::proc::steps::CoordinateToCameraAngles.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_COORDINATE_TO_CAMERA_ANGLES_H
#define CEDAR_PROC_STEPS_COORDINATE_TO_CAMERA_ANGLES_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/processing/Step.h"

// FORWARD DECLARATIONS
#include "cedar/processing/steps/CoordinateToCameraAngles.fwd.h"
#include "cedar/auxiliaries/MatData.fwd.h"

// SYSTEM INCLUDES


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::steps::CoordinateToCameraAngles : public cedar::proc::Step
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
  CoordinateToCameraAngles();

  //!@brief Destructor
  virtual ~CoordinateToCameraAngles();

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

  cedar::proc::DataSlot::VALIDITY determineInputValidity(
            cedar::proc::ConstDataSlotPtr slot,
            cedar::aux::ConstDataPtr data) const;

  cv::Mat calculateTiltPanFor3DPoint(cv::Mat coordinatePoint, cv::Mat cameraJointPos);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
    cedar::aux::MatDataPtr mAngleOutput;

    cedar::aux::ConstMatDataPtr mInputCoordinates;


    std::string mInputCoordinatesName = "3D allocentric coordinates (m)";
    std::string mAngleOutputName = "output angles (tilt,pan) in deg";

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
    cedar::aux::DoubleVectorParameterPtr mCameraTranslation;
    cedar::aux::DoubleParameterPtr mTiltJointDistance;
    cedar::aux::DoubleVectorParameterPtr mCameraFrustrumAngleDeg;

private:
  // none yet

}; // class cedar::proc::steps::CoordinateToCameraAngles

#endif // CEDAR_PROC_STEPS_COORDINATE_TO_CAMERA_ANGLES_H

