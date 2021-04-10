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
#include "cedar/processing/auxiliaries/gui/StringParameter.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/processing/auxiliaries/TypeBasedFactory.h"
#include "cedar/processing/auxiliaries/Singleton.h"
#include "cedar/processing/gui/Ide.h"
#include "cedar/processing/undoRedo/UndoStack.h"
#include "cedar/processing/undoRedo/commands/ChangeParameterValue.h"
//#include "cedar/processing/undoRedo/commands/ChangeNameParameterValue.h"

// SYSTEM INCLUDES
#include <QHBoxLayout>
#include <QToolTip>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// associate aux::gui parameter with the aux parameter
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registered = cedar::proc::aux::gui::ParameterFactorySingleton::getInstance()->add
      <
        cedar::aux::StringParameter,
        cedar::proc::aux::gui::StringParameter
      >();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::aux::gui::StringParameter::StringParameter(QWidget *pParent)
:
cedar::proc::aux::gui::Parameter(pParent),
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
cedar::proc::aux::gui::StringParameter::~StringParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::aux::gui::StringParameter::propertiesChanged()
{
  auto parameter = boost::dynamic_pointer_cast<cedar::aux::StringParameter>(this->getParameter());
  cedar::aux::Parameter::ReadLocker locker(parameter.get());
  this->mpEdit->setDisabled(parameter->isConstant());
}

void cedar::proc::aux::gui::StringParameter::setReadOnly(bool readOnly)
{
  this->mpEdit->setReadOnly(readOnly);
}

void cedar::proc::aux::gui::StringParameter::parameterPointerChanged()
{
  cedar::aux::StringParameterPtr parameter;
  parameter = boost::dynamic_pointer_cast<cedar::aux::StringParameter>(this->getParameter());
  this->mpEdit->setText(parameter->getValue().c_str());
  QObject::connect(this->mpEdit, SIGNAL(textEdited(const QString&)), this, SLOT(textEdited(const QString&)));
  QObject::connect(parameter.get(), SIGNAL(valueChanged()), this, SLOT(parameterValueChanged()));

  this->propertiesChanged();
}

void cedar::proc::aux::gui::StringParameter::parameterValueChanged()
{
  auto parameter = boost::dynamic_pointer_cast<cedar::aux::StringParameter>(this->getParameter());
  CEDAR_DEBUG_ASSERT(parameter);
  QString value = QString::fromStdString(parameter->getValue());
  if (this->mpEdit->text() != value)
  {
    this->mpEdit->setText(value);
  }
}

void cedar::proc::aux::gui::StringParameter::textEdited(const QString& text)
{
  this->setStyleSheet("");
  auto parameter = boost::dynamic_pointer_cast<cedar::aux::StringParameter>(this->getParameter());
  try
  {
    std::string oldValue = parameter->getValue();
    // If parameter belongs to a step, push to undo stack (e.g. settings parameter should not be undoable)
    if(dynamic_cast<cedar::aux::NamedConfigurable*>(parameter->getOwner()))
    {
      // Find the scene
      cedar::proc::gui::Scene *scene;

      QObject *parent = this;
      while (parent != nullptr)
      {
        if (auto ide = dynamic_cast<cedar::proc::gui::Ide *>(parent))
        {
          scene = ide->mpProcessingDrawer->getScene();
        }
        parent = parent->parent();
      }
      CEDAR_ASSERT(scene != nullptr);
      cedar::proc::gui::Ide::mpUndoStack->push(
              new cedar::proc::undoRedo::commands::ChangeParameterValue<std::string>(
                      parameter.get(), oldValue, text.toStdString(), scene));
    }
    else
    {
      parameter->setValue(text.toStdString());
    }
  }
  catch (cedar::aux::ValidationFailedException& exc)
  {
    QPoint pos = this->mapToGlobal(QPoint(0, this->height()/2));

    this->setStyleSheet("QLineEdit {background-color: red; }");
    QToolTip::showText(pos, QString::fromStdString(exc.getMessage()), this);
  }
}

void cedar::proc::aux::gui::StringParameter::setPlaceHolderText(std::string placeHolder)
{
  this->mpPlaceHolderText = placeHolder;
  this->mpEdit->setPlaceholderText(QString::fromStdString(mpPlaceHolderText));
}

