/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Configurable.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 01 16

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/Configurable.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/Parameter.h"

// SYSTEM INCLUDES
#include <QVBoxLayout>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::Configurable::Configurable()
{
  // create layout
  QVBoxLayout* p_layout = new QVBoxLayout();
  this->setLayout(p_layout);

  // insert property tree
  this->mpPropertyTree = new QTreeWidget();
  p_layout->addWidget(mpPropertyTree, 1);
  p_layout->setContentsMargins(0, 0, 0, 0);

  // setup columns
  this->mpPropertyTree->setColumnCount(2);
  QStringList header_labels;
  header_labels << "Property" << "Value";
  this->mpPropertyTree->setHeaderLabels(header_labels);
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::Configurable::display(cedar::aux::ConfigurablePtr configurable)
{
  this->clear();

  std::string type_name = cedar::aux::objectTypeToString(configurable);
  QStringList text;
  text << QString::fromStdString(type_name);
  QTreeWidgetItem* p_item = new QTreeWidgetItem(text);
  this->mpPropertyTree->addTopLevelItem(p_item);
  this->mpPropertyTree->setFirstItemColumnSpanned(p_item, true);

  this->append(configurable, p_item);
}

void cedar::aux::gui::Configurable::append(cedar::aux::ConfigurablePtr configurable, QTreeWidgetItem* pItem)
{
  auto parameters = configurable->getParameters();
  for (auto param_iter = parameters.begin(); param_iter != parameters.end(); ++param_iter)
  {
    cedar::aux::ConstParameterPtr parameter = *param_iter;
    QTreeWidgetItem* parameter_item = new QTreeWidgetItem();
    parameter_item->setText(0, QString::fromStdString(parameter->getName()));
    pItem->addChild(parameter_item);
  }

  auto children = configurable->configurableChildren();
  for (auto child_iter = children.begin(); child_iter != children.end(); ++child_iter)
  {
    const std::string& child_name = child_iter->first;
    cedar::aux::ConfigurablePtr child = child_iter->second;
    QTreeWidgetItem* child_item = new QTreeWidgetItem();
    child_item->setText(0, QString::fromStdString(child_name));
    pItem->addChild(child_item);
    this->append(child, child_item);
  }
}

void cedar::aux::gui::Configurable::clear()
{
}
