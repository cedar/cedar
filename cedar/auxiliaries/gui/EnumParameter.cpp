/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        EnumParameter.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 28

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/EnumParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/defines.h"
#include "cedar/auxiliaries/TypeBasedFactory.h"
#include "cedar/auxiliaries/Singleton.h"

// SYSTEM INCLUDES
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// associate aux::gui parameter with the aux parameter
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered = cedar::aux::gui::ParameterFactorySingleton::getInstance()->add
      <
        cedar::aux::EnumParameter,
        cedar::aux::gui::EnumParameter
      >();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::EnumParameter::EnumParameter(QWidget *pParent)
:
cedar::aux::gui::Parameter(pParent)
{
  this->setLayout(new QHBoxLayout());
  this->mpEdit = new QComboBox();
  this->layout()->setContentsMargins(0, 0, 0, 0);
  this->layout()->addWidget(this->mpEdit);

  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
}

//!@brief Destructor
cedar::aux::gui::EnumParameter::~EnumParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::EnumParameter::parameterPointerChanged()
{
  cedar::aux::EnumParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::EnumParameter>(this->getParameter());

  this->mpEdit->clear();
  int select_index = -1;
  int first_enabled = -1;
  parameter->lockForRead();
  for (size_t i = 0; i < parameter->getEnumDeclaration().list().size(); ++i)
  {
    const cedar::aux::Enum& enum_val = parameter->getEnumDeclaration().list().at(i);
    bool enabled = parameter->isEnabled(enum_val);
    if (first_enabled == -1 && enabled)
    {
      first_enabled = static_cast<int>(i);
    }
    if (enum_val == parameter->getValue() && enabled)
    {
      select_index = static_cast<int>(i);
    }
    QVariant data(QString(enum_val.name().c_str()));
    this->mpEdit->addItem(enum_val.prettyString().c_str(), data);
    int item_index = this->mpEdit->findData(data);
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(this->mpEdit->model());

    CEDAR_DEBUG_ASSERT(model != NULL);

    QModelIndex index = model->index(item_index, this->mpEdit->modelColumn(), this->mpEdit->rootModelIndex());
    QStandardItem* p_item = model->itemFromIndex(index);
    CEDAR_DEBUG_ASSERT(p_item != NULL);
    p_item->setEnabled(enabled);
  }
  parameter->unlock();
  if (select_index != -1)
  {
    this->mpEdit->setCurrentIndex(select_index);
  }
  else // may set the index to -1 or first enabled option
  {
    this->mpEdit->setCurrentIndex(first_enabled);
    this->currentIndexChanged(this->mpEdit->currentText());
  }

  QObject::connect
  (
    this->mpEdit,
    SIGNAL(currentIndexChanged(const QString&)),
    this,
    SLOT(currentIndexChanged(const QString&))
  );
}

void cedar::aux::gui::EnumParameter::currentIndexChanged(const QString&)
{
  if (this->mpEdit->currentIndex() != -1)
  {
    cedar::aux::EnumParameterPtr parameter;
    parameter = boost::dynamic_pointer_cast<cedar::aux::EnumParameter>(this->getParameter());
    QString value = this->mpEdit->itemData(this->mpEdit->currentIndex(), Qt::UserRole).toString();
    parameter->setValue(value.toStdString(), true);
  }
}
