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

    File:        DistanceImageCoordinateTransform.cpp

    Maintainer:  Jan Tekuelve
    Email:       jan.tekuelve@ini.rub.de
    Date:        2022 03 02

    Description: Source file for the class cedar::proc::steps::DistanceImageCoordinateTransform.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/DistanceImageCoordinateTransform.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/processing/ElementDeclaration.h"

// SYSTEM INCLUDES
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
                      new ElementDeclarationTemplate<cedar::proc::steps::DistanceImageCoordinateTransform>
                              (
                                      "Image Processing",
                                      "cedar.processing.DistanceImageCoordinateTransform"
                              )
              );
      declaration->setIconPath(":steps/distance_image_coordinate_transform.svg");
      declaration->setDescription
              (
                      "Transforms a Distance Image into an allocentric coordinate frame depending on the source camera's pan and tilt."
              );

      declaration->declare();

      return true;
    }

    bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::DistanceImageCoordinateTransform::DistanceImageCoordinateTransform():
mCameraTranslation(new cedar::aux::DoubleVectorParameter(this,"camera joint translation (depth, horizontal, height in meter)",3,0)),
mTiltJointDistance(new cedar::aux::DoubleParameter (this,"cam distance to tiltjoint (m)",0)),
mCameraFrustrumAngleDeg(new cedar::aux::DoubleVectorParameter(this,"camera frustrum angle (deg)",2,45)),
mOutputSizes(new cedar::aux::IntVectorParameter(this,"output sizes",3,100)),
mOutputScaling(new cedar::aux::DoubleParameter(this,"field unit per meter",100,1,1000)),
mOutputTranslation(new cedar::aux::DoubleVectorParameter(this,"output translation (m)",3,0))
{
  std::vector<int> defaultSizes {mOutputSizes->getValue().at(0),mOutputSizes->getValue().at(1),mOutputSizes->getValue().at(2)};
  mAlloOutput = cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat::zeros(3, &defaultSizes.at(0), CV_32F)));

  // declare all inputs
  cedar::proc::DataSlotPtr imageInput = this->declareInput(mDistanceImageInputName, true);
  cedar::proc::DataSlotPtr rollInput = this->declareInput(mRollInputName, false);
  cedar::proc::DataSlotPtr tiltInput = this->declareInput(mTiltInputName, false);
  cedar::proc::DataSlotPtr panInput = this->declareInput(mPanInputName, false);

  // declare output
  auto weightTriggerOutput = this->declareOutput(mAlloOutputName, mAlloOutput);

  // setup connections
  QObject::connect(mOutputSizes.get(), SIGNAL(valueChanged()),this, SLOT(outputSizeChanged()));

}

cedar::proc::steps::DistanceImageCoordinateTransform::~DistanceImageCoordinateTransform()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::DistanceImageCoordinateTransform::compute(const cedar::proc::Arguments&)
{
  //Do something if all inputs are present
  if (mDistanceImageInput)
  {
    float rollRad = this->mRollInput ? this->mRollInput->getData().at<float>(0,0) * (M_PI/180.0) : 0; //Rotation along Roll if present
    float tiltRad = this->mTiltInput ? this->mTiltInput->getData().at<float>(0,0) * (M_PI/180.0) : 0; // This is rotation along pitch in Rad if present
    float panRad = this->mPanInput ? this->mPanInput->getData().at<float>(0,0) * (M_PI/180.0) :0; // This is rotation along yaw in Rad if present

    cv::Mat outputMat = computeAllocentricRepresentation(mDistanceImageInput->getData(), rollRad,tiltRad,panRad);

    mAlloOutput->setData(outputMat);
  }
}

void cedar::proc::steps::DistanceImageCoordinateTransform::inputConnectionChanged(const std::string& inputName)
{
  if (inputName == mDistanceImageInputName)
  {
    this->mDistanceImageInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  }

  if (inputName == mPanInputName)
  {
    this->mPanInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  }

  if (inputName == mTiltInputName)
  {
    this->mTiltInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  }

  if (inputName == mRollInputName)
  {
    this->mRollInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  }
}


