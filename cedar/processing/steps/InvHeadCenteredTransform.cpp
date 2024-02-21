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

    File:        InvHeadCenteredTransform.fwd.h

    Maintainer:  Stephan Sehring
    Email:       stephan.sehring@rub.de
    Date:        2024 02 16

    Description: Caren camera to head centered transform

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/InvHeadCenteredTransform.h"

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
                      new ElementDeclarationTemplate<cedar::proc::steps::InvHeadCenteredTransform>
                              (
                                      "Image Processing",
                                      "cedar.processing.InvHeadCenteredTransform"
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

cedar::proc::steps::InvHeadCenteredTransform::InvHeadCenteredTransform():
mTiltJointDistance(new cedar::aux::DoubleParameter (this,"cam distance to tiltjoint (m)",0.097)),
mCameraFrustrumAngleDeg(new cedar::aux::DoubleVectorParameter(this,"camera frustrum angle (deg)",2,90)),
mOutputSizes(new cedar::aux::IntVectorParameter(this,"output sizes",2,180)),
mCameraFocalLength(new cedar::aux::DoubleParameter (this, "camera focal length (m)",0.01))
{
  std::vector<int> defaultSizes {mOutputSizes->getValue().at(0),mOutputSizes->getValue().at(1)};
  mEgoOutput = cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat::zeros(2, &defaultSizes.at(0), CV_32F)));

  // declare all inputs
  cedar::proc::DataSlotPtr imageInput = this->declareInput(mAlloInputName, true);
  cedar::proc::DataSlotPtr rollInput = this->declareInput(mRollInputName, false);
  cedar::proc::DataSlotPtr tiltInput = this->declareInput(mTiltInputName, false);
  cedar::proc::DataSlotPtr panInput = this->declareInput(mPanInputName, false);

  // declare output
  auto weightTriggerOutput = this->declareOutput(mEgoOutputName, mEgoOutput);

  // setup connections
  QObject::connect(mOutputSizes.get(), SIGNAL(valueChanged()),this, SLOT(outputSizeChanged()));

}

cedar::proc::steps::InvHeadCenteredTransform::~InvHeadCenteredTransform()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::InvHeadCenteredTransform::compute(const cedar::proc::Arguments&)
{
  //Do something if all inputs are present
  if (mAlloInput)
  {
    float rollRad = this->mRollInput ? this->mRollInput->getData().at<float>(0,0) * (M_PI/180.0) : 0; //Rotation along Roll if present
    float tiltRad = this->mTiltInput ? this->mTiltInput->getData().at<float>(0,0) * (M_PI/180.0) : 0; // This is rotation along pitch in Rad if present
    float panRad = this->mPanInput ? this->mPanInput->getData().at<float>(0,0) * (M_PI/180.0) :0; // This is rotation along yaw in Rad if present

    cv::Mat outputMat = computeEgocentricRepresentation(mAlloInput->getData(), rollRad,tiltRad,panRad);

    mEgoOutput->setData(outputMat);
  }
}

