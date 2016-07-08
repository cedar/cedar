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

 File:        VehicleRotationStep.cpp

 Maintainer:  Jan Tekülve
 Email:       jan.tekuelve@ini.rub.de
 Date:        2016 07 07

 Description: Source file for the class cedar::proc::steps::VehicleRotationStep.

 Credits:

 ======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/VehicleRotationStep.h"

// CEDAR INCLUDES
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/devices/Vehicle.h"
// SYSTEM INCLUDES
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration(new ElementDeclarationTemplate<cedar::proc::steps::VehicleRotationStep>("Robotics", "cedar.processing.steps.VehicleRotationStep"));
//    declaration->setIconPath(":/cedar/dev/gui/icons/generic_hardware_icon.svg");
    //todo:Icon generieren
    declaration->declare();

    return true;
  }

  bool declared = declare();
}
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::VehicleRotationStep::VehicleRotationStep()
    :
      cedar::proc::Step(true),
      mOutputVelocity(new cedar::aux::MatData(cv::Mat())),
      mInputVelocityName("rotation velocity"),
      _mComponent(new cedar::dev::ComponentParameter(this, "component"))
{
  this->declareInput(mInputVelocityName);
  QObject::connect(this->_mComponent.get(), SIGNAL(valueChanged()), this, SLOT(rebuildOutputs()));
}

cedar::proc::steps::VehicleRotationStep::~VehicleRotationStep()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::steps::VehicleRotationStep::compute(const cedar::proc::Arguments&)
{
  cedar::aux::ConstDataPtr inputData = this->getInputSlot(mInputVelocityName)->getData();

  if (this->hasComponent() && boost::dynamic_pointer_cast<const cedar::aux::MatData>(inputData))
  {
    cv::Mat rotationVelocityMat = inputData->getData<cv::Mat>().clone();
    auto component = this->getComponent();
    this->testStates(component);
    cedar::dev::VehiclePtr vehicle = boost::dynamic_pointer_cast < cedar::dev::Vehicle > (component);
    if (vehicle)
    {
      cv::Mat wheelRotations = vehicle->getWheelRotations(rotationVelocityMat.at<float>(0,0));
//      cv::Mat wheel_velocities = wheelRotationDirections * rotationVelocityMat;
      mOutputVelocity->setData(wheelRotations);
    }
  }
}

void cedar::proc::steps::VehicleRotationStep::onStart()
{
  if (this->hasComponent())
  {
    auto component = this->getComponent();
    testStates(component);
  }
}

void cedar::proc::steps::VehicleRotationStep::onStop()
{
  if (this->hasComponent())
  {
    auto component = this->getComponent();
    if (component->isCommunicating())
    {
      cedar::aux::LogSingleton::getInstance()->warning(component->prettifyName() + " is still connected and running.", CEDAR_CURRENT_FUNCTION_NAME);
    }
    else
    {
      this->resetState();
    }
  }
}

bool cedar::proc::steps::VehicleRotationStep::hasComponent() const
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

void cedar::proc::steps::VehicleRotationStep::reset()
{
  auto component = this->getComponent();
  component->clearAll();
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::VehicleRotationStep::determineInputValidity(cedar::proc::ConstDataSlotPtr slot, cedar::aux::ConstDataPtr data) const
{
  cedar::aux::ConstMatDataPtr _input = boost::dynamic_pointer_cast < cedar::aux::ConstMatData > (data);
  if (_input && _input->getDimensionality() == 0 && slot->getName() == mInputVelocityName && _input->getData().type() == CV_32F)
  {
    return cedar::proc::DataSlot::VALIDITY_VALID;
  }

  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::VehicleRotationStep::rebuildOutputs()
{
  this->removeAllSlots(cedar::proc::DataRole::OUTPUT);
  auto component = this->getComponent();
  cedar::dev::VehiclePtr vehicle = boost::dynamic_pointer_cast < cedar::dev::Vehicle > (component);
  if (vehicle)
  {
    mOutputVelocity->setData(cv::Mat::zeros(vehicle->getNumberOfWheels(), 1, CV_64FC1));
    this->declareOutput("wheel velocity", mOutputVelocity);
  }
}

void cedar::proc::steps::VehicleRotationStep::testStates(cedar::dev::ComponentPtr component)
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

