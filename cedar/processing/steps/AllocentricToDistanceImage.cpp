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

    File:        AllocentricToDistanceImage.cpp

    Maintainer:  Jan Tekuelve
    Email:       jan.tekuelve@ini.rub.de
    Date:        2022 03 02

    Description: Source file for the class cedar::proc::steps::AllocentricToDistanceImage.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/AllocentricToDistanceImage.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
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
                      new ElementDeclarationTemplate<cedar::proc::steps::AllocentricToDistanceImage>
                              (
                                      "Image Processing",
                                      "cedar.processing.AllocentricToDistanceImage"
                              )
              );
      declaration->setIconPath(":steps/distance_image_coordinate_transform.svg");
      declaration->setDescription
              (
                      "Transforms an allocentric representation into an egocentric camera frame depending on the camera's pan and tilt."
              );

      declaration->declare();

      return true;
    }

    bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::AllocentricToDistanceImage::AllocentricToDistanceImage():
mCameraTranslation(new cedar::aux::DoubleVectorParameter(this,"camera joint translation (depth, horizontal, height in meter)",3,0)),
mTiltJointDistance(new cedar::aux::DoubleParameter (this,"cam distance to tiltjoint (m)",0)),
mCameraFrustrumAngleDeg(new cedar::aux::DoubleVectorParameter(this,"camera frustrum angle (deg)",2,45)),
mOutputSizes(new cedar::aux::IntVectorParameter(this,"output sizes",2,100)),
mOutputScaling(new cedar::aux::IntVectorParameter(this,"allo field unit per meter",3,50)),
mOutputTranslation(new cedar::aux::DoubleVectorParameter(this,"output translation (m)",3,0)),
mAlloDetectionThreshold(new cedar::aux::DoubleParameter(this,"allo detection threshold",0.1))
{
//  std::vector<int> defaultSizes {mOutputSizes->getValue().at(0),mOutputSizes->getValue().at(1),mOutputSizes->getValue().at(2)};
  mEgoOutput = cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat::zeros(mOutputSizes->getValue().at(0), mOutputSizes->getValue().at(1) , CV_32F)));

  // declare all inputs
  cedar::proc::DataSlotPtr imageInput = this->declareInput(mAllocentricInputName, true);
  cedar::proc::DataSlotPtr rollInput = this->declareInput(mRollInputName, false);
  cedar::proc::DataSlotPtr tiltInput = this->declareInput(mTiltInputName, false);
  cedar::proc::DataSlotPtr panInput = this->declareInput(mPanInputName, false);

  // declare output
  auto weightTriggerOutput = this->declareOutput(mEgoOutputName, mEgoOutput);

  // setup connections
  QObject::connect(mOutputSizes.get(), SIGNAL(valueChanged()),this, SLOT(outputSizeChanged()));

}

cedar::proc::steps::AllocentricToDistanceImage::~AllocentricToDistanceImage()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::AllocentricToDistanceImage::compute(const cedar::proc::Arguments&)
{
  //Do something if all inputs are present
  if (mAllocentricInput)
  {
    float rollRad = this->mRollInput ? this->mRollInput->getData().at<float>(0,0) * (M_PI/180.0) : 0; //Rotation along Roll if present
    float tiltRad = this->mTiltInput ? this->mTiltInput->getData().at<float>(0,0) * (M_PI/180.0) : 0; // This is rotation along pitch in Rad if present
    float panRad = this->mPanInput ? this->mPanInput->getData().at<float>(0,0) * (M_PI/180.0) :0; // This is rotation along yaw in Rad if present

//    std::cout<<this->getName() << " current Roll: " << rollRad <<" Tilt: " << tiltRad << " Pan: " << panRad <<"\n\tAllocentric Dimensions. Rows: " << mAllocentricInput->getData().rows <<  " Cols: " << mAllocentricInput->getData().cols   << std::endl;

    cv::Mat outputMat = computeEgocentricRepresentation(mAllocentricInput->getData(), rollRad,tiltRad,panRad);

    mEgoOutput->setData(outputMat);
  }
}

