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

 File:        

 Maintainer:  Jean-Stephane Jokeit
 Email:       
 Date:        

 Description: Source file for the class cedar::proc::steps::InverseKinematicsStep.

 Credits:

 ======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/DampedPseudoInverseKinematics.h"

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

    ElementDeclarationPtr declaration(new ElementDeclarationTemplate<cedar::proc::steps::DampedPseudoInverseKinematics>("Robotics", "cedar.processing.steps.DampedPseudoInverseKinematics"));
    declaration->setIconPath(":/steps/damped_inverse_kinematic.svg");
    declaration->setDescription("Connects to a robot and calculates the damped least squares inverse of the end-effector Jacobian. Then applies the input to it.  'Lambda' is the damping parameter. Set it to zero for no damping. Needs to be connected to an appropriate Component (Robot). TODO: output the Jacobian TODO: optional input as a point on the robot to calculate J for that position.");
    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::DampedPseudoInverseKinematics::DampedPseudoInverseKinematics()
    :
      cedar::proc::Step(true),
      mOutputVelocity(new cedar::aux::MatData(cv::Mat())),
      mInputVelocityName("Cartesian velocity"),
      _mComponent(new cedar::dev::ComponentParameter(this, "component")),
      mLambda(new cedar::aux::DoubleParameter(this, "damping lambda", 0.04, 0.0, 100000.0 ))
{
  this->declareInput(mInputVelocityName);
  QObject::connect(this->_mComponent.get(), SIGNAL(valueChanged()), this, SLOT(rebuildOutputs()));
}

cedar::proc::steps::DampedPseudoInverseKinematics::~DampedPseudoInverseKinematics()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::steps::DampedPseudoInverseKinematics::compute(const cedar::proc::Arguments&)
{
  cedar::aux::ConstDataPtr inputData = this->getInputSlot(mInputVelocityName)->getData();

  if (this->hasComponent() && boost::dynamic_pointer_cast<const cedar::aux::MatData>(inputData))
  {
    cv::Mat cartesianVelocityMat = inputData->getData<cv::Mat>().clone();
    auto component = this->getComponent();
    this->testStates(component);
    cedar::dev::KinematicChainPtr kinChain = boost::dynamic_pointer_cast < cedar::dev::KinematicChain > (component);
    if (kinChain)
    {
      cv::Mat Jacobian = kinChain->calculateEndEffectorJacobian();
      cv::Mat jacobian_pseudo_inverse = cv::Mat::zeros(kinChain->getNumberOfJoints(), 2, CV_32FC1);

      cv::Mat U, V_transposed, S;

      //cv::invert(Jacobian, jacobian_pseudo_inverse, cv::DECOMP_SVD);
      cv::SVD::compute( Jacobian, S, U, V_transposed, cv::SVD::FULL_UV );

      cv::Mat V, U_transposed;
      V= V_transposed.t();
      U_transposed= U.t();

      cv::Mat damped_inverse;
     
      float lambda_squared = mLambda->getValue() * mLambda->getValue();

      cv::Mat S_diag= cv::Mat::diag( S );
      cv::Mat S2;
      cv::divide(S_diag, S_diag*S_diag + lambda_squared, S2 );
      cv::Mat S3;
      if (V.cols > S2.rows) 
      {
            cv::vconcat(S2, cv::Mat::zeros( V.cols - S2.rows,
                                      S2.cols,
                                      CV_32F ),
                  S3);
      }
      else 
      {
        S3 = S2;
      }
      if (U_transposed.rows > S3.cols) 
      {
            cv::hconcat(S3.clone(), cv::Mat::zeros(S3.rows, U_transposed.rows - S3.cols, CV_32F), S3);
      }

      damped_inverse= V * S3 * U_transposed;
      
      cv::Mat joint_velocities = damped_inverse * cartesianVelocityMat;
      mOutputVelocity->setData(joint_velocities);
    }
  }
}

void cedar::proc::steps::DampedPseudoInverseKinematics::onStart()
{
  if (this->hasComponent())
  {
    auto component = this->getComponent();
    testStates(component);
  }
}

void cedar::proc::steps::DampedPseudoInverseKinematics::onStop()
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

bool cedar::proc::steps::DampedPseudoInverseKinematics::hasComponent() const
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

void cedar::proc::steps::DampedPseudoInverseKinematics::reset()
{
  auto component = this->getComponent();
  component->clearAll();
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::DampedPseudoInverseKinematics::determineInputValidity(cedar::proc::ConstDataSlotPtr slot, cedar::aux::ConstDataPtr data) const
{
  cedar::aux::ConstMatDataPtr _input = boost::dynamic_pointer_cast < cedar::aux::ConstMatData > (data);
  if (_input && _input->getDimensionality() == 1 && slot->getName() == mInputVelocityName && cedar::aux::math::get1DMatrixSize(_input->getData()) == 3)
  {
    return cedar::proc::DataSlot::VALIDITY_VALID;
  }

  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::DampedPseudoInverseKinematics::rebuildOutputs()
{
  this->removeAllSlots(cedar::proc::DataRole::OUTPUT);
  auto component = this->getComponent();
  cedar::dev::KinematicChainPtr kinChain = boost::dynamic_pointer_cast < cedar::dev::KinematicChain > (component);
  if (kinChain)
  {
    mOutputVelocity->setData(cv::Mat::zeros(kinChain->getNumberOfJoints(), 1, CV_32FC1));
    this->declareOutput("joint velocity", mOutputVelocity);
  }
}

void cedar::proc::steps::DampedPseudoInverseKinematics::testStates(cedar::dev::ComponentPtr component)
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
