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
#include "cedar/devices/Component.h"
#include "cedar/devices/exceptions.h"
#include "cedar/auxiliaries/Data.h"

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
        cedar::proc::details::ComponentStepGroupParameter,
        cedar::proc::details::ComponentStepGroupParameterWidget
      >();
}

cedar::proc::details::ComponentStepGroupParameterWidget::ComponentStepGroupParameterWidget()
{
  auto layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(layout);

  this->mpSelector = new QComboBox();
  layout->addWidget(this->mpSelector);

  QObject::connect(this->mpSelector, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(selectedGroupChanged(const QString&)));
}

void cedar::proc::details::ComponentStepGroupParameterWidget::parameterChanged()
{
  auto parameter = boost::dynamic_pointer_cast<cedar::proc::details::ComponentStepGroupParameter>(this->getParameter());
  CEDAR_ASSERT(parameter);

  this->rebuildGroupList();

  this->applyProperties();

  QObject::connect(parameter.get(), SIGNAL(componentChanged()), this, SLOT(componentChanged()));
}

void cedar::proc::details::ComponentStepGroupParameterWidget::rebuildGroupList()
{
  auto parameter = boost::dynamic_pointer_cast<cedar::proc::details::ComponentStepGroupParameter>(this->getParameter());
  CEDAR_ASSERT(parameter);

  auto component = parameter->getComponent();
  std::string current = parameter->getValue();

  bool blocked = this->mpSelector->blockSignals(true);
  this->mpSelector->clear();


  this->mpSelector->addItem("all");
  int select = 0;

  if (component)
  {
    auto groups = component->listCommandGroups();

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

void cedar::proc::details::ComponentStepGroupParameterWidget::propertiesChanged()
{
  this->applyProperties();
}

void cedar::proc::details::ComponentStepGroupParameterWidget::applyProperties()
{
  auto parameter = boost::dynamic_pointer_cast<cedar::proc::details::ComponentStepGroupParameter>(this->getParameter());
  CEDAR_ASSERT(parameter);

  this->mpSelector->setDisabled(parameter->isConstant());
}

void cedar::proc::details::ComponentStepGroupParameterWidget::componentChanged()
{
  this->rebuildGroupList();
}

void cedar::proc::details::ComponentStepGroupParameterWidget::selectedGroupChanged(const QString& group)
{
  auto parameter = boost::dynamic_pointer_cast<cedar::proc::details::ComponentStepGroupParameter>(this->getParameter());
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

cedar::proc::details::ComponentStepGroupParameter::ComponentStepGroupParameter(cedar::aux::Configurable* owner, const std::string& name)
:
cedar::aux::ParameterTemplate<std::string>(owner, name, "")
{
}

void cedar::proc::details::ComponentStepGroupParameter::setComponent(cedar::dev::ComponentPtr component)
{
  this->mComponent = component;

  emit componentChanged();
}

cedar::dev::ComponentPtr cedar::proc::details::ComponentStepGroupParameter::getComponent()
{
  return this->mComponent;
}


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::Component::Component()
:
cedar::proc::Step(true),
_mComponent(new cedar::dev::ComponentParameter(this, "component")),
_mGroup(new cedar::proc::details::ComponentStepGroupParameter(this, "command group"))
{
  this->_mGroup->setConstant(true);
  QObject::connect(this->_mComponent.get(), SIGNAL(valueChanged()), this, SLOT(componentChangedSlot()));
  QObject::connect(this->_mComponent.get(), SIGNAL(valueChanged()), this, SIGNAL(componentChanged()));
  QObject::connect(this->_mGroup.get(), SIGNAL(valueChanged()), this, SLOT(selectedGroupChanged()));

  this->mMeasurementTimeId = this->registerTimeMeasurement("step measurements time");
  this->mCommandTimeId = this->registerTimeMeasurement("step commands time");
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


void cedar::proc::steps::Component::selectedGroupChanged()
{
  this->rebuildInputs();
}

void cedar::proc::steps::Component::testStates(cedar::dev::ComponentPtr component)
{
  if (!component->isRunning())
  {
    this->setState(cedar::proc::Triggerable::STATE_INITIALIZING, 
        component->prettifyName() + " is not connected, yet. Open the Robot Manager to connect.");
  }
  else if (!component->isReadyForMeasurements())
  {
    this->setState(cedar::proc::Triggerable::STATE_INITIALIZING, 
      component->prettifyName() + " is not ready to receive measurments, yet.");
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

    component->clearUserCommand();

    // should be supressed, un-lock:
    if (component->getSuppressUserInteraction())
    {
      component->setSuppressUserInteraction(false);
    }

    testStates(component);

    if (!component->isRunning())
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

    component->clearUserCommand();
    component->setSuppressUserInteraction(true);

    if (component->isRunning())
    {
      cedar::aux::LogSingleton::getInstance()->warning(
        component->prettifyName() + " is still connected and running.",
        CEDAR_CURRENT_FUNCTION_NAME);
    }
    else
    {
      this->resetState();
    }
  }
}

void cedar::proc::steps::Component::compute(const cedar::proc::Arguments&)
{
  auto component = this->getComponent();

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

  // unlock the suppression of user commands when the architecture is running
  component->setSuppressUserInteraction(false);

  // if no inputs are present, there is nothing to do (i.e., no inputs have to be passed to the component)
  if (!this->hasSlotForRole(cedar::proc::DataRole::INPUT))
  {
    return;
  }

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

  if (auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
  {
    if (mat_data->getData().type() != CV_64F)
    {
      return cedar::proc::DataSlot::VALIDITY_ERROR;
    }

    auto type = this->getComponent()->getCommandTypeForName(name);
    // check that the matrix has the same dimensionality as the command it is connecting to
    unsigned int size = this->getComponent()->getCommandDimensionality(type);

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

  std::string selected_group = this->_mGroup->getValue();
  if (selected_group.empty())
  {
    auto installed_set = component->getInstalledCommandTypes();
    commands.assign(installed_set.begin(), installed_set.end());
  }
  else
  {
    commands = component->getCommandsInGroup(selected_group);
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
  auto component = this->getComponent();
  auto measurements = component->getInstalledMeasurementTypes();

  for (const auto& measurement : measurements)
  {
    std::string name = component->getNameForMeasurementType(measurement);
    auto data = component->getMeasurementData(measurement);
    this->declareOutput(name, data);
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
  this->_mGroup->setConstant(!component->hasCommandGroups());
  this->_mGroup->setComponent(component);
}

void cedar::proc::steps::Component::reset()
{
  auto component = this->getComponent();
  component->clearAll();
}

void cedar::proc::steps::Component::inputConnectionChanged(const std::string& /*inputName*/)
{
  auto component = this->getComponent();
  component->clearUserCommand();
}


