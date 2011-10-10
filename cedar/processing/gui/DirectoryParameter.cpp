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

    File:        DirectoryParameter.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 22

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "processing/gui/DirectoryParameter.h"
#include "auxiliaries/DirectoryParameter.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QHBoxLayout>
#include <QPushButton>
#include <iostream>
#include <QFileDialog>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::DirectoryParameter::DirectoryParameter(QWidget *pParent)
:
cedar::proc::gui::ParameterBase(pParent)
{
  QHBoxLayout *p_layout = new QHBoxLayout();
  this->setLayout(p_layout);
  this->mpEdit = new QLineEdit();
  this->mpEdit->setReadOnly(true);
  p_layout->addWidget(this->mpEdit);

  QPushButton *p_button = new QPushButton(">");
  p_button->setMinimumWidth(20);
  p_button->setMaximumWidth(30);
  p_layout->addWidget(p_button);

  p_layout->setContentsMargins(0, 0, 0, 0);
  p_layout->setStretch(0, 1);
  p_layout->setStretch(1, 0);

  QObject::connect(p_button, SIGNAL(clicked()), this, SLOT(onBrowseClicked()));
  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
}

//!@brief Destructor
cedar::proc::gui::DirectoryParameter::~DirectoryParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::DirectoryParameter::parameterPointerChanged()
{
  cedar::aux::DirectoryParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::DirectoryParameter>(this->getParameter());
  this->mpEdit->setReadOnly(false);
  this->mpEdit->setText(parameter->get().absolutePath());
  this->mpEdit->setReadOnly(true);

  QObject::connect(parameter.get(), SIGNAL(valueChanged()), this, SLOT(parameterValueChanged()));
}

void cedar::proc::gui::DirectoryParameter::parameterValueChanged()
{
  cedar::aux::DirectoryParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::DirectoryParameter>(this->getParameter());
  this->mpEdit->setReadOnly(false);
  this->mpEdit->setText(parameter->get().absolutePath());
  this->mpEdit->setReadOnly(true);
}

void cedar::proc::gui::DirectoryParameter::onBrowseClicked()
{
  cedar::aux::DirectoryParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::DirectoryParameter>(this->getParameter());
  QString value = QFileDialog::getExistingDirectory(this, "Select a directory", parameter->get().absolutePath());
  parameter->set(value.toStdString());
}
