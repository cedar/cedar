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

    File:        ObjectListParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 09

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/ObjectListParameter.h"
#include "cedar/auxiliaries/gui/PropertyPane.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/TypeBasedFactory.h"
#include "cedar/auxiliaries/Singleton.h"

// SYSTEM INCLUDES
#include <QGridLayout>
#include <QDialog>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
// associate aux::gui parameter with the aux parameter
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered = cedar::aux::gui::ParameterFactorySingleton::getInstance()->addDerived
      <
        cedar::aux::ObjectListParameter,
        cedar::aux::gui::ObjectListParameter
      >();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::ObjectListParameter::ObjectListParameter()
{
  int margins = 0;
  int spacing = 2;
  int button_size = 20;

  QGridLayout *p_layout = new QGridLayout();
  p_layout->setContentsMargins(margins, margins, margins, margins);
  p_layout->setSpacing(spacing);
  this->setLayout(p_layout);

  mpTypeSelector = new QComboBox();
  mpTypeSelector->setToolTip("Select the type of the object you wish to add.");
  p_layout->addWidget(mpTypeSelector, 0, 0);

  mpAddButton = new QPushButton("+");
  mpAddButton->setToolTip("Add an object of the selected type to the end of the list.");
  mpAddButton->setMaximumWidth(button_size);
  p_layout->addWidget(mpAddButton, 0, 1);

  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
  QObject::connect(this->mpAddButton, SIGNAL(clicked()), this, SLOT(addClicked()));
}

cedar::aux::gui::ObjectListParameter::~ObjectListParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::ObjectListParameter::parameterPointerChanged()
{
  cedar::aux::ObjectListParameterPtr parameter = this->getObjectList();
  // Fill types -------------------------------------------------------------------------------
  this->mpTypeSelector->clear();

  std::vector<std::string> types;
  parameter->listTypes(types);

  for (size_t i = 0; i < types.size(); ++i)
  {
    QString type_id = QString::fromStdString(types.at(i));
    this->mpTypeSelector->addItem(this->prettyTypeId(type_id));
    this->mpTypeSelector->setItemData(i, type_id);
  }
}

QString cedar::aux::gui::ObjectListParameter::prettyTypeId(const QString& typeId) const
{
  QStringList parts = typeId.split('.');
  QString class_id = parts.back();
  parts.pop_back();
  QString namespace_id = parts.join(".");
  return class_id + " (" + namespace_id + ")";
}

void cedar::aux::gui::ObjectListParameter::addClicked()
{
  this->getObjectList()->pushBack(this->getSelectedType());
}

std::string cedar::aux::gui::ObjectListParameter::getSelectedType() const
{
  int index = this->mpTypeSelector->currentIndex();
  if (index != -1)
  {
    return this->mpTypeSelector->itemData(index).toString().toStdString();
  }
  else
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException, "No type selected.");
  }
}
