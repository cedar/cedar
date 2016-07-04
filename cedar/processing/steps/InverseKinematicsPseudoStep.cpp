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

 File:        InverseKinematicsStep.cpp

 Maintainer:  Jan Tekülve
 Email:       jan.tekuelve@ini.rub.de
 Date:        2016 06 29

 Description: Source file for the class cedar::proc::steps::InverseKinematicsStep.

 Credits:

 ======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/InverseKinematicsPseudoStep.h"

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

    ElementDeclarationPtr declaration(new ElementDeclarationTemplate<cedar::proc::steps::InverseKinematicsPseudoStep>("Robotics", "cedar.processing.steps.InverseKinematicsPseudoStep"));
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

cedar::proc::steps::InverseKinematicsPseudoStep::InverseKinematicsPseudoStep()
    :
      cedar::proc::Step(true),
      mOutputVelocity(new cedar::aux::MatData(cv::Mat())),
      mInputVelocityName("Cartesian velocity"),
      _mComponent(new cedar::dev::ComponentParameter(this, "component"))
{
  this->declareInput(mInputVelocityName);
  QObject::connect(this->_mComponent.get(), SIGNAL(valueChanged()), this, SLOT(rebuildOutputs()));
}

cedar::proc::steps::InverseKinematicsPseudoStep::~InverseKinematicsPseudoStep()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::steps::InverseKinematicsPseudoStep::compute(const cedar::proc::Arguments&)
{
  cedar::aux::ConstDataPtr inputData = this->getInputSlot(mInputVelocityName)->getData();

  if (this->hasComponent() && boost::dynamic_pointer_cast<const cedar::aux::MatData>(inputData))
  {
    cv::Mat cartesianVelocityMat = inputData->getData<cv::Mat>().clone();
    auto component = this->getComponent();
    cedar::dev::KinematicChainPtr kinChain = boost::dynamic_pointer_cast < cedar::dev::KinematicChain > (component);
    if (kinChain)
    {
      cv::Mat Jacobian = kinChain->calculateEndEffectorJacobian();
      cv::Mat jacobian_pseudo_inverse = cv::Mat::zeros(kinChain->getNumberOfJoints(), 2, CV_64FC1);
      cv::invert(Jacobian, jacobian_pseudo_inverse, cv::DECOMP_SVD);

      cv::Mat joint_velocities = jacobian_pseudo_inverse * cartesianVelocityMat;

      mOutputVelocity->setData(joint_velocities);
    }
  }
}

bool cedar::proc::steps::InverseKinematicsPseudoStep::hasComponent() const
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

void cedar::proc::steps::InverseKinematicsPseudoStep::reset()
{
  auto component = this->getComponent();
  component->clearAll();
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::InverseKinematicsPseudoStep::determineInputValidity(cedar::proc::ConstDataSlotPtr slot, cedar::aux::ConstDataPtr data) const
{
  cedar::aux::ConstMatDataPtr _input = boost::dynamic_pointer_cast < cedar::aux::ConstMatData > (data);
  if (_input && _input->getDimensionality() == 1 && slot->getName() == mInputVelocityName && cedar::aux::math::get1DMatrixSize(_input->getData()) == 3)
  {
    return cedar::proc::DataSlot::VALIDITY_VALID;
  }

  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::InverseKinematicsPseudoStep::rebuildOutputs()
{
  this->removeAllSlots(cedar::proc::DataRole::OUTPUT);
  auto component = this->getComponent();
  cedar::dev::KinematicChainPtr kinChain = boost::dynamic_pointer_cast < cedar::dev::KinematicChain > (component);
  if (kinChain)
  {
    mOutputVelocity->setData(cv::Mat::zeros(kinChain->getNumberOfJoints(), 1, CV_64FC1));
    this->declareOutput("joint velocity", mOutputVelocity);
  }
}
