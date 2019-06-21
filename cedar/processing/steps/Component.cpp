/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Component.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 03 05

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/stepViews/ComponentStepView.h"
#include "cedar/processing/steps/Component.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/gui/IdeApplication.h"
#include "cedar/processing/gui/Ide.h"
#include "cedar/devices/Component.h"
#include "cedar/devices/exceptions.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/devices/Sensor.h"
#include "cedar/devices/SimulatedKinematicChain.h"



// SYSTEM INCLUDES
#include <QHBoxLayout>
#include <typeinfo>
#include <vector>


//----------------------------------------------------------------------------------------------------------------------
// declaration
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration
    (
      new ElementDeclarationTemplate<cedar::proc::steps::Component, cedar::proc::gui::ComponentStepView>
      (
        "Robotics",
        "cedar.processing.steps.Component"
      )
    );
    declaration->setDescription("Start here! The interface to your robot.");;
    declaration->setIconPath(":/cedar/dev/gui/icons/generic_hardware_icon.svg");
    declaration->declare();

    return true;
  }

  bool declared = declare();
}


//----------------------------------------------------------------------------------------------------------------------
// GUI for the group parameter
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool registered = cedar::aux::gui::ParameterFactorySingleton::getInstance()->add
      <
        cedar::proc::details::ComponentStepUserSelectableCommandTypeSubsetParameter,
        cedar::proc::details::ComponentStepUserSelectableCommandTypeSubsetParameterWidget
      >();
}

cedar::proc::details::ComponentStepUserSelectableCommandTypeSubsetParameterWidget::ComponentStepUserSelectableCommandTypeSubsetParameterWidget()
{
  auto layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(layout);

  this->mpSelector = new QComboBox();
  layout->addWidget(this->mpSelector);

  QObject::connect(this->mpSelector, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(selectedUserSelectableCommandTypeSubsetChanged(const QString&)));
}

void cedar::proc::details::ComponentStepUserSelectableCommandTypeSubsetParameterWidget::parameterChanged()
{
  auto parameter = boost::dynamic_pointer_cast<cedar::proc::details::ComponentStepUserSelectableCommandTypeSubsetParameter>(this->getParameter());
  CEDAR_ASSERT(parameter);

  this->rebuildUserSelectableCommandTypeSubsetList();

  this->applyProperties();

  QObject::connect(parameter.get(), SIGNAL(componentChanged()), this, SLOT(componentChanged()));
}

void cedar::proc::details::ComponentStepUserSelectableCommandTypeSubsetParameterWidget::rebuildUserSelectableCommandTypeSubsetList()
{
  auto parameter = boost::dynamic_pointer_cast<cedar::proc::details::ComponentStepUserSelectableCommandTypeSubsetParameter>(this->getParameter());
  CEDAR_ASSERT(parameter);

  std::string current = parameter->getValue();

  bool blocked = this->mpSelector->blockSignals(true);
  this->mpSelector->clear();

  this->mpSelector->addItem("all");
  int select = 0;


  if (parameter->hasComponent())
  {
    auto component = parameter->getComponent();
    auto groups = component->listUserSelectableCommandTypeSubsets();

    for (const auto& group : groups)
    {
      if (group == current)
      {
        select = this->mpSelector->count();
      }
      this->mpSelector->addItem(QString::fromStdString(group));
    }
  }
  this->mpSelector->setCurrentIndex(select);
  this->mpSelector->blockSignals(blocked);
}

void cedar::proc::details::ComponentStepUserSelectableCommandTypeSubsetParameterWidget::propertiesChanged()
{
  this->applyProperties();
}

void cedar::proc::details::ComponentStepUserSelectableCommandTypeSubsetParameterWidget::applyProperties()
{
  auto parameter = boost::dynamic_pointer_cast<cedar::proc::details::ComponentStepUserSelectableCommandTypeSubsetParameter>(this->getParameter());
  CEDAR_ASSERT(parameter);

  this->mpSelector->setDisabled(parameter->isConstant());
}

void cedar::proc::details::ComponentStepUserSelectableCommandTypeSubsetParameterWidget::componentChanged()
{
  this->rebuildUserSelectableCommandTypeSubsetList();
}

