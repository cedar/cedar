/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ComponentStepView.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 12 02

    Description: Source file for the class cedar::proc::gui::ComponentStepView.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/gui/stepViews/ComponentStepView.h"

// CEDAR INCLUDES
#include "cedar/processing/steps/Component.h"
#include "cedar/devices/ComponentDeclaration.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::ComponentStepView::connectableChanged()
{
  this->cedar::proc::gui::DefaultConnectableIconView::connectableChanged();

  auto component_step = this->getComponentStep();
  QObject::connect(component_step.get(), SIGNAL(componentChanged()), this, SLOT(updateIcon()));

  this->updateIcon();
}

cedar::proc::steps::ConstComponentPtr cedar::proc::gui::ComponentStepView::getComponentStep() const
{
  return boost::dynamic_pointer_cast<cedar::proc::steps::ConstComponent>(this->getConnectable());
}

void cedar::proc::gui::ComponentStepView::updateIcon()
{
  auto component_step = boost::dynamic_pointer_cast<cedar::proc::steps::ConstComponent>(this->getConnectable());
  CEDAR_DEBUG_ASSERT(component_step);

  const QString default_icon_path = ":/cedar/dev/gui/icons/generic_hardware_icon.svg";

  if (component_step->hasComponent())
  {
    // get the component
    auto component = component_step->getComponent();

    // check if an icon path has been definded in the components configuration
    QString icon_path = component->getIconPath();
    if (!icon_path.isEmpty())
    {
      // defined icon path found, use it
      this->setIconPath(icon_path);
      return;
    }

    // find the declaration of the component from the component manager
    cedar::dev::ConstComponentDeclarationPtr declaration;
    try
    {
      auto plugin_declaration = cedar::dev::ComponentDeclarationManagerSingleton::getInstance()->getDeclarationOf(component);
      declaration = boost::dynamic_pointer_cast<cedar::dev::ConstComponentDeclaration>(plugin_declaration);
    }
    catch (cedar::aux::UnknownTypeException)
    {
      // no declaration found, use default icon
      this->setIconPath(default_icon_path);
      return;
    }

    if (!declaration)
    {
      // no declaration found, use default icon
      this->setIconPath(default_icon_path);
      return;
    }

    // get the icon from the component and apply it
    if (!declaration->getIconPath().empty())
    {
      this->setIconPath(QString::fromStdString(declaration->getIconPath()));
    }
    else
    {
      this->setIconPath(default_icon_path);
    }
  }
  else
  {
    // apply the default icon
    this->setIconPath(default_icon_path);
  }
}