cv::Mat cedar::proc::steps::DistanceImageCoordinateTransform::computeAllocentricRepresentation(cv::Mat distanceImage,float rollRad,float tiltRad, float panRad)
{
//  auto start_time = boost::posix_time::microsec_clock::universal_time();

  std::vector<int> outPutSizes {mOutputSizes->getValue().at(0),mOutputSizes->getValue().at(1),mOutputSizes->getValue().at(2)};
  cv::Mat returnMat = cv::Mat(3, &outPutSizes.at(0), CV_32F,cv::Scalar(0));

  // For OpenCV x and y are actually swapped
  float angleRangeHorizontal = this->mCameraFrustrumAngleDeg->getValue().at(0) * (M_PI/180.0); // Convert to radians
  int horizontalPixels = distanceImage.cols;
  float horizontalAnglePerPixel = angleRangeHorizontal/(float) horizontalPixels;
  int xZeroAnglePixel = horizontalPixels/2;

  float angleRangeVertical = this->mCameraFrustrumAngleDeg->getValue().at(1) * (M_PI/180.0); // Convert to radians
  int verticalPixels = distanceImage.rows;
  float verticalAnglePerPixel = angleRangeVertical/(float) verticalPixels;
  int yZeroAnglePixel = verticalPixels/2;


  cv::Mat rotationMat = calculateRotationMatrix(rollRad,tiltRad,panRad);
  auto translationVectorToJoint = this->mCameraTranslation->getValue();
  auto translationVectorJointToCam = calculateTranslationFromJointToCam(rollRad,tiltRad,panRad);
  auto fieldUnitPerMeter = this->mOutputScaling->getValue();
  auto outputTranslation = this->mOutputTranslation->getValue();

 //Parallelizing saves 30 ms per iteration on a 300x300 depthImage on my VM
  distanceImage.forEach<float>([&returnMat,xZeroAnglePixel,horizontalAnglePerPixel,yZeroAnglePixel,verticalAnglePerPixel,rotationMat,translationVectorToJoint,translationVectorJointToCam,outputTranslation,fieldUnitPerMeter](float &p, const int * position) -> void {
            int yImage = position[0]; //OpenCV has a weird depiction of row and column in images...
            int xImage = position[1];
            float distance = p;

            float currentXAngle = (xImage - xZeroAnglePixel) * horizontalAnglePerPixel;
            float currentYAngle = (yImage - yZeroAnglePixel) * verticalAnglePerPixel;
            float xEgo = tan(currentXAngle) * distance; // Pythagoras..
            float zEgo = distance;
            float yEgo = tan(currentYAngle) * distance * -1; //flip y-axis from image to regular. Upwards is positive and downwards from image center is negative

            cv::Mat egoVector = cv::Mat::zeros(3,1,CV_32F);
            egoVector.at<float>(0,0) = zEgo;
            egoVector.at<float>(1,0) = xEgo;
            egoVector.at<float>(2,0) = yEgo;

            //Rotate the whole thing!
            cv::Mat rotatedVector = rotationMat * egoVector;

            float zReal = rotatedVector.at<float>(0,0) + translationVectorToJoint.at(0) + translationVectorJointToCam.at(0);
            float xReal = rotatedVector.at<float>(1,0) + translationVectorToJoint.at(1) + translationVectorJointToCam.at(1);
            float yReal = rotatedVector.at<float>(2,0) + translationVectorToJoint.at(2) + translationVectorJointToCam.at(2);


            //Translate the measure values in the field coordinate frame
            int outPutX = (int) round((xReal - outputTranslation.at(1)) * fieldUnitPerMeter);
            int outPutY = (int) round((yReal - outputTranslation.at(2)) * fieldUnitPerMeter);
            int outPutZ = (int) round((zReal - outputTranslation.at(0)) * fieldUnitPerMeter);

            if(outPutX<returnMat.size[1] && outPutX >=0 && outPutY<returnMat.size[2] && outPutY >=0 && outPutZ<returnMat.size[0] && outPutZ >=0)
            {
              //Again the OpenCV Plot treats the first two dimensions in the plot different than usual (for me)
              returnMat.at<float>(outPutZ, outPutX, outPutY) = 1;
            }

          });




//  auto time_after_stepping = boost::posix_time::microsec_clock::universal_time();
//  boost::posix_time::time_duration measured_step_time_unitless = time_after_stepping - start_time;
//  double elapsedMilliSeconds = measured_step_time_unitless.total_microseconds()  / 1000.0;
//  std::cout<<"Calculation of allocentric representation took: " << elapsedMilliSeconds << " ms" <<std::endl;

  return returnMat;
}


