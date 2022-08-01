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

  //std::cout<<"\n***************************\nTargetPoint:>>" << coordinatePoint.t() << "\tCameraPos: " << cameraJointPos.t() << std::endl;

//  std::cout<<"Distances between Camera and Point:\n>>" << distanceMat << std::endl;

  //CAREN's camera setup in mind, this assumes pan first followed by the correct tilt

  float panRad = atan2(distanceMat.at<float>(1,0),distanceMat.at<float>(0,0)) ;  // angle in the x,z plane

  float distanceInXZPlane = sqrt(distanceMat.at<float>(0,0)*distanceMat.at<float>(0,0) + distanceMat.at<float>(1,0)* distanceMat.at<float>(1,0));

  //std::cout << "DistanceInXZPlane: " << distanceInXZPlane << std::endl;

  // angle in the y,z plane after rotation. Distance of point is now the distance in the x,z plane. Flip sign for Caren Head, because downwards is positive
  // 
  // 
  // This is the old way... The new way incorporates the distance between joint and camera for tilt
  float oldTiltRad = atan2(distanceMat.at<float>(2,0),distanceInXZPlane)  * -1.0;

  //To find out the correct tilt, we have to imagine the camera moving on a circle with the center being the joint and the radius the distance between cam and joint, i.e., mTiltJointDistance
  //On this circle we need to rotate until we reach a point on the circle that is part of a tangent from the target point to this circle
  //We know the circle_center, the radius and the target point, so we can achieve this following, for example, this shorthand formula: https://de.serlo.org/mathe/1647/tangente-an-kreis
  //There will be two correct tangents...

  //Define variables according to website
  float r = mTiltJointDistance->getValue();
  float pointM[2] = { 0,cameraJointPos.at<float>(0,2) };
  cv::Mat M = cv::Mat(2, 1, CV_32F, pointM);
  float pointQ[2] = { distanceInXZPlane,coordinatePoint.at<float>(0,2) };
  cv::Mat Q = cv::Mat(2, 1, CV_32F, pointQ);
  float distanceMQ = sqrt(pow(Q.at<float>(0, 0) - M.at<float>(0, 0), 2) + pow(Q.at<float>(1, 0) - M.at<float>(1, 0), 2));

  //std::cout << "Parameters:\n\tM: " << M.t() <<"\n\tQ:" << Q.t() <<"\n\tr:" << r << std::endl;

  float term1Top = M.at<float>(0, 0) + ((pow(r, 2) / pow(distanceMQ, 2)) * (Q.at<float>(0, 0) - M.at<float>(0, 0)));
  float term1Bottom = M.at<float>(1, 0) + ((pow(r, 2) / pow(distanceMQ, 2)) * (Q.at<float>(1, 0) - M.at<float>(1, 0)));

  float term2Factor = sqrt(pow(distanceMQ, 2) - pow(r, 2)) * (r / pow(distanceMQ, 2));
  float term2Top = term2Factor * (-Q.at<float>(1, 0) + M.at<float>(1, 0));
  float term2Bottom = term2Factor * (Q.at<float>(0, 0) - M.at<float>(0, 0));


  //Resulting in two points S1 and S2
  float S1Top = term1Top + term2Top;
  float S1Bottom = term1Bottom + term2Bottom;
  float pointS1[] = { S1Top,S1Bottom };
  cv::Mat S1 = cv::Mat(2, 1, CV_32F, pointS1);

  float S2Top = term1Top - term2Top;
  float S2Bottom = term1Bottom - term2Bottom;
  float pointS2[] = { S2Top,S2Bottom };
  cv::Mat S2 = cv::Mat(2, 1, CV_32F, pointS2);

  //std::cout << "\tS1: " << S1.t() << "\n\tS2:" << S2.t() << std::endl;
  // To get now the tilt required to end up at this point, we can again use the atan2 formula, probably with some fixed offset, because I am currently not sure where 0 degrees is in this picture

  cv::Mat differenceMS1 = S1 - M;
  cv::Mat differenceMS2 = S2 - M;

  float tiltRadS1 = atan2(differenceMS1.at<float>(1, 0), differenceMS1.at<float>(0, 0));
  float tiltRadS2 = atan2(differenceMS2.at<float>(1, 0), differenceMS2.at<float>(0, 0));

  float tiltDegS1 = tiltRadS1 * 180.0 / M_PI;
  float tiltDegS2 = tiltRadS2 * 180.0 / M_PI;

  float panDeg = panRad * 180.0 / M_PI;
  float tiltDeg = 90 - tiltDegS1; // We try with an offset and just take the positive point S1 always! Let's see when this bites us in the butt...

//  std::cout<<"Distance in XZ plane \n>>" << distanceInXZPlane << std::endl;
  /*std::cout<<"PanRad: " << panRad << "\tTiltRad " << tiltRad << "\nPanDegree: " << panDeg << "\tTiltDegree " << tiltDeg << std::endl;*/
  //std::cout << "TiltToS1WithOffset: " << tiltDeg << "\tTiltDegS1 " << tiltDegS1 << "\tTiltDegS2: " << tiltDegS2 << std::endl;

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