void cedar::proc::steps::AllocentricToDistanceImage::inputConnectionChanged(const std::string& inputName)
{
  if (inputName == mAllocentricInputName)
  {
    this->mAllocentricInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
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


cv::Mat cedar::proc::steps::AllocentricToDistanceImage::computeEgocentricRepresentation(cv::Mat allocentricImage, float rollRad, float tiltRad, float panRad)
{
//  auto start_time = boost::posix_time::microsec_clock::universal_time();
    cv::Mat returnMat = cv::Mat::zeros(mOutputSizes->getValue().at(0),mOutputSizes->getValue().at(1),CV_32F);

    float angleRangeHorizontal = this->mCameraFrustrumAngleDeg->getValue().at(0) * (M_PI/180.0); // Convert to radians
    int horizontalPixels = returnMat.cols;
    float horizontalPixelPerAngle = horizontalPixels / angleRangeHorizontal;
    int xZeroAnglePixel = round(horizontalPixels/2);

    float angleRangeVertical = this->mCameraFrustrumAngleDeg->getValue().at(1) * (M_PI/180.0); // Convert to radians
    int verticalPixels = returnMat.rows;
    float verticalPixelPerAngle = verticalPixels / angleRangeVertical;
    int yZeroAnglePixel = round(verticalPixels/2);

    cv::Mat rotationMat = calculateRotationMatrix(rollRad,tiltRad,panRad);
    cv::Mat inverseRotationMat = rotationMat.inv();
    auto translationVectorToJoint = this->mCameraTranslation->getValue();
    auto translationVectorJointToCam = calculateTranslationFromJointToCam(rollRad,tiltRad,panRad);
    auto fieldUnitPerMeter = this->mOutputScaling->getValue();
    auto outputTranslation = this->mOutputTranslation->getValue();
    float alloDetectionThreshold = this->mAlloDetectionThreshold->getValue();

  allocentricImage.forEach<float>([&returnMat,angleRangeHorizontal,xZeroAnglePixel,horizontalPixelPerAngle,angleRangeVertical,yZeroAnglePixel,verticalPixelPerAngle,inverseRotationMat,translationVectorToJoint,translationVectorJointToCam,outputTranslation,fieldUnitPerMeter,alloDetectionThreshold](float &p, const int * position) -> void {

      if(p > alloDetectionThreshold)
      {      
        //There is something in the allocentric representation
        float zCoordinate = (double) position[0] / (double) fieldUnitPerMeter.at(0) + outputTranslation.at(0);
        float xCoordinate = (double) position[1] / (double) fieldUnitPerMeter.at(1) + outputTranslation.at(1);
        float yCoordiante = (double) position[2] / (double) fieldUnitPerMeter.at(2) + outputTranslation.at(2);

          //Subtract Camera Transform
        zCoordinate = zCoordinate - translationVectorToJoint.at(0) - translationVectorJointToCam.at(0);
        xCoordinate = xCoordinate - translationVectorToJoint.at(1) - translationVectorJointToCam.at(1);
        yCoordiante = yCoordiante - translationVectorToJoint.at(2) - translationVectorJointToCam.at(2);

        cv::Mat alloVector = cv::Mat::zeros(3, 1, CV_32F);
        alloVector.at<float>(0, 0) = zCoordinate;
        alloVector.at<float>(1, 0) = xCoordinate;
        alloVector.at<float>(2, 0) = yCoordiante;
        //Rotate the whole thing!
        cv::Mat reRotatedVector = inverseRotationMat * alloVector;


        float angleX = atan(reRotatedVector.at<float>(1, 0) / reRotatedVector.at<float>(0, 0));
        float angleY = atan(reRotatedVector.at<float>(2, 0) / reRotatedVector.at<float>(0, 0));

        if (abs(angleX) <= (angleRangeHorizontal / 2.0) && abs(angleY) <= (angleRangeVertical / 2.0))
        {
          // If inside the camera add it to correct location of the output matrix with correct distance value
          float imagePosX = xZeroAnglePixel + angleX * horizontalPixelPerAngle;
          float imagePosY = yZeroAnglePixel + angleY * verticalPixelPerAngle * -1; //y-direction is flipped
          int imagePosXInt = round(imagePosX);
          int imagePosYInt = round(imagePosY);
          returnMat.at<float>(imagePosYInt, imagePosXInt) = sqrt(pow(reRotatedVector.at<float>(0, 0),2) + pow(reRotatedVector.at<float>(1, 0),2) +pow(reRotatedVector.at<float>(2, 0),2));
        }
      }

  });





//  auto time_after_stepping = boost::posix_time::microsec_clock::universal_time();
//  boost::posix_time::time_duration measured_step_time_unitless = time_after_stepping - start_time;
//  double elapsedMilliSeconds = measured_step_time_unitless.total_microseconds()  / 1000.0;
//  std::cout<<"Calculation of egocentric representation took: " << elapsedMilliSeconds << " ms" <<std::endl;

  return returnMat;
}


cv::Mat cedar::proc::steps::AllocentricToDistanceImage::calculateRotationMatrix(float rollRad, float tiltRad,
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

std::vector<float> cedar::proc::steps::AllocentricToDistanceImage::calculateTranslationFromJointToCam(
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


void cedar::proc::steps::AllocentricToDistanceImage::outputSizeChanged()
{
  //std::cout<<"OutputSizeChanged >> RecalculateOutputData" << std::endl;
  cv::Mat outPutMat = cv::Mat::zeros(mOutputSizes->getValue().at(0), mOutputSizes->getValue().at(1) , CV_32F);
  mEgoOutput->setData(outPutMat);

  //Recalculate the Output, if input is present
  this->onTrigger();
}
