/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        GroupParameterDesigner.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 02 27

    Description: Source file for the class cedar::proc::gui::GroupParameterDesigner.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/GroupParameterDesigner.h"
#include "cedar/processing/Group.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/ParameterDeclaration.h"

// SYSTEM INCLUDES
#include <QVBoxLayout>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::GroupParameterDesigner::GroupParameterDesigner(cedar::proc::GroupPtr group)
:
mGroup(group)
{
  this->setupUi(this);

  this->fillParameterTypeBox();

  QObject::connect(this->mpAddParameterButton, SIGNAL(clicked()), this, SLOT(addClicked()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::GroupParameterDesigner::fillParameterTypeBox()
{
  for (auto category : cedar::aux::ParameterDeclarationManagerSingleton::getInstance()->listCategories())
  {
    //!@todo category separator
    for (auto declaration : cedar::aux::ParameterDeclarationManagerSingleton::getInstance()->getCategoryEntries(category))
    {
      std::string class_name = declaration->getClassName();
      this->mpTypeSelector->addItem(QString::fromStdString(class_name));
    }
  }
}

void cedar::proc::gui::GroupParameterDesigner::addClicked()
{
  int index = this->mpTypeSelector->currentIndex();

  // check if anything is selected at all
  if (index < 0)
  {
    return;
  }

  // get the class name
  std::string class_name = this->mpTypeSelector->itemText(index).toStdString();
  this->addParameter(class_name);
}

void cedar::proc::gui::GroupParameterDesigner::addParameter(const std::string& type)
{
  auto parameter = cedar::aux::ParameterDeclarationManagerSingleton::getInstance()->allocate(type);

  parameter->setName("new parameter");
  parameter->setOwner(this->mGroup.get());
}
