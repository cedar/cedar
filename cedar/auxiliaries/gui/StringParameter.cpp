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

    File:        StringParameter.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 06

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/StringParameter.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/TypeBasedFactory.h"
#include "cedar/auxiliaries/Singleton.h"

// SYSTEM INCLUDES
#include <QHBoxLayout>
#include <QToolTip>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// associate aux::gui parameter with the aux parameter
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered = cedar::aux::gui::ParameterFactorySingleton::getInstance()->add
      <
        cedar::aux::StringParameter,
        cedar::aux::gui::StringParameter
      >();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::StringParameter::StringParameter(QWidget *pParent)
:
cedar::aux::gui::Parameter(pParent),
mpPlaceHolderText("")
{
  this->setLayout(new QHBoxLayout());
  this->mpEdit = new QLineEdit();
  this->layout()->setContentsMargins(0, 0, 0, 0);
  this->layout()->addWidget(this->mpEdit);

  this->mpEdit->setPlaceholderText(QString::fromStdString(mpPlaceHolderText));

  QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
}

//!@brief Destructor
cedar::aux::gui::StringParameter::~StringParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::StringParameter::propertiesChanged()
{
  auto parameter = boost::dynamic_pointer_cast<cedar::aux::StringParameter>(this->getParameter());
  cedar::aux::Parameter::ReadLocker locker(parameter.get());
  this->mpEdit->setDisabled(parameter->isConstant());
}

void cedar::aux::gui::StringParameter::setReadOnly(bool readOnly)
{
  this->mpEdit->setReadOnly(readOnly);
}

void cedar::aux::gui::StringParameter::parameterPointerChanged()
{
  cedar::aux::StringParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::StringParameter>(this->getParameter());
  this->mpEdit->setText(parameter->getValue().c_str());
  QObject::connect(this->mpEdit, SIGNAL(textEdited(const QString&)), this, SLOT(textEdited(const QString&)));
  QObject::connect(parameter.get(), SIGNAL(valueChanged()), this, SLOT(parameterValueChanged()));

  this->propertiesChanged();
}

void cedar::aux::gui::StringParameter::parameterValueChanged()
{
  auto parameter = boost::dynamic_pointer_cast<cedar::aux::StringParameter>(this->getParameter());
  CEDAR_DEBUG_ASSERT(parameter);
  QString value = QString::fromStdString(parameter->getValue());
  if (this->mpEdit->text() != value)
  {
    this->mpEdit->setText(value);
  }
}

void cedar::aux::gui::StringParameter::textEdited(const QString& text)
{
  this->setStyleSheet("");
  auto parameter = boost::dynamic_pointer_cast<cedar::aux::StringParameter>(this->getParameter());
  try
  {
    parameter->setValue(text.toStdString());
  }
  catch (cedar::aux::ValidationFailedException& exc)
  {
    QPoint pos = this->mapToGlobal(QPoint(0, this->height()/2));

    this->setStyleSheet("QLineEdit {background-color: red; }");
    QToolTip::showText(pos, QString::fromStdString(exc.getMessage()), this);
  }
}

void cedar::aux::gui::StringParameter::setPlaceHolderText(std::string placeHolder)
{
  this->mpPlaceHolderText = placeHolder;
  this->mpEdit->setPlaceholderText(QString::fromStdString(mpPlaceHolderText));
}

