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

    File:        ExceptionDialog.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 07 12

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/ExceptionDialog.h"
#include "cedar/auxiliaries/ExceptionBase.h"
#include "cedar/auxiliaries/utilities.h"

// SYSTEM INCLUDES
#include <QStyle>
#include <QApplication>
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::ExceptionDialog::ExceptionDialog()
{
  this->setupUi(this);
  this->mpDetails->setVisible(false);
  QIcon icon = QApplication::style()->standardIcon(QStyle::SP_MessageBoxCritical);
  this->mpIcon->setPixmap(icon.pixmap(64, 64));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::ExceptionDialog::setAdditionalString(const std::string& intro)
{
  this->mAdditionalString = QString::fromStdString(intro);
}

void cedar::aux::gui::ExceptionDialog::displayCedarException(const cedar::aux::ExceptionBase& exception)
{
  this->display(exception.getMessage(), exception.exceptionInfo());
}

void cedar::aux::gui::ExceptionDialog::displayStdException(const std::exception& exception)
{
  std::string type = cedar::aux::unmangleName(typeid(exception));
  this->displayGenericException(exception.what(), type);
}

void cedar::aux::gui::ExceptionDialog::displayGenericException(const std::string& what, const std::string& type)
{
  this->display(what, "The type of the exception is: " + type);
}

void cedar::aux::gui::ExceptionDialog::displayUnknownException()
{
  this->display("Unknown exception.", "The type of the exception was unexpected.");
}

void cedar::aux::gui::ExceptionDialog::display(const std::string& message, const std::string& details)
{
  this->mpExceptionInfo->setText(QString::fromStdString(message));

  this->mpDetails->setText(QString::fromStdString(details));

  this->addAdditionalText();
}

void cedar::aux::gui::ExceptionDialog::addAdditionalText()
{
  if (!this->mAdditionalString.isEmpty())
  {
    this->mpExceptionInfo->setText(this->mpExceptionInfo->text() + "\n\n" + this->mAdditionalString);
  }
}