void cedar::proc::details::ComponentStepUserSelectableCommandTypeSubsetParameterWidget::selectedUserSelectableCommandTypeSubsetChanged(const QString& group)
{
  auto parameter = boost::dynamic_pointer_cast<cedar::proc::details::ComponentStepUserSelectableCommandTypeSubsetParameter>(this->getParameter());
  CEDAR_ASSERT(parameter);

  if (group == "all")
  {
    parameter->setValue(std::string());
  }
  else
  {
    parameter->setValue(group.toStdString());
  }
}

//----------------------------------------------------------------------------------------------------------------------
// group parameter
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::details::ComponentStepUserSelectableCommandTypeSubsetParameter::ComponentStepUserSelectableCommandTypeSubsetParameter(cedar::aux::Configurable* owner, const std::string& name)
:
cedar::aux::ParameterTemplate<std::string>(owner, name, "")
{
}

void cedar::proc::details::ComponentStepUserSelectableCommandTypeSubsetParameter::setComponent(cedar::dev::ComponentPtr component)
{
  this->mWeakComponent = component;
  emit componentChanged();
}

bool cedar::proc::details::ComponentStepUserSelectableCommandTypeSubsetParameter::hasComponent()
{
  return mWeakComponent.use_count() != 0;
}

cedar::dev::ComponentPtr cedar::proc::details::ComponentStepUserSelectableCommandTypeSubsetParameter::getComponent()
{
  if(auto component = this->mWeakComponent.lock())
  {
    return component;
  }
  else
  {
   return NULL;
  }
}


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::Component::Component()
:
cedar::proc::Step(true),
mInitConfigHasBeenApplied(false),
_mComponent(new cedar::dev::ComponentParameter(this, "component")),
_mUserSelectableCommandTypeSubset(new cedar::proc::details::ComponentStepUserSelectableCommandTypeSubsetParameter(this,
"Command Subset")),
_mCommunicationStepSize(new cedar::aux::DoubleParameter(this, "communication step size [ms]", 10.0, 0.01, 100)),
_mUseKinChainConfigurationOnReset( new cedar::aux::BoolParameter(this, "init/reset to current config", false))
{
  this->_mUserSelectableCommandTypeSubset->setConstant(true);

  QObject::connect(this->_mComponent.get(), SIGNAL(valueChanged()), this, SLOT(componentChangedSlot()));
  QObject::connect(this->_mComponent.get(), SIGNAL(valueChanged()), this, SIGNAL(componentChanged()));
  QObject::connect(this->_mUserSelectableCommandTypeSubset.get(), SIGNAL(valueChanged()), this, SLOT(selectedUserSelectableCommandTypeSubsetChanged()));
  QObject::connect(this->_mCommunicationStepSize.get(), SIGNAL(valueChanged()), this, SLOT(communicationStepSizeChanged()));

  this->mMeasurementTimeId = this->registerTimeMeasurement("step measurements time");
  this->mCommandTimeId = this->registerTimeMeasurement("step commands time");

  this->registerFunction( "brake slowly", boost::bind(&cedar::proc::steps::Component::brakeSlowly, this ) );
  this->registerFunction( "brake hard", boost::bind(&cedar::proc::steps::Component::brakeHard, this ) );
  this->registerFunction( "disconnect", boost::bind(&cedar::proc::steps::Component::disconnectManually, this ) );
  this->registerFunction( "connect", boost::bind(&cedar::proc::steps::Component::connectManually, this ) );
  this->registerFunction( "open Robot Manager", boost::bind(&cedar::proc::steps::Component::openRobotManager, this ) );


  if(auto kinChain = boost::dynamic_pointer_cast<cedar::dev::SimulatedKinematicChain>(this->getComponent()))
  {
    _mUseKinChainConfigurationOnReset->setConstant(false);
  } else
  {
    _mUseKinChainConfigurationOnReset->setValue(false);
    _mUseKinChainConfigurationOnReset->setConstant(true);
  }

}

