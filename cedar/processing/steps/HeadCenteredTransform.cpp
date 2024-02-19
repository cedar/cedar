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

    File:        HeadCenteredTransform.fwd.h

    Maintainer:  Stephan Sehring
    Email:       stephan.sehring@rub.de
    Date:        2024 02 16

    Description: Caren camera to head centered transform

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/HeadCenteredTransform.h"

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
                      new ElementDeclarationTemplate<cedar::proc::steps::HeadCenteredTransform>
                              (
                                      "Image Processing",
                                      "cedar.processing.HeadCenteredTransform"
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

cedar::proc::steps::HeadCenteredTransform::HeadCenteredTransform():
mTiltJointDistance(new cedar::aux::DoubleParameter (this,"cam distance to tiltjoint (m)",0.097)),
mCameraFrustrumAngleDeg(new cedar::aux::DoubleVectorParameter(this,"camera frustrum angle (deg)",2,90)),
mOutputSizes(new cedar::aux::IntVectorParameter(this,"output sizes",2,180))
{
  std::vector<int> defaultSizes {mOutputSizes->getValue().at(0),mOutputSizes->getValue().at(1)};
  mAlloOutput = cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat::zeros(2, &defaultSizes.at(0), CV_32F)));

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

cedar::proc::steps::HeadCenteredTransform::~HeadCenteredTransform()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::HeadCenteredTransform::compute(const cedar::proc::Arguments&)
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

void cedar::proc::steps::HeadCenteredTransform::inputConnectionChanged(const std::string& inputName)
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


