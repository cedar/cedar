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

    File:        EnumParameter.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 28

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/auxiliaries/gui/EnumParameter.h"
#include "cedar/processing/gui/Ide.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/undoRedo/commands/ChangeParameterValue.h"
#include "cedar/processing/undoRedo/UndoStack.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/defines.h"
#include "cedar/processing/auxiliaries/TypeBasedFactory.h"
#include "cedar/processing/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/NamedConfigurable.h"

// SYSTEM INCLUDES
#include <QGraphicsItem>
#include <QWidget>
#include <QObject>
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <iostream>
#include <cedar/processing/gui/Ide.h>

//----------------------------------------------------------------------------------------------------------------------
// associate aux::gui parameter with the aux parameter
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered = cedar::proc::aux::gui::ParameterFactorySingleton::getInstance()->add
      <
        cedar::aux::EnumParameter,
        cedar::proc::aux::gui::EnumParameter
      >();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::aux::gui::EnumParameter::EnumParameter(QWidget *pParent)
:
cedar::proc::aux::gui::Parameter(pParent)
{
  this->setLayout(new QHBoxLayout());
  this->mpEdit = new QComboBox();
  this->layout()->setContentsMargins(0, 0, 0, 0);
  this->layout()->addWidget(this->mpEdit);

  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
}

//!@brief Destructor
cedar::proc::aux::gui::EnumParameter::~EnumParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::aux::gui::EnumParameter::parameterValueChanged()
{
  bool blocked = this->mpEdit->blockSignals(true);

  auto parameter = boost::dynamic_pointer_cast<cedar::aux::EnumParameter>(this->getParameter());
  int index = -1;
  QString current_value = QString::fromStdString(parameter->getValue().name());
  for (int i = 0; i < this->mpEdit->count(); ++i)
  {
    if (this->mpEdit->itemData(i) == current_value)
    {
      index = i;
      break;
    }
  }
  CEDAR_DEBUG_ASSERT(index != -1);

  this->mpEdit->setCurrentIndex(index);
  this->mpEdit->blockSignals(blocked);
}

void cedar::proc::aux::gui::EnumParameter::parameterPointerChanged()
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
    auto explanation = enum_val.explanatoryString();
    if (explanation != "")
    {
      this->mpEdit->setItemData(item_index, explanation.c_str(), Qt::ToolTipRole);
    }
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

  QObject::connect(parameter.get(), SIGNAL(valueChanged()), this, SLOT(parameterValueChanged()));
  this->propertiesChanged();
}

void cedar::proc::aux::gui::EnumParameter::currentIndexChanged(const QString&)
{
  if (this->mpEdit->currentIndex() != -1)
  {
    cedar::aux::EnumParameterPtr parameter;
    parameter = boost::dynamic_pointer_cast<cedar::aux::EnumParameter>(this->getParameter());
    QString value = this->mpEdit->itemData(this->mpEdit->currentIndex(), Qt::UserRole).toString();

    // If parameter belongs to a step, push to undo stack (e.g. settings parameter should not be undoable)
    if(dynamic_cast<cedar::aux::NamedConfigurable*>(parameter->getOwner()))
    {
      //Find the scene
      cedar::proc::gui::Scene* scene;

      QObject* parent = this;
      while(parent != nullptr)
      {
        if(auto ide = dynamic_cast<cedar::proc::gui::Ide*>(parent))
        {
          scene = ide->mpProcessingDrawer->getScene();
        }
        parent = parent->parent();
      }
      CEDAR_ASSERT(scene != nullptr);

      cedar::proc::gui::Ide::mpUndoStack->push(
              new cedar::proc::undoRedo::commands::ChangeParameterValue<std::string, cedar::aux::EnumParameter>(
                      parameter.get(), parameter->getValue().name(), value.toStdString(), true, scene));
    }
    else
    {
      parameter->setValue(value.toStdString(), true);
    }
  }
}

void cedar::proc::aux::gui::EnumParameter::propertiesChanged()
{
  cedar::aux::ParameterPtr parameter = this->getParameter();

  bool blocked = this->mpEdit->blockSignals(true);

  parameter->lockForRead();
  this->mpEdit->setDisabled(parameter->isConstant());
  parameter->unlock();

  this->mpEdit->blockSignals(blocked);
}
