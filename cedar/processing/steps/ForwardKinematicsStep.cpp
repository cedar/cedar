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

 File:        ForwardKinematicsStep.cpp

 Maintainer:  Jan Tekülve
 Email:       jan.tekuelve@ini.rub.de
 Date:        2016 06 28

 Description: Source file for the class cedar::proc::steps::ForwardKinematicsStep.

 Credits:

 ======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/ForwardKinematicsStep.h"

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

    ElementDeclarationPtr declaration(new ElementDeclarationTemplate<cedar::proc::steps::ForwardKinematicsStep>("Robotics", "cedar.processing.steps.ForwardKinematicsStep"));
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

cedar::proc::steps::ForwardKinematicsStep::ForwardKinematicsStep()
    :
      cedar::proc::Step(true),
      mOutputPos(new cedar::aux::MatData(cv::Mat())),
      mOutputVelocity(new cedar::aux::MatData(cv::Mat())),
      mOutputAcceleration(new cedar::aux::MatData(cv::Mat())),
      _mComponent(new cedar::dev::ComponentParameter(this, "component"))
{
  QObject::connect(this->_mComponent.get(), SIGNAL(valueChanged()), this, SLOT(rebuildOutputs()));
}

cedar::proc::steps::ForwardKinematicsStep::~ForwardKinematicsStep()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::steps::ForwardKinematicsStep::compute(const cedar::proc::Arguments&)
{
  if (this->hasComponent())
  {
    auto component = this->getComponent();
    cedar::dev::KinematicChainPtr kinChain = boost::dynamic_pointer_cast < cedar::dev::KinematicChain > (component);
    if (kinChain)
    {
      mOutputPos->setData(kinChain->calculateEndEffectorPosition());
      mOutputVelocity->setData(kinChain->calculateEndEffectorVelocity());
      mOutputAcceleration->setData(kinChain->calculateEndEffectorAcceleration());
    }
  }
}

bool cedar::proc::steps::ForwardKinematicsStep::hasComponent() const
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

void cedar::proc::steps::ForwardKinematicsStep::reset()
{
  auto component = this->getComponent();
  component->clearAll();
}

void cedar::proc::steps::ForwardKinematicsStep::rebuildOutputs()
{
  this->removeAllSlots(cedar::proc::DataRole::OUTPUT);
  auto component = this->getComponent();
  cedar::dev::KinematicChainPtr kinChain = boost::dynamic_pointer_cast < cedar::dev::KinematicChain > (component);
  if (kinChain)
  {
    mOutputPos->setData(cv::Mat::zeros(kinChain->getNumberOfJoints(), 1, CV_64FC1));
    mOutputVelocity->setData(cv::Mat::zeros(kinChain->getNumberOfJoints(), 1, CV_64FC1));
    mOutputAcceleration->setData(cv::Mat::zeros(kinChain->getNumberOfJoints(), 1, CV_64FC1));
    this->declareOutput("Cartesian position", mOutputPos);
    this->declareOutput("Cartesian velocity", mOutputVelocity);
    this->declareOutput("Cartesian acceleration", mOutputAcceleration);
  }
}

