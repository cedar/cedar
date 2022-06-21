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

    File:        CoordinateToCameraAngles.cpp

    Maintainer:  Jan Tekuelve
    Email:       jan.tekuelve@ini.rub.de
    Date:        2022 03 23

    Description: Source file for the class cedar::proc::steps::CoordinateToCameraAngles.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/CoordinateToCameraAngles.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/MatData.h"
#include "cedar/processing/ElementDeclaration.h"

// SYSTEM INCLUDES
#ifdef _WIN32
    #define _USE_MATH_DEFINES
    #include <cmath>
#endif

#include <math.h>



//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
    bool declare()
    {
      using cedar::proc::ElementDeclarationPtr;
      using cedar::proc::ElementDeclarationTemplate;

      ElementDeclarationPtr declaration
              (
                      new ElementDeclarationTemplate<cedar::proc::steps::CoordinateToCameraAngles>
                              (
                                      "Robotics",
                                      "cedar.processing.CoordinateToCameraAngles"
                              )
              );
      declaration->setIconPath(":steps/coordinate_to_camera_angles.svg");
      declaration->setDescription
              (
                      "Calculates the Tilt and Pan required to center a camera on a given 3D point in space"
              );

      declaration->declare();

      return true;
    }

    bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::CoordinateToCameraAngles::CoordinateToCameraAngles()
:
mAngleOutput(new cedar::aux::MatData(cv::Mat::zeros(2,1,CV_32F))),
mCameraTranslation(new cedar::aux::DoubleVectorParameter(this,"camera joint translation (depth, horizontal, height in meter)",3,0)),
mTiltJointDistance(new cedar::aux::DoubleParameter (this,"cam distance to tiltjoint (m)",0)),
mCameraFrustrumAngleDeg(new cedar::aux::DoubleVectorParameter(this,"camera frustrum angle (deg)",2,45))
{
  // declare all inputs
  cedar::proc::DataSlotPtr coordinateInput = this->declareInput(mInputCoordinatesName, true);

  // declare output
  auto angleOutput = this->declareOutput(mAngleOutputName, mAngleOutput);
}

cedar::proc::steps::CoordinateToCameraAngles::~CoordinateToCameraAngles()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::steps::CoordinateToCameraAngles::compute(const cedar::proc::Arguments&)
{
  if(mInputCoordinates)
  {
    cv::Mat cameraTranslation = cv::Mat(3,1,CV_64F,(void*) mCameraTranslation->getValue().data());
    cameraTranslation.convertTo(cameraTranslation,CV_32F);


    cv::Mat panTiltPanMat = calculateTiltPanFor3DPoint(mInputCoordinates->getData(),cameraTranslation);
    mAngleOutput->setData(panTiltPanMat);
  }
}

cv::Mat cedar::proc::steps::CoordinateToCameraAngles::calculateTiltPanFor3DPoint(cv::Mat coordinatePoint, cv::Mat cameraJointPos)
{
  //assuming points to contain:  depth,horizontal translation and height
  cv::Mat distanceMat =  coordinatePoint - cameraJointPos;

//  std::cout<<"Distances between Camera and Point:\n>>" << distanceMat << std::endl;

  //CAREN's camera setup in mind, this assumes pan first followed by the correct tilt

  float panRad = atan2(distanceMat.at<float>(1,0),distanceMat.at<float>(0,0)) ;  // angle in the x,z plane

  float distanceInXZPlane = sqrt(distanceMat.at<float>(0,0)*distanceMat.at<float>(0,0) + distanceMat.at<float>(1)* distanceMat.at<float>(1,0));

  // angle in the y,z plane after rotation. Distance of point is now the distance in the x,z plane. Flip sign for Caren Head, because downwards is positive
  float tiltRad = atan2(distanceMat.at<float>(2,0),distanceInXZPlane)  * -1.0;

  float panDeg = panRad * 180.0 / M_PI;
  float tiltDeg = tiltRad * 180.0 / M_PI;

//  std::cout<<"Distance in XZ plane \n>>" << distanceInXZPlane << std::endl;
//  std::cout<<"PanRad: " << panRad << "\tTiltRad " << tiltRad << "\nPanDegree: " << panDeg << "\tTiltDegree " << tiltDeg << std::endl;

  float anglesInDegree[2] = { tiltDeg, panDeg};
  cv::Mat returnMat =  cv::Mat(2,1,CV_32F,anglesInDegree);

  return returnMat.clone(); //Without cloning the matrix data is lost...

}

void cedar::proc::steps::CoordinateToCameraAngles::inputConnectionChanged(const std::string& inputName)
{
  if (inputName == mInputCoordinatesName)
  {
    this->mInputCoordinates = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  }
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::CoordinateToCameraAngles::determineInputValidity(cedar::proc::ConstDataSlotPtr slot, cedar::aux::ConstDataPtr data) const
{
  //Hooray for the Data Type :-P
  if (cedar::aux::ConstMatDataPtr input = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
  {
    //Accept coordinates only in 3x1 Form
    if (input && input->getDimensionality() == 1 && slot->getName() == mInputCoordinatesName && input->getData().rows == 3 && input->getData().cols == 1)
    {
        return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}