void cedar::proc::steps::InvHeadCenteredTransform::inputConnectionChanged(const std::string& inputName)
{
  if (inputName == mAlloInputName)
  {
    this->mAlloInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
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


cv::Mat cedar::proc::steps::InvHeadCenteredTransform::computeEgocentricRepresentation(cv::Mat HeadCenteredRep,float rollRad,float tiltRad, float panRad)
{
//  auto start_time = boost::posix_time::microsec_clock::universal_time();

  std::vector<int> outPutSizes {mOutputSizes->getValue().at(0),mOutputSizes->getValue().at(1)};
  float angleRangeX = this->mCameraFrustrumAngleDeg->getValue().at(0) * (M_PI/180.0); // Convert to radians
  float angleRangeY = this->mCameraFrustrumAngleDeg->getValue().at(1) * (M_PI/180.0); // Convert to radians
  cv::Mat returnMat = cv::Mat(2, &outPutSizes.at(0), CV_32F,cv::Scalar(0));

  // For OpenCV x and y are actually swapped
  float panSampling = (float) HeadCenteredRep.cols;
  float tiltSampling = (float) HeadCenteredRep.rows;

  float xPixels = mOutputSizes->getValue().at(0);
  float yPixels = mOutputSizes->getValue().at(1);


  cv::Matx33f jointRotationMat = calculateRotationMatrix2(rollRad,tiltRad,panRad);
  
  cv::Vec3f r_jc_def(0,0,(float) this->mTiltJointDistance->getValue()); //default cam offset
  cv::Vec3f r_cd_def(0,1.0,0); // default vector in plane of camera platform

  cv::Vec3f r_jc = jointRotationMat * r_jc_def; // current cam offset
  cv::Vec3f r_cd = -r_jc.cross(jointRotationMat*r_cd_def); // current cam direction
  float l_cd = sqrt( pow(r_cd(0), 2) + pow(r_cd(1), 2) + pow(r_cd(2), 2));
  r_cd = r_cd/l_cd; // normalize

  // calculate camera matrix
  float f = 0.01;
  float s_x = xPixels/(2*f*tan(angleRangeX/2));
  float s_y = yPixels/(2*f*tan(angleRangeY/2));
  float u_0 = xPixels/2;
  float v_0 = yPixels/2;
  cv::Mat camMat(3,4,CV_32F, cv::Scalar(0));
  camMat.at<float>(0,0) = s_x*f;
  camMat.at<float>(0,2) = u_0;
  camMat.at<float>(1,1) = s_y*f;
  camMat.at<float>(1,2) = v_0;
  camMat.at<float>(2,2) = 1;
  cv::Mat invCamMat = camMat.inv(cv::DecompTypes::DECOMP_SVD);

 //Parallelizing saves 30 ms per iteration on a 300x300 depthImage on my VM
  HeadCenteredRep.forEach<float>([&returnMat,tiltRad,panRad,angleRangeX,panSampling,angleRangeY,tiltSampling,r_jc,r_cd,l_cd,invCamMat,camMat,this](float &p, const int * position) -> void {
            float tiltPos = (float) position[0]; //row,y,tilt //OpenCV has a weird depiction of row and column in images...
            float panPos = (float) position[1]; //col,x,pan
            float distance = p;

            float pointTilt = tiltPos/tiltSampling * M_PI; //range 0 to pi
            float pointPan = panPos/panSampling * 2 * M_PI - M_PI; // range -pi to pi

            cv::Vec4f r_allo(sin(pointTilt)*cos(pointPan) ,sin(pointTilt)*sin(pointPan) ,cos(pointTilt) ,1);
            r_allo = r_allo * distance;
            r_allo(3) = r_allo(3)/distance;
            cv::Vec4f r_ego;
            float cam_pan = atan2(r_cd(1), r_cd(0));
            float cam_tilt = acos( r_cd(2) / 1.0 );
            r_ego = this->calculateExtrinsicMatrix(0., cam_tilt, cam_pan, r_jc) * r_allo;

            //cv::Vec4f r_ego4(-r_ego(0), -r_ego(1), -r_ego(2), 1);
            cv::Mat pix_mat = camMat * r_ego;
            cv::Vec3f pix_vec = pix_mat.col(0);
            float d = pix_vec(2);
            pix_vec = pix_vec/pix_vec(2);
            

            // calculate field positions
            int outPutX = (int) round(pix_vec(1));
            int outPutY = (int) round(pix_vec(0));

            if(outPutX<returnMat.size[1] && outPutX >=0 && outPutY < returnMat.size[0] && outPutY >=0)
            {
              //Again the OpenCV Plot treats the first two dimensions in the plot different than usual (for me)
              returnMat.at<float>(outPutY, outPutX) = d;
            }

            //debug code...
            /*if (tiltPos == 142 && panPos==80)
            {
              std::cout << "joint angles: (" << panRad/M_PI * 180 << ", " << tiltRad/M_PI * 180 << ") -> (pan,tilt)" << std::endl;
              std::cout << "r_jc = " << r_jc << std::endl;
              std::cout << "r_cd = " << r_cd << std::endl;
              std::cout << "camMat = " << camMat << std::endl;
            //  std::cout << "invcamMat = " << invCamMat << std::endl;
            std::cout << "point Tilt" << pointTilt/M_PI*180 << std::endl;
            std::cout << "point Pan" << pointPan/M_PI*180 << std::endl;
            std::cout << "r_allo " << r_allo << std::endl;
            std::cout << "cam pan " << cam_pan/M_PI*180 << std::endl; 
            std::cout << "cam tilt " << cam_tilt/M_PI*180 << std::endl; 
            std::cout << "extrinsic mat " << this->calculateExtrinsicMatrix(0., cam_tilt, cam_pan, r_jc) << std::endl; 
            std::cout << "r_ego" << r_ego << std::endl;
            //std::cout << "r_ego4" << r_ego4 << std::endl;
            std::cout << "pix_vec" << pix_vec << std::endl;
            std::cout << "-------------------------------------->" << std::endl;
            }*/

          });

  return returnMat;
}


cv::Matx33f cedar::proc::steps::InvHeadCenteredTransform::calculateRotationMatrix(float rollRad, float tiltRad,
                                                                                      float panRad)
{

  cv::Matx33f rollMatrix( 1, 0, 0, 0, cosf(rollRad), -sinf(rollRad), 0, sinf(rollRad), cosf(rollRad) ); //Rx

  cv::Matx33f tiltMatrix(cosf(tiltRad), 0, sinf(tiltRad), 0, 1, 0, -sinf(tiltRad), 0, cosf(tiltRad)); //Ry
  
  cv::Matx33f panMatrix( cosf(panRad), -sinf(panRad), 0, sinf(panRad), cosf(panRad), 0, 0, 0, 1 ); //Rz

  cv::Matx33f rotationMatrix = rollMatrix * tiltMatrix *  panMatrix;

  return rotationMatrix;
}

cv::Matx33f cedar::proc::steps::InvHeadCenteredTransform::calculateRotationMatrix2(float rollRad, float tiltRad,
                                                                                      float panRad)
{

  cv::Matx33f rollMatrix( 1, 0, 0, 0, cosf(rollRad), -sinf(rollRad), 0, sinf(rollRad), cosf(rollRad) ); //Rx

  cv::Matx33f tiltMatrix(cosf(tiltRad), 0, sinf(tiltRad), 0, 1, 0, -sinf(tiltRad), 0, cosf(tiltRad)); //Ry
  
  cv::Matx33f panMatrix( cosf(panRad), -sinf(panRad), 0, sinf(panRad), cosf(panRad), 0, 0, 0, 1 ); //Rz

  cv::Matx33f rotationMatrix = rollMatrix * panMatrix *  tiltMatrix;

  return rotationMatrix;
}

cv::Matx44f cedar::proc::steps::InvHeadCenteredTransform::calculateExtrinsicMatrix(float rollRad, float tiltRad,
                                                                                      float panRad, cv::Vec3f translation)
{
  cv::Matx44f extrinsicMatrix; 
  cv::Matx33f inverseRotation = this->calculateRotationMatrix2(rollRad, tiltRad, panRad).inv(cv::DecompTypes::DECOMP_SVD);
  cv::Vec3f rotated_translation = inverseRotation * translation * -1;
  extrinsicMatrix(0,0) = inverseRotation(0,0);
  extrinsicMatrix(1,0) = inverseRotation(1,0);
  extrinsicMatrix(2,0) = inverseRotation(2,0);
  extrinsicMatrix(0,1) = inverseRotation(0,1);
  extrinsicMatrix(1,1) = inverseRotation(1,1);
  extrinsicMatrix(2,1) = inverseRotation(2,1);
  extrinsicMatrix(0,2) = inverseRotation(0,2);
  extrinsicMatrix(1,2) = inverseRotation(1,2);
  extrinsicMatrix(2,2) = inverseRotation(2,2);
  
  extrinsicMatrix(0,3) = rotated_translation(0);
  extrinsicMatrix(1,3) = rotated_translation(1);
  extrinsicMatrix(2,3) = rotated_translation(2);

  extrinsicMatrix(3,0) = 0;
  extrinsicMatrix(3,1) = 0;
  extrinsicMatrix(3,2) = 0;
  extrinsicMatrix(3,3) = 1;

  return extrinsicMatrix;

}

void cedar::proc::steps::InvHeadCenteredTransform::outputSizeChanged()
{
//  std::cout<<"OutputSizeChanged >> RecalculateOutputData" << std::endl;
  std::vector<int> outPutSizes {mOutputSizes->getValue().at(0),mOutputSizes->getValue().at(1)};
  cv::Mat outPutMat = cv::Mat::zeros(2, &outPutSizes.at(0), CV_32F);
  mEgoOutput->setData(outPutMat);

  //Recalculate the Output, if input is present
  this->onTrigger();
}
