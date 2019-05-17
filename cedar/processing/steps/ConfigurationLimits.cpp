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

 File:        ConfigurationLimits.cpp

 Maintainer:  
 Email:       
 Date:        

 Description: Source file for the class cedar::proc::steps::ConfigurationLimits.

 Credits:

 ======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/ConfigurationLimits.h"

// CEDAR INCLUDES
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"

#include "cedar/devices/exceptions.h"
#include "cedar/devices/KinematicChain.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// declaration
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration(new ElementDeclarationTemplate<cedar::proc::steps::ConfigurationLimits>("Robotics", "cedar.processing.steps.ConfigurationLimits"));
    declaration->setIconPath(":/steps/configuration_limits.svg");
    declaration->setDescription( "Return the upper/lower limits of joints, joint velocities and accelerations. Needs to be connected to a Component (Robot).");
    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::ConfigurationLimits::ConfigurationLimits()
    :
      cedar::proc::Step(true),
      mOutputPosUpper(new cedar::aux::MatData(cv::Mat())),
      mOutputPosLower(new cedar::aux::MatData(cv::Mat())),
      mOutputVelocityUpper(new cedar::aux::MatData(cv::Mat())),
      mOutputVelocityLower(new cedar::aux::MatData(cv::Mat())),
      mOutputAccelerationUpper(new cedar::aux::MatData(cv::Mat())),
      mOutputAccelerationLower(new cedar::aux::MatData(cv::Mat())),
      _mComponent(new cedar::dev::ComponentParameter(this, "component"))
{
  QObject::connect(this->_mComponent.get(), SIGNAL(valueChanged()), this, SLOT(rebuildOutputs()));
}

cedar::proc::steps::ConfigurationLimits::~ConfigurationLimits()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::steps::ConfigurationLimits::compute(const cedar::proc::Arguments&)
{
  if (this->hasComponent())
  {
    auto component = this->getComponent();
    this->testStates(component);

    cedar::dev::KinematicChainPtr kinChain = boost::dynamic_pointer_cast < cedar::dev::KinematicChain > (component);
    if (kinChain)
    {

      cv::Mat mat = kinChain->getUpperJointLimits();

      mOutputPosUpper->setData(kinChain->getUpperJointLimits());
/*      
      mOutputPosLower->setData(kinChain->getLowerJointLimits());
      mOutputVelocityUpper->setData(kinChain->getUpperVelocityLimits());
      mOutputVelocityLower->setData(kinChain->getLowerVelocityLimits());
      mOutputAccelerationUpper->setData(kinChain->getUpperAccelerationLimits());
      mOutputAccelerationLower->setData(kinChain->getLowerAccelerationLimits());
*/
      // todo: doesnt need to be done in every compute()
    }
  }
}

void cedar::proc::steps::ConfigurationLimits::onStart()
{
  if (this->hasComponent())
  {
    auto component = this->getComponent();
    testStates(component);
  }
}

void cedar::proc::steps::ConfigurationLimits::onStop()
{

  if (this->hasComponent())
  {
    auto component = this->getComponent();
    if (component->isCommunicating())
    {
#if 0      
      cedar::aux::LogSingleton::getInstance()->warning(component->prettifyName() + " is still connected and running.", CEDAR_CURRENT_FUNCTION_NAME);
#endif      
    }
    else
    {
      this->resetState();
    }
  }
}

bool cedar::proc::steps::ConfigurationLimits::hasComponent() const
{
  try
  {
    return this->_mComponent->hasComponentSlot() && static_cast<bool>(this->_mComponent->getValue());
  }
  catch (const cedar::dev::NoComponentSelectedException&)
  {
    return false;
  }
}

void cedar::proc::steps::ConfigurationLimits::reset()
{
  auto component = this->getComponent();
  if (component)
  {
    component->clearAll();
  }
}

void cedar::proc::steps::ConfigurationLimits::rebuildOutputs()
{
  this->removeAllSlots(cedar::proc::DataRole::OUTPUT);
  auto component = this->getComponent();
  cedar::dev::KinematicChainPtr kinChain = boost::dynamic_pointer_cast < cedar::dev::KinematicChain > (component);
  if (kinChain)
  {
    mOutputPosUpper->setData(cv::Mat::zeros(kinChain->getNumberOfJoints(), 1, CV_32FC1));
    mOutputPosLower->setData(cv::Mat::zeros(kinChain->getNumberOfJoints(), 1, CV_32FC1));
    mOutputVelocityUpper->setData(cv::Mat::zeros(kinChain->getNumberOfJoints(), 1, CV_32FC1));
    mOutputVelocityLower->setData(cv::Mat::zeros(kinChain->getNumberOfJoints(), 1, CV_32FC1));
    mOutputAccelerationUpper->setData(cv::Mat::zeros(kinChain->getNumberOfJoints(), 1, CV_32FC1));
    mOutputAccelerationLower->setData(cv::Mat::zeros(kinChain->getNumberOfJoints(), 1, CV_32FC1));

    this->declareOutput("upper joint limits", mOutputPosUpper);
    this->declareOutput("lower joint limits", mOutputPosLower);

    this->declareOutput("upper velocity limits", mOutputVelocityUpper);
    this->declareOutput("lower velocity limits", mOutputVelocityLower);

    this->declareOutput("upper acceleration limits", mOutputAccelerationUpper);
    this->declareOutput("lower acceleration limits", mOutputAccelerationLower);
  }
}

void cedar::proc::steps::ConfigurationLimits::testStates(cedar::dev::ComponentPtr component)
{
  if (!component->isCommunicating())
  {
    this->setState(cedar::proc::Triggerable::STATE_INITIALIZING, component->prettifyName() + " is not connected, yet. Open the Robot Manager to connect.");
  }
  else if (!component->isReadyForMeasurements())
  {
    this->setState(cedar::proc::Triggerable::STATE_INITIALIZING, component->prettifyName() + " is not ready to receive measurements, yet.");
  }
  else
  {
    this->resetState();
  }
}

