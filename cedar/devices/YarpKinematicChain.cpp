/*======================================================================================================================

 Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

 File:        YarpKinematicChain.cpp

 Maintainer:  Jan Tekülve
 Email:       jan.tekuelve@ini.rub.de
 Date:        2016 06 20

 Description: Source file for the class cedar::dev::YarpKinematicChain.

 Credits:

 ======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_YARP
#include "cedar/devices/YarpKinematicChain.h"
#include "cedar/devices/KinematicChain.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES
#include <QWriteLocker>

// CLASS HEADER

// CEDAR INCLUDES

// SYSTEM INCLUDES



//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered()
  {
    cedar::dev::ComponentManagerSingleton::getInstance()->registerType<cedar::dev::YarpKinematicChainPtr>();
    return true;
  }

  bool registerFnCall = registered();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::YarpKinematicChain::YarpKinematicChain()
    :
    mReadPort(new cedar::aux::StringParameter(this, "readPortName", "readPortYarpChain")),
    mWritePort(new cedar::aux::StringParameter(this, "writePortName", "writePortYarpChain"))
{
  registerCommandHook(cedar::dev::KinematicChain::JOINT_ANGLES, boost::bind(&cedar::dev::YarpKinematicChain::sendAngles, this, _1));
  registerMeasurementHook(cedar::dev::KinematicChain::JOINT_ANGLES, boost::bind(&cedar::dev::YarpKinematicChain::retrieveAngles, this));
  this->applyDeviceSideCommandsAs(cedar::dev::KinematicChain::JOINT_ANGLES);
}

cedar::dev::YarpKinematicChain::~YarpKinematicChain()
{
  prepareComponentDestructAbsolutelyRequired();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::YarpKinematicChain::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  cedar::dev::KinematicChain::readConfiguration(node);

  this->registerPortNames();
}

void cedar::dev::YarpKinematicChain::registerPortNames()
{
//  std::cout<<"Yarp Kinematic Chain adding the Reader and Writer Ports: "<< mReadPort->getValue() << ", "<< mWritePort->getValue() <<std::endl;
  auto yarpChannel = boost::static_pointer_cast< cedar::dev::YarpChannel<cv::Mat> >(this->getChannel());
  if (!yarpChannel)
  {
    cedar::aux::LogSingleton::getInstance()->error("Lost yarpChannel pointer", CEDAR_CURRENT_FUNCTION_NAME);
    return;
  }
  else
  {
    yarpChannel->addReaderPort(mReadPort->getValue());
    yarpChannel->addWriterPort(mWritePort->getValue());
  }
}

void cedar::dev::YarpKinematicChain::sendAngles(cv::Mat mat)
{
  auto yarpChannel = boost::static_pointer_cast < cedar::dev::YarpChannel<cv::Mat> > (this->getChannel());
  if (!yarpChannel)
  {
    cedar::aux::LogSingleton::getInstance()->error("Lost yarpChannel pointer", CEDAR_CURRENT_FUNCTION_NAME);
    return;
  }
  yarpChannel->write(mat, mWritePort->getValue());
}

cv::Mat cedar::dev::YarpKinematicChain::retrieveAngles()
{
  auto yarpChannel = boost::static_pointer_cast < cedar::dev::YarpChannel<cv::Mat> > (this->getChannel());
  if (!yarpChannel)
  {
    cedar::aux::LogSingleton::getInstance()->error("Lost yarpChannel pointer", CEDAR_CURRENT_FUNCTION_NAME);
    return cv::Mat();
  }
  auto ret = yarpChannel->read(mReadPort->getValue());

  return ret;
}

bool cedar::dev::YarpKinematicChain::applyCrashbrake()
{
  return false;
}

#endif // #ifdef CEDAR_USE_YARP