cv::Mat cedar::proc::steps::HeadCenteredTransform::computeAllocentricRepresentation(cv::Mat distanceImage,float rollRad,float tiltRad, float panRad)
{
//  auto start_time = boost::posix_time::microsec_clock::universal_time();

  std::vector<int> outPutSizes {mOutputSizes->getValue().at(0),mOutputSizes->getValue().at(1)};
  cv::Mat returnMat = cv::Mat(2, &outPutSizes.at(0), CV_32F,cv::Scalar(0));

  // For OpenCV x and y are actually swapped
  float angleRangeX = this->mCameraFrustrumAngleDeg->getValue().at(0) * (M_PI/180.0); // Convert to radians
  float xPixels = (float) distanceImage.cols;

  float angleRangeY = this->mCameraFrustrumAngleDeg->getValue().at(1) * (M_PI/180.0); // Convert to radians
  float yPixels = (float) distanceImage.rows;


  cv::Matx33f jointRotationMat = calculateRotationMatrix(rollRad,tiltRad,panRad);
  
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
  distanceImage.forEach<float>([&returnMat,tiltRad,panRad,angleRangeX,xPixels,angleRangeY,yPixels,r_jc,r_cd,l_cd,invCamMat,camMat,this](float &p, const int * position) -> void {
            float yImage = (float) position[0]; //row,y,tilt //OpenCV has a weird depiction of row and column in images...
            float xImage = (float) position[1]; //col,x,pan
            float distance = p;

            cv::Vec3f pix_vec(xImage,yImage,1);
            cv::Mat r_ego4_mat = invCamMat * pix_vec;
            cv::Vec4f r_ego4 = r_ego4_mat.col(0);
            cv::Vec3f r_ego(r_ego4(1), r_ego4(0), r_ego4(2));
            r_ego = r_ego/sqrt( pow(r_ego(0),2) + pow(r_ego(1),2) + pow(r_ego(2),2) );

            float cam_pan = atan2(r_cd(1), r_cd(0));
            float cam_tilt = acos( r_cd(2) / 1.0 );
            r_ego = this->calculateRotationMatrix(0., cam_tilt, cam_pan) * r_ego;

            // angle offset of pixel on image from middle 
            //float XAngle = xImage/xPixels*angleRangeX - angleRangeX/2.;
            //float YAngle = yImage/yPixels*angleRangeY - angleRangeY/2.;

            // calculate vector from cam to point on image
            //float point_pan = atan2(r_cd(1), r_cd(0)) + XAngle; //current camera tilt and pan plus offset
            //float point_tilt = acos( r_cd(2) / 1.0 ) + YAngle; // length of r_cd already normalized acos(r_cd/|r_cd|)

            //cv::Vec3f r_ego(sin(point_tilt)*cos(point_pan), sin(point_tilt)*sin(point_pan), cos(point_tilt));
            r_ego = r_ego * distance;
            //cv::Vec3f r_ego = distance * this->calculateRotationMatrix(0., YAngle, XAngle) * r_cd;
            
            // calculate vector from joint to point
            cv::Vec3f r_allo = r_jc + r_ego;
            float l_allo = sqrt( pow(r_allo(0),2) + pow(r_allo(1),2) + pow(r_allo(2),2) );
            
            // compute head centered angles.
            float pan = atan2(r_allo(1), r_allo(0));
            float tilt = acos(r_allo(2)/l_allo);

            // calculate field positions
            int outPutX = (int) round((pan+M_PI)/M_PI/2.0 * (float)returnMat.size[1]); // pan in range o,2pi
            int outPutY = (int) round(tilt/M_PI * (float)returnMat.size[0]);   // tilt in range 0,pi

            if(outPutX<returnMat.size[1] && outPutX >=0 && outPutY < returnMat.size[0] && outPutY >=0)
            {
              //Again the OpenCV Plot treats the first two dimensions in the plot different than usual (for me)
              returnMat.at<float>(outPutY, outPutX) = l_allo;
            }

            //debug code...
            if (p > 0.097)
            {
              std::cout << "joint angles: (" << panRad/M_PI * 180 << ", " << tiltRad/M_PI * 180 << ") -> (pan,tilt)" << std::endl;
              std::cout << "r_jc = " << r_jc << std::endl;
              std::cout << "r_cd = " << r_cd << std::endl;
              std::cout << "camMat = " << camMat << std::endl;
              std::cout << "invcamMat = " << invCamMat << std::endl;
              std::cout << "pixel: (" << xImage << ", " << yImage << ", " << distance << ") -> (col,row,d)" << std::endl;
              std::cout << "pixel vec = " << pix_vec << std::endl;
              std::cout << "ego4 = " << r_ego4<< std::endl;
              std::cout << "restored pix vec = " << camMat*r_ego4 << std::endl;

              //std::cout << "angle offset: (" << XAngle/M_PI * 180 << ", " << YAngle/M_PI * 180 << ") -> (pan,tilt)" << std::endl;
              //std::cout << "point pan: "<< point_pan/M_PI * 180 << std::endl;
              //std::cout << "point tilt: " << point_tilt/M_PI * 180 << std::endl;
              std::cout << "r_rego = " << r_ego << std::endl;
              std::cout << "r_allo = " << r_allo << std::endl;
              std::cout << "angle allo: (" << pan /M_PI * 180<< ", " << tilt/M_PI * 180 << ")  -> (pan,tilt)" << std::endl;
              std::cout << "output pos: (" << outPutX << ", " << outPutY << ") -> (col, row)" << std::endl;
              std::cout << "-------------------------------------->" << std::endl;
            }

          });

  return returnMat;
}


cv::Matx33f cedar::proc::steps::HeadCenteredTransform::calculateRotationMatrix(float rollRad, float tiltRad,
                                                                                      float panRad)
{

  cv::Matx33f rollMatrix( 1, 0, 0, 0, cosf(rollRad), -sinf(rollRad), 0, sinf(rollRad), cosf(rollRad) ); //Rx

  cv::Matx33f tiltMatrix(cosf(tiltRad), 0, sinf(tiltRad), 0, 1, 0, -sinf(tiltRad), 0, cosf(tiltRad)); //Ry
  
  cv::Matx33f panMatrix( cosf(panRad), -sinf(panRad), 0, sinf(panRad), cosf(panRad), 0, 0, 0, 1 ); //Rz

  cv::Matx33f rotationMatrix = rollMatrix * panMatrix *  tiltMatrix;

  return rotationMatrix;
}

void cedar::proc::steps::HeadCenteredTransform::outputSizeChanged()
{
//  std::cout<<"OutputSizeChanged >> RecalculateOutputData" << std::endl;
  std::vector<int> outPutSizes {mOutputSizes->getValue().at(0),mOutputSizes->getValue().at(1)};
  cv::Mat outPutMat = cv::Mat::zeros(2, &outPutSizes.at(0), CV_32F);
  mAlloOutput->setData(outPutMat);

  //Recalculate the Output, if input is present
  this->onTrigger();
}
