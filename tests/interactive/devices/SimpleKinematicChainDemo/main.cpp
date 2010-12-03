/*------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        KinematicChainTest.cpp

 ----- Author:      Bjoern Weghenkel
 ----- Email:       bjoern.weghenkel@ini.rub.de
 ----- Date:        2010 11 23

 ----- Description:

 ----- Credits:
 -----------------------------------------------------------------------------*/

#include "cedar/devices/robot/KinematicChain.h"
#include "cedar/devices/robot/ReferenceGeometry.h"
#include "boost/smart_ptr.hpp"

using namespace std;

//class MyDemoChain : public cedar::dev::robot::KinematicChain
//{
//
//private:
//
//  vector<double> *mpAngles;
//
//public:
//
//  MyDemoChain() : KinematicChain(2,1000) {
//    mpAngles = new vector<double>( mNumberOfJoints, 0.0 );
//    return;
//  }
//
//  ~MyDemoChain() {
//    delete mpAngles;
//  }
//
//  double getJointAngle(unsigned int index) const {
//    return (*mpAngles)[index];
//  }
//
//  vector<double> getJointAngles() const {
//    vector<double> dummy( *mpAngles );
//    return dummy;
//  }
//
//  cv::Mat getJointAnglesMatrix() const {
//    cv::Mat dummy(mNumberOfJoints, 1, CV_32F);
//    for (unsigned i = 0; i < mNumberOfJoints; i++)
//      dummy.at<double>(i,0) = (*mpAngles)[i];
//    return dummy;
//  }
//
//  void setJointAngleOnDevice(unsigned int index, double angle) {
//    (*mpAngles)[index] = angle;
//    cout << " ( " << (*mpAngles)[0] << ", " << (*mpAngles)[1] << " )" << endl;
//  }
//
///*
//  void setJointAngles(const vector<double>& angles) {
//    (*mpAngles) = angles;
//  }
//
//  void setJointAngles(const cv::Mat& angleMatrix) {
//    for(unsigned i = 0; i < mNumberOfJoints; i++)
//      (*mpAngles)[i] = angleMatrix.at<double>(i,0);
//  }
//*/
//
//};

using namespace cedar::dev::robot;
using namespace boost;

int main() {
//  cout << "main" << endl;
//  MyDemoChain myDemoChain;
//  ReferenceGeometryPtr pGeometry( new ReferenceGeometry("/home/weghebvc/workspace/cedar/tests/interactive/devices/SimpleKinematicChainDemo/test_arm.conf") );
//  myDemoChain.setReferenceGeometry( pGeometry );
//  cout << "starting ..." << endl;
//  myDemoChain.start();
//  myDemoChain.setJointVelocity(0, 1.0);
//  cout << "stoping ..." << endl;
//  myDemoChain.stop(5000);
//  cout << "stoped" << endl;
  return 0;
}
