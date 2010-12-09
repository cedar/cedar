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
#include "cedar/auxiliaries/exceptions/BadConnectionException.h"

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
ConfigurationInterface(configFileName)
{
  mIsInit = false;
  mpFriRemote = 0;
  init();
}

KukaInterface::~KukaInterface()
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
void KukaInterface::init()
{
  //The number of joints the KUKA LBR has
  KinematicChain::mNumberOfJoints = LBR_MNJ;
  //Load Parameters from the configuration file
  //ServerPort: 0 means, FRI will use the default Port
  addParameter(&_mServerPort, "ServerPort", 0);
  //RemoteHost: 127.0.0.1 overwrites the FRI default
  //TODO define a commandline parameter that can be NULL
  addParameter(&_mRemoteHost, "RemoteHost", "127.0.0.1");

  //create a new Instance of the friRemote
  mpFriRemote = new friRemote(_mServerPort, _mRemoteHost.c_str());

  /* Do Handshakes to the remote host until the robot is in command mode*/
  while(mpFriRemote->getState() != FRI_STATE_CMD)
  {
    mpFriRemote->setToKRLInt(0, 1);
    mpFriRemote->doDataExchange();
  }
  mIsInit = true;
}

const double KukaInterface::getJointAngle(const unsigned int index)const
{
  //Receive data from the Kuka LBR
  mpFriRemote->doReceiveData();
  //does not test if index is out of bounds yet
  return (double)mpFriRemote->getMsrMsrJntPosition()[index];
}
const vector<double> KukaInterface::getJointAngles() const
{
  //Receive data from the Kuka LBR
  mpFriRemote->doReceiveData();
  //Create a std::vector from the float-Array
  float *pJointPos = mpFriRemote->getMsrMsrJntPosition();
  return vector<double>(pJointPos, pJointPos + getNumberOfJoints());
}
const cv::Mat KukaInterface::getJointAnglesMatrix() const
{
  //This may be inefficient, but heck, the bloody udp-communication is imho more slow than this
  return cv::Mat(getJointAngles(), true);
}
void KukaInterface::setJointAngle(const unsigned int index, const double angle) throw()
{
  //If the KUKA/LBR is not in command mode, throw an Exception
  commandModeTest();

  //We want to move exactly one joint. Therefore, the other joints must have the same commanded Positions as before.
  float *p_angles = mpFriRemote->getMsrCmdJntPosition();
  p_angles[index] = float(angle);
  mpFriRemote->doPositionControl(p_angles, true);
}
void KukaInterface::setJointAngles(const std::vector<double>& angles) throw()
{
  //If the KUKA/LBR is not in command mode, throw an Exception
  commandModeTest();

  //The FRI function is expecting a float array, so I have to allocate a temporary array.
  float *p_angles = new float[getNumberOfJoints()];

  for (unsigned i=0; i<getNumberOfJoints(); i++)
  {
    p_angles[i] = float(angles[i]);
  }

  //Although the first parameter is not const, this FRI function copies the content and does not change
  //Second Parameter defines if the Data should be transfered to the LBR right now.
  mpFriRemote->doPositionControl(p_angles, true);

  delete[] p_angles;
}
void KukaInterface::setJointAngles(const cv::Mat& angleMatrix) throw()
{
  //If the KUKA/LBR is not in command mode, throw an Exception
  commandModeTest();

  //The cv::Mat Matrix has a template method to return a specific type.
  //I don't know if it works without problems, though
  const float *p_angles = angleMatrix.ptr<float>();
  //since the fri expects a non-constant array, I have to copy the array.
  float *p_angles_copy = new float[getNumberOfJoints()];
  for(unsigned i=0; i<getNumberOfJoints(); i++)
  {
    p_angles_copy[i] = p_angles[i];
  }

  //Al
  mpFriRemote->doPositionControl(p_angles_copy, true);

  delete[] p_angles_copy;
}

FRI_STATE KukaInterface::getFriState()const
{
  return mpFriRemote->getState();
}

FRI_QUALITY KukaInterface::getFriQuality()const
{
  return mpFriRemote->getQuality();
}
void doDataExchange()
{
  mpFriRemote->doDataExchange();
}

void KukaInterface::commandModeTest()const throw()
{
  if(mpFriRemote->getState() != FRI_STATE_CMD)
    {
      CEDAR_THROW(aux::exc::BadConnectionException,
                  "KUKA LBR is not in command mode. This may mean the connection "\
                  "is not good enough or command mode has been stopped by the robot"
                 );
    }
}