cv::Mat cedar::proc::steps::DistanceImageCoordinateTransform::calculateRotationMatrix(float rollRad, float tiltRad,
                                                                                      float panRad)
{

  float rollEntries[9] = { 1, 0, 0, 0, cosf(rollRad), -sinf(rollRad), 0, sinf(rollRad), cosf(rollRad) };
  cv::Mat rollMatrix = cv::Mat(3,3,CV_32F,rollEntries);

  float tiltEntries[9] = { cosf(tiltRad), 0, sinf(tiltRad), 0, 1, 0, -sinf(tiltRad), 0, cosf(tiltRad) };
  cv::Mat tiltMatrix = cv::Mat(3,3,CV_32F,tiltEntries);

  float panEntries[9] = { cosf(panRad), -sinf(panRad), 0, sinf(panRad), cosf(panRad), 0, 0, 0, 1 };
  cv::Mat panMatrix = cv::Mat(3,3,CV_32F,panEntries);

  cv::Mat rotationMatrix = rollMatrix *  panMatrix * tiltMatrix;

  return rotationMatrix;
}

std::vector<float> cedar::proc::steps::DistanceImageCoordinateTransform::calculateTranslationFromJointToCam(
        float rollRad, float tiltRad, float panRad)
{
  std::vector<float> returnVector {0.0,0.0,0.0};

  float tiltJointDistance = (float) this->mTiltJointDistance->getValue();

  if(rollRad!=0 && tiltJointDistance!=0)
  {
    this->setState(cedar::proc::Step::STATE_EXCEPTION,
                   "Currently Rolling is only supported if the camera rotated around its own center");

    return returnVector;
  }

  float upwardsAngleRad =  (M_PI/2.0) - tiltRad;  //For CAREN at 0.0 degree tilt the camera is upright

  float heightTranslation = sinf(upwardsAngleRad) * tiltJointDistance;
  float depthTranslationTiltOnly = cosf(upwardsAngleRad) * tiltJointDistance;

  float horizontalTranslation = sinf(panRad) * depthTranslationTiltOnly;
  float depthTranslationAfterPan = cosf(panRad) * depthTranslationTiltOnly;

  returnVector.at(0) = depthTranslationAfterPan;
  returnVector.at(1) = horizontalTranslation;
  returnVector.at(2) = heightTranslation;

//  std::cout<<"TranslationChange through TiltJointDistancefromCam: " << tiltJointDistance <<" is: " << depthTranslationAfterPan << ", " << horizontalTranslation <<", " << heightTranslation <<std::endl;

  return returnVector;
}


void cedar::proc::steps::DistanceImageCoordinateTransform::outputSizeChanged()
{
//  std::cout<<"OutputSizeChanged >> RecalculateOutputData" << std::endl;
  std::vector<int> outPutSizes {mOutputSizes->getValue().at(0),mOutputSizes->getValue().at(1),mOutputSizes->getValue().at(2)};
  cv::Mat outPutMat = cv::Mat::zeros(3, &outPutSizes.at(0), CV_32F);
  mAlloOutput->setData(outPutMat);

  //Recalculate the Output, if input is present
  this->onTrigger();
}