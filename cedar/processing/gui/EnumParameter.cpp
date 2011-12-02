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

// LOCAL INCLUDES
#include "cedar/processing/gui/EnumParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"

// PROJECT INCLUDES
#include "cedar/defines.h"

// SYSTEM INCLUDES
#include <QHBoxLayout>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::EnumParameter::EnumParameter(QWidget *pParent)
:
cedar::proc::gui::Parameter(pParent)
{
  this->setLayout(new QHBoxLayout());
  this->mpEdit = new QComboBox();
  this->layout()->setContentsMargins(0, 0, 0, 0);
  this->layout()->addWidget(this->mpEdit);

  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
}

//!@brief Destructor
cedar::proc::gui::EnumParameter::~EnumParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::EnumParameter::parameterPointerChanged()
{
  cedar::aux::EnumParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::EnumParameter>(this->getParameter());

  this->mpEdit->clear();
  int select_index = -1;
  for (size_t i = 0; i < parameter->getEnumDeclaration().list().size(); ++i)
  {
    const cedar::aux::Enum& enum_val = parameter->getEnumDeclaration().list().at(i);
    if (enum_val == parameter->getValue())
    {
      select_index = static_cast<int>(i);
    }
    this->mpEdit->addItem(enum_val.prettyString().c_str(), QVariant(QString(enum_val.name().c_str())));
  }
  if(select_index != -1)
  {
    this->mpEdit->setCurrentIndex(select_index);
  }

  QObject::connect(this->mpEdit, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(currentIndexChanged(const QString&)));
}

void cedar::proc::gui::EnumParameter::currentIndexChanged(const QString&)
{
  if (this->mpEdit->currentIndex() != -1)
  {
    cedar::aux::EnumParameterPtr parameter;
    parameter = boost::dynamic_pointer_cast<cedar::aux::EnumParameter>(this->getParameter());
    QString value = this->mpEdit->itemData(this->mpEdit->currentIndex(), Qt::UserRole).toString();
    parameter->set(value.toStdString());
  }
}