cedar::proc::steps::Component::~Component()
{
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::proc::steps::Component::hasComponent() const
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


void cedar::proc::steps::Component::selectedUserSelectableCommandTypeSubsetChanged()
{
  if (this->hasComponent())
  {
    auto component = this->getComponent();

    std::string selected_group = this->_mUserSelectableCommandTypeSubset->getValue();
    component->setActiveUserSelectableCommandTypeSubset( selected_group );
  }

  this->rebuildInputs();
}

void cedar::proc::steps::Component::communicationStepSizeChanged()
{
  const double &paramvalue = _mCommunicationStepSize->getValue();
  const cedar::unit::Time step_size = cedar::unit::Time(paramvalue * cedar::unit::milli * cedar::unit::seconds);
  this->getComponent()->setCommunicationStepSize(step_size);
}

void cedar::proc::steps::Component::testStates(cedar::dev::ComponentPtr component)
{
  if (!component->isCommunicating())
  {
    this->setState(cedar::proc::Triggerable::STATE_INITIALIZING,
        component->prettifyName() + " is not connected, yet. Open the Robot Manager to connect.");
  }
  else if (!component->isReadyForMeasurements())
  {
    this->setState(cedar::proc::Triggerable::STATE_INITIALIZING,
      component->prettifyName() + " is not ready to receive measurements, yet.");
  }
  else if (!component->isReadyForCommands())
  {
    this->setState(cedar::proc::Triggerable::STATE_INITIALIZING,
      component->prettifyName() + " is not ready to send commands, yet.");
  }
  else
  {
    this->resetState();
  }
}

void cedar::proc::steps::Component::onStart()
{
  this->_mComponent->setConstant(true);
  if (this->hasComponent())
  {
    auto component = this->getComponent();

    component->clearUserSideCommand();

    // should be supressed, un-lock:
    if (component->getSuppressUserSideInteraction())
    {
      component->setSuppressUserSideInteraction(false);
    }

    testStates(component);

    if (!component->isCommunicating())
    {
      cedar::aux::LogSingleton::getInstance()->message(
        component->prettifyName() + " is not connected, yet. Open the Robot Manager to connect.",
        CEDAR_CURRENT_FUNCTION_NAME);
    }
  }
}

void cedar::proc::steps::Component::onStop()
{
  this->_mComponent->setConstant(false);
  if (this->hasComponent())
  {
    auto component = this->getComponent();
    component->clearUserSideCommand();

    if (component->isCommunicating())
    {
#if 0      
      cedar::aux::LogSingleton::getInstance()->warning(
        component->prettifyName() + " is still connected and running.",
        CEDAR_CURRENT_FUNCTION_NAME);
#endif      
    }
    else
    {
      this->resetState();
    }
  }
}

void cedar::proc::steps::Component::compute(const cedar::proc::Arguments&)
{
  if (!this->hasComponent())
    return;

  auto component = this->getComponent();

  if(!mInitConfigHasBeenApplied &&  _mUseKinChainConfigurationOnReset->getValue() && boost::dynamic_pointer_cast<cedar::dev::SimulatedKinematicChain>(component))
  {
    //This ensures that the Initial Configuration is applied after the first start of the architecture
    //Onstart does not work, because the communication might not yet be established at that point
    //Connection to the onconnect signal somehow produced a deadlock
    this->applyCurrentInitialConfiguration();
  }

  this->testStates(component);

  // update time measurements
  if (component->hasLastStepMeasurementsDuration())
  {
    auto time = component->retrieveLastStepMeasurementsDuration();
    this->setTimeMeasurement(this->mMeasurementTimeId, time);
  }

  if (component->hasLastStepCommandsDuration())
  {
    auto time = component->retrieveLastStepCommandsDuration();
    this->setTimeMeasurement(this->mCommandTimeId, time);
  }

//  std::cout<<"1"<<std::endl;
//  // unlock the suppression of user commands when the architecture is running ... only once please
  if(component->getSuppressUserSideInteraction())
  {
    component->setSuppressUserSideInteraction(false);
  }
//  std::cout<<"2"<<std::endl;
//  // retrieve Data from the component and copy it to the output slots of the component
  auto measurements = component->getInstalledMeasurementTypes();
//  std::cout<<"3"<<std::endl;
  for (const auto& measurement : measurements)
  {
    std::string name = component->getNameForMeasurementType(measurement);
    if(auto measurementDataOriginal = component->getMeasurementData(measurement))
    {
      auto measurementData = measurementDataOriginal; // There was a clone here that seemed unneccesary and only caused a crash on windows.
      if (boost::dynamic_pointer_cast<const cedar::aux::MatData>(measurementData))
      {
        cv::Mat measurementMat = measurementData->getData<cv::Mat>().clone();
        std::string name = component->getNameForMeasurementType(measurement);
        if(auto outPutPtr = mOutputs.at(name))
        {
          outPutPtr->setData(measurementMat);
        }
      }
    }
  }
//
//
//  // if no inputs are present, there is nothing to do (i.e., no inputs have to be passed to the component)
  if (this->hasSlotForRole(cedar::proc::DataRole::INPUT))
  {
    // copy data from the input slots to the command slots of the component
    for (auto name_slot_pair : this->getDataSlots(cedar::proc::DataRole::INPUT))
    {
      const auto& name = name_slot_pair.first;
      auto slot = name_slot_pair.second;

      auto command_type = component->getCommandTypeForName(name);

      auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(slot->getData());
      if (mat_data)
      {
        component->setUserSideCommandBuffer(command_type, mat_data->getData());
      }
    }
  }
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::Component::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr slot,
                                  cedar::aux::ConstDataPtr data
                                ) const
{
  const std::string& name = slot->getName();

  if (!this->hasComponent())
  {
    return cedar::proc::DataSlot::VALIDITY_WARNING;
  }

  // only commanded data are inputs
// ? lost in some merge
//  cedar::aux::ConstData *component_data = this->getComponent()->getCommandedData(name).get();
//  cedar::aux::ConstData *that_data = data.get();
//  if (typeid(*component_data) == typeid(*that_data))

  if (auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
  {
    if (mat_data->getData().type() != CV_32F)
    {
      return cedar::proc::DataSlot::VALIDITY_ERROR;
    }

    auto type = this->getComponent()->getCommandTypeForName(name);
    // check that the matrix has the same dimensionality as the command it is connecting to
    auto fulldim = this->getComponent()->getCommandDimensionality(type);
    auto size= fulldim[0];

    if (size == 1 && mat_data->getDimensionality() == 0)
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }

    if ((size > 1 && mat_data->getDimensionality() != 1))
    {
      return cedar::proc::DataSlot::VALIDITY_ERROR;
    }

    if (size == cedar::aux::math::get1DMatrixSize(mat_data->getData()))
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }

  // if nothing valid was found, return an error
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::Component::rebuildInputs()
{
  this->removeAllSlots(cedar::proc::DataRole::INPUT);

  if (!this->hasComponent())
  {
    return;
  }

  auto component = this->getComponent();
  std::vector<cedar::dev::Component::ComponentDataType> commands;

  std::string selected_group = this->_mUserSelectableCommandTypeSubset->getValue();
  if (selected_group.empty())
  {
    auto installed_set = component->getInstalledCommandTypes();
    commands.assign(installed_set.begin(), installed_set.end());
  }
  else
  {
    commands = component->getCommandsInUserSelectableCommandTypeSubset(selected_group);
  }

  for (const auto& command : commands)
  {
    std::string name = component->getNameForCommandType(command);
    //!@todo On inputConnectionChanged, incoming data must be set at the component.
    this->declareInput(name, false);
  }
}


void cedar::proc::steps::Component::rebuildOutputs()
{
  this->removeAllSlots(cedar::proc::DataRole::OUTPUT);
  mOutputs.clear();
  auto component = this->getComponent();
  auto measurements = component->getInstalledMeasurementTypes();

  for (const auto& measurement : measurements)
  {
    std::string name = component->getNameForMeasurementType(measurement);
    auto measurementData = component->getMeasurementData(measurement);
    if (boost::dynamic_pointer_cast<const cedar::aux::MatData>(measurementData))
    {
      cv::Mat measurementMat = measurementData->getData<cv::Mat>().clone();
      cedar::aux::MatDataPtr dataPtr = cedar::aux::MatDataPtr(new cedar::aux::MatData(measurementMat));
      this->declareOutput(name, dataPtr);
      mOutputs.insert(std::pair<std::string,cedar::aux::MatDataPtr>(name,dataPtr));
    }
  }
}

void cedar::proc::steps::Component::componentChangedSlot()
{
  //!@todo Clearing all slots means that all connections are lost. This is bad! Existing slots should remain.
  if (!this->hasComponent())
  {
    return;
  }

  this->rebuildOutputs();
  this->rebuildInputs();

  auto component = this->getComponent();
  this->_mUserSelectableCommandTypeSubset->setConstant(!component->hasUserSelectableCommandTypeSubsets());
  this->_mUserSelectableCommandTypeSubset->setComponent(component);

  if(auto kinChain = boost::dynamic_pointer_cast<cedar::dev::SimulatedKinematicChain>(this->getComponent()))
  {
    _mUseKinChainConfigurationOnReset->setConstant(false);
  } else
  {
    _mUseKinChainConfigurationOnReset->setValue(false);
    _mUseKinChainConfigurationOnReset->setConstant(true);
  }


}

void cedar::proc::steps::Component::reset()
{
  auto component = this->getComponent();
  component->clearAll();

  this->applyCurrentInitialConfiguration();
}


void cedar::proc::steps::Component::applyCurrentInitialConfiguration()
{
  auto component = this->getComponent();
  auto kinChain = boost::dynamic_pointer_cast<cedar::dev::SimulatedKinematicChain>(component);
  if( kinChain && _mUseKinChainConfigurationOnReset->getValue())
  {
    std::vector<std::string> configList = kinChain->getInitialConfigurationNames();

    if(configList.size()>0)
    {
//      std::cout << "Reset the position of the simulated arm. The current selected initial configuration is: " << kinChain->getCurrentInitialConfigurationName() << std::endl;
      if (!kinChain->hasInitialConfiguration(kinChain->getCurrentInitialConfigurationName()))
      {
        std::cout << "Reset " << this->getName() <<" : Choose the first initial configuration, because no current configuration was selected!" << std::endl;
        kinChain->setCurrentInitialConfiguration(configList[0]);
        mInitConfigHasBeenApplied = true;
      }
      if (kinChain->isCommunicating())
      {
//        std::cout<<"Reset to: " << kinChain->getCurrentInitialConfigurationName() << std::endl;
        kinChain->applyInitialConfiguration(kinChain->getCurrentInitialConfigurationName());
        mInitConfigHasBeenApplied = true;
      } else
      {
//        std::cout<<"Reset: Tried to reset "<< this->getName() <<" to " << kinChain->getCurrentInitialConfigurationName() <<" but it was not connected"<<std::endl;
      }
    } else
    {
      std::cout<<"Reset: Tried to reset "<< this->getName() <<" but no initial configurations were registered"<<std::endl;
    }
  }
}

void cedar::proc::steps::Component::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  cedar::proc::Step::readConfiguration(node);

  this->applyCurrentInitialConfiguration();
}

void cedar::proc::steps::Component::inputConnectionChanged(const std::string& /*inputName*/)
{
  auto component = this->getComponent();

  if (!component)
    return;

  component->clearUserSideCommand();
}

void cedar::proc::steps::Component::brakeSlowly()
{
  auto component = this->getComponent();

  if (!component)
    return;

  component->startBrakingSlowly();
}

void cedar::proc::steps::Component::brakeHard()
{
  auto component = this->getComponent();

  if (!component)
    return;

  component->startBrakingNow();
}

void cedar::proc::steps::Component::connectManually()
{
  auto component = this->getComponent();

  if (!component)
    return;

  component->startCommunication();
}

void cedar::proc::steps::Component::disconnectManually()
{
  auto component = this->getComponent();

  if (!component)
    return;

  component->stopCommunication();
}

void cedar::proc::steps::Component::openRobotManager()
{
  auto app = QCoreApplication::instance();
  if (app)
  {
    auto pointer = dynamic_cast< cedar::proc::gui::IdeApplication* >(app);
    if (pointer)
    {
      auto ide = pointer->getIde();
      ide->showRobotManager();
    }
  }
}
