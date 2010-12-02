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
KukaInterface::KukaInterface(const string& configFileName)
:
SZ::ConfigurationInterface(configFileName)
{
  mIsInit = false;
  mpFriRemote = 0;
  init();
}

KukaInterface::~KukaInterface(void)
{
  if(mIsInit)
  {
    //Free Memory
    if(mpFriRemote)
    {
      delete mpFriRemote;
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
// member functions
//----------------------------------------------------------------------------------------------------------------------
KukaInterface::init()
{
  //The number of joints the KUKA LBR has
  mNumberOfJoints = LBR_MNJ;
  //Load Parameters from the configuration file
  //ServerPort: 0 means, FRI will use the default Port
  addParameter(&_mServerPort, "ServerPort", 0);
  //RemoteHost: 127.0.0.1 overwrites the FRI default, but it should make no difference
  addParameter(&_mRemoteHost, "RemoteHost", "127.0.0.1");

  //create a new Instance of the friRemote
  mpFriRemote = new friRemote(_mServerPort, _mRemoteHost.c_str());

  mIsInit = true;
}

const double KukaInterface::getJointAngle(const unsigned int index)const
{
  //Receive data from the Kuka LBR
  mpFriRemote->doReceiveData();
  //does not test if index is out of bounds yet
  return (double)mpFriRemote->getMsrMsrJntPosition()[i];
}
const vector<double> KukaInterface::getJointAngles() const
{
  //Receive data from the Kuka LBR
  mpFriRemote->doReceiveData();
  //Create a std::vector from the float-Array
  float *pJointPos = mpFriRemote->getMsrMsrJntPosition();
  return vector<double>(pJointPos, pJointPos + getNumberOfJoints());
}
const cv::Mat KukaInterace::getJointAnglesMatrix() const
{
  //This may be inefficient, but heck, the bloody udp-communication is imho more slow than this
  return cv::Mat(getJointAngles(), true);
}
void KukaInterface::setJointAngle(const unsigned int index, const double angle)
{
  //We want to move exactly one joint. Therefore, the other joints must have the same commanded Positions as before.
  float *p_angles = mpFriRemote->getMsrCmdJntPosition();
  p_angles[index] = float(angle);
  mpFriRemote->doPositionControl(p_angles, true);
}
void KukaInterface::setJointAngles(const std::vector<double>& angles)
{
  //The FRI function is expecting a float array, so I have to allocate a temporary array.
  float p_angles[] = new float[getNumberOfJoints()];

  for (int i=0; i<getNumberOfJoints(); i++)
  {
    p_angles[i] = float(angles[i]);
  }

  //This FRI function copies the content of the first Parameter
  //Second Parameter defines if the Data should be transfered to the LBR right now.
  mpFriRemote->doPositionControl(p_angles, true);

  delete[] p_angles;
}
void KukaInterface::setJointAngles(const cv::Mat& angleMatrix)
{
  //The cv::Mat Matrix has a template method to return a specific type.
  //I don't know if it works without problems, though
  float *p_angles = angleMatrix.ptr<float>();
  mpFriRemote->doPositionControl(p_angles, true);
}

