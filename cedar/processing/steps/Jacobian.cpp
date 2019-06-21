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

 File:        Jacobian

 Maintainer:  
 Email:       
 Date:        

 Description: 

 Credits:

 ======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/Jacobian.h"

// CEDAR INCLUDES
// CEDAR INCLUDES
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"

#include "cedar/devices/exceptions.h"
#include "cedar/devices/KinematicChain.h"
// SYSTEM INCLUDES

namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration(new ElementDeclarationTemplate<cedar::proc::steps::Jacobian>("Robotics", "cedar.processing.steps.Jacobian"));
    declaration->setIconPath(":/steps/jacobian.svg");
    declaration->declare();
    declaration->setDescription
    (
      "Calculates the (kinematic) Jacobian for the current configuration "
      "of a robot. Needs to be connected to a Component (Robot)."
    );

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::Jacobian::Jacobian()
    :
      cedar::proc::Step(true),
      mOutput(new cedar::aux::MatData(cv::Mat())),
      _mComponent(new cedar::dev::ComponentParameter(this, "component"))
{
  QObject::connect(this->_mComponent.get(), SIGNAL(valueChanged()), this, SLOT(rebuildOutputs()));
}

cedar::proc::steps::Jacobian::~Jacobian()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::steps::Jacobian::compute(const cedar::proc::Arguments&)
{
  auto component = this->getComponent();
  this->testStates(component);

  cedar::dev::KinematicChainPtr kinChain = boost::dynamic_pointer_cast < cedar::dev::KinematicChain > (component);
  if (kinChain)
  {
    cv::Mat jacobian = kinChain->calculateEndEffectorJacobian();
    mOutput->setData(jacobian);
  }
}

void cedar::proc::steps::Jacobian::onStart()
{
  if (this->hasComponent())
  {
    auto component = this->getComponent();
    testStates(component);
  }
}

void cedar::proc::steps::Jacobian::onStop()
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

bool cedar::proc::steps::Jacobian::hasComponent() const
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

void cedar::proc::steps::Jacobian::reset()
{
  auto component = this->getComponent();
  component->clearAll();
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::Jacobian::determineInputValidity(cedar::proc::ConstDataSlotPtr /*slot*/, cedar::aux::ConstDataPtr /*data*/) const
{
  //cedar::aux::ConstMatDataPtr _input = boost::dynamic_pointer_cast < cedar::aux::ConstMatData > (data);
 
  if (1)
  {
    return cedar::proc::DataSlot::VALIDITY_VALID;
  }

  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::Jacobian::rebuildOutputs()
{
  this->removeAllSlots(cedar::proc::DataRole::OUTPUT);
  auto component = this->getComponent();
  cedar::dev::KinematicChainPtr kinChain = boost::dynamic_pointer_cast < cedar::dev::KinematicChain > (component);
  if (kinChain)
  {
    mOutput->setData(cv::Mat::zeros(kinChain->getNumberOfJoints(), 1, CV_32FC1));
    this->declareOutput("jacobian", mOutput);
  }
}

void cedar::proc::steps::Jacobian::testStates(cedar::dev::ComponentPtr component)
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
