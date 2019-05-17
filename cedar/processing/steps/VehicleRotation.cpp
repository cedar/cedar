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

 File:        VehicleRotation.cpp

 Maintainer:  Jan Tekülve
 Email:       jan.tekuelve@ini.rub.de
 Date:        2016 07 07

 Description: Source file for the class cedar::proc::steps::VehicleRotation.

 Credits:

 ======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_OS_WINDOWS
#define _USE_MATH_DEFINES
#include <cmath>
#endif // CEDAR_OS_WINDOWS

// CLASS HEADER
#include "cedar/processing/steps/VehicleRotation.h"

// CEDAR INCLUDES
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/devices/Vehicle.h"
#include "cedar/processing/StepTime.h"
// SYSTEM INCLUDES
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration(new ElementDeclarationTemplate<cedar::proc::steps::VehicleRotation>("Robotics", "cedar.processing.steps.VehicleRotation"));
    declaration->setIconPath(":/steps/vehicle_rotation.svg");
    declaration->deprecatedName("cedar.processing.steps.VehicleRotationStep");
    declaration->declare();
    declaration->setDescription
    (
      "Control an appropritate vehicle by rotation commands. "
      "Needs to connect to a Component (Robot)."
    );

    return true;
  }

  bool declared = declare();
}
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EnumType<cedar::proc::steps::VehicleRotation::AngularMeasurementUnit> cedar::proc::steps::VehicleRotation::AngularMeasurementUnit::mType("FunctionType::");

cedar::proc::steps::VehicleRotation::VehicleRotation()
    :
    cedar::proc::Step(true),
      mOutputVelocity(new cedar::aux::MatData(cv::Mat())),
      mInputVelocityName("rotation velocity"),
      _mComponent(new cedar::dev::ComponentParameter(this, "component")),
      mUnitType(
                new cedar::aux::EnumParameter(this, "input unit", cedar::proc::steps::VehicleRotation::AngularMeasurementUnit::typePtr(),
                    AngularMeasurementUnit::Rad))
{
  this->declareInput(mInputVelocityName);
  QObject::connect(this->_mComponent.get(), SIGNAL(valueChanged()), this, SLOT(rebuildOutputs()));
  this->mTimestepMeasurementId = this->registerTimeMeasurement("time step");
}

cedar::proc::steps::VehicleRotation::~VehicleRotation()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::steps::VehicleRotation::compute(const cedar::proc::Arguments& arguments)
{
  try
  {
    const cedar::proc::StepTime& step_time = dynamic_cast<const cedar::proc::StepTime&>(arguments);

    this->setTimeMeasurement(this->mTimestepMeasurementId, step_time.getStepTime());

    this->eulerStep(step_time.getStepTime());
  }
  catch (const std::bad_cast& e)
  {
    CEDAR_THROW(cedar::proc::InvalidArgumentsException, "Bad arguments passed to dynamics. Expected StepTime.");
  }
}

void cedar::proc::steps::VehicleRotation::eulerStep(const cedar::unit::Time&)
{
  cedar::aux::ConstDataPtr inputData = this->getInputSlot(mInputVelocityName)->getData();

  if (this->hasComponent() && boost::dynamic_pointer_cast<const cedar::aux::MatData>(inputData))
  {
    cv::Mat rotationVelocityMat = inputData->getData<cv::Mat>().clone();

    //Ensure that we work with Rad Values
    if(mUnitType->getValue() == AngularMeasurementUnit::Degree)
    {
      rotationVelocityMat = rotationVelocityMat * (M_PI /(180.0));
    }

    auto component = this->getComponent();
    this->testStates(component);
    if (auto vehicle = boost::dynamic_pointer_cast < cedar::dev::Vehicle > (component))
    {
      cv::Mat wheelRotations = vehicle->getWheelRotations(rotationVelocityMat.at<float>(0,0));
//      double timeFactor = time / cedar::unit::Time(1 * cedar::unit::milli * cedar::unit::seconds);
//      wheelRotations = wheelRotations * timeFactor;
      mOutputVelocity->setData(wheelRotations);
    }
  }
}

void cedar::proc::steps::VehicleRotation::onStart()
{
  if (this->hasComponent())
  {
    auto component = this->getComponent();
    testStates(component);
  }
}

void cedar::proc::steps::VehicleRotation::onStop()
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

bool cedar::proc::steps::VehicleRotation::hasComponent() const
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

void cedar::proc::steps::VehicleRotation::reset()
{
  auto component = this->getComponent();
  component->clearAll();
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::VehicleRotation::determineInputValidity(cedar::proc::ConstDataSlotPtr slot, cedar::aux::ConstDataPtr data) const
{
  cedar::aux::ConstMatDataPtr _input = boost::dynamic_pointer_cast < cedar::aux::ConstMatData > (data);
  if (_input && _input->getDimensionality() == 0 && slot->getName() == mInputVelocityName && _input->getData().type() == CV_32F)
  {
    return cedar::proc::DataSlot::VALIDITY_VALID;
  }

  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::VehicleRotation::rebuildOutputs()
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

void cedar::proc::steps::VehicleRotation::testStates(cedar::dev::ComponentPtr component)
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

