/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        KukaInterface.cpp

 ----- Author:      Guido Knips

 ----- Email:       guido.knips@ini.rub.de

 ----- Date:        2010 11 23

 ----- Description:

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "KukaInterface.h"

// PROJECT INCLUDES
#include "auxiliaries/exceptions/BadConnectionException.h"
#include "auxiliaries/exceptions/IndexOutOfRangeException.h"

// SYSTEM INCLUDES
#ifdef DEBUG
#include <iostream>
#endif

using namespace cedar::dev::robot::kuka;
using namespace std;
using namespace libconfig;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
KukaInterface::KukaInterface(const string& configFileName, const std::string& communicatorConfigFileName)
:
cedar::dev::robot::KinematicChain(configFileName)
{
  mIsInit = false;
  mpCommunicator = 0;
  mpCommunicator = new KukaCommunicator(communicatorConfigFileName);
  init();
}

KukaInterface::~KukaInterface()
{
  if(mIsInit)
  {
    //Free Memory
    if(mpCommunicator)
    {
      delete mpCommunicator;
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
// member functions
//----------------------------------------------------------------------------------------------------------------------
void KukaInterface::init()
{
  mIsInit = true;
}

double KukaInterface::getJointAngle(const unsigned int index)const
{
  //The index must be less than the number of angles
  if (index >= getNumberOfJoints() )
  {
    CEDAR_THROW(aux::exc::IndexOutOfRangeException, "KukaInterface: invalid joint index");
  }
  return mpCommunicator->getJointAngle(index);
}
vector<double> KukaInterface::getJointAngles() const
{
  return mpCommunicator->getJointAngles();
}
cv::Mat KukaInterface::getJointAnglesMatrix() const
{
  //This may be inefficient... but I'm lazy
  return cv::Mat(mpCommunicator->getJointAngles(), true);
}
void KukaInterface::setJointAngle(const unsigned int index, const double angle)
{
  //The index must be less than the number of angles
  if (index >= getNumberOfJoints() )
  {
    CEDAR_THROW(aux::exc::IndexOutOfRangeException, "KukaInterface: invalid joint index");
  }

  //only do this if power is on and robot is in CommandMode
  if (isPowerOn() && getFriState() == FRI_STATE_CMD)
  {
    mpCommunicator->setJointAngle(index, angle);
  }
}
void KukaInterface::setJointAngles(const std::vector<double>& angles)
{
  //You can only move the arm if power is on
  if (isPowerOn() && getFriState() == FRI_STATE_CMD)
  {
    mpCommunicator->setJointAngles(angles);
  }
}
void KukaInterface::setJointAngles(const cv::Mat& angleMatrix)
{
  //You can only move the arm if power is on
  if(isPowerOn() && getFriState() == FRI_STATE_CMD)
  {
    vector<double> angles(LBR_MNJ);
    for(int i=0; i<LBR_MNJ; i++)
    {
      angles[i] = angleMatrix.at<double>(i,0);
    }
    mpCommunicator->setJointAngles(angles);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// wrapped fri-functions
//----------------------------------------------------------------------------------------------------------------------
FRI_STATE KukaInterface::getFriState()const
{
  return mpCommunicator->getFriState();
}

FRI_QUALITY KukaInterface::getFriQuality()const
{
  return mpCommunicator->getFriQuality();
}
float KukaInterface::getSampleTime()const
{
  return mpCommunicator->getSampleTime();
}
bool KukaInterface::isPowerOn()const{
  return mpCommunicator->isPowerOn();
}

