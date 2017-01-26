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

    File:        OneTimeMessageDialog.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 11 11

    Description: Source file for the class cedar::proc::gui::OneTimeMessageDialog.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/OneTimeMessageDialog.h"
#include "cedar/version.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::OneTimeMessageDialog::OneTimeMessageDialog
(
  const std::vector<cedar::proc::gui::Settings::OneTimeMessagePtr>& messages,
  bool markAsRead,
  QWidget* pParent
)
:
QDialog(pParent),
mMessages(messages),
mMarkAsRead(markAsRead),
mCurrentMessage(0)
{
  this->setupUi(this);

  if (!this->mMessages.empty())
  {
    this->showMessage(0);
  }

  QObject::connect(this->mpNextButton, SIGNAL(clicked()), this, SLOT(showNextMessage()));
  QObject::connect(this->mpPreviousButton, SIGNAL(clicked()), this, SLOT(showPreviousMessage()));
  QObject::connect(this, SIGNAL(accepted()), this, SLOT(dialogAccepted()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::OneTimeMessageDialog::dialogAccepted()
{
  if (this->mMarkAsRead)
  {
    cedar::proc::gui::SettingsSingleton::getInstance()->markAsRead(this->mMessages);
  }
}

void cedar::proc::gui::OneTimeMessageDialog::showMessage(size_t index)
{
  CEDAR_DEBUG_ASSERT(index < this->mMessages.size());
  auto message = this->mMessages.at(index);
  this->mCurrentMessage = index;

  QString message_text;
  message_text += QString("<p><i>relates to version %1.%2.%3</i></p>")
                    .arg(CEDAR_GET_VERSION_MAJOR(message->mVersion))
                    .arg(CEDAR_GET_VERSION_MINOR(message->mVersion))
                    .arg(CEDAR_GET_VERSION_BUGFIX(message->mVersion));
  message_text += QString::fromStdString(message->mMessage);

  this->mpTitleLabel->setText(QString::fromStdString(message->mTitle));
  this->mpMessageText->setText(message_text);

  this->mpPreviousButton->setEnabled(index != 0);
  this->mpNextButton->setEnabled(index < this->mMessages.size() - 1);

  this->mpPlaceLabel->setText(QString("%1/%2").arg(index + 1).arg(this->mMessages.size()));
}

void cedar::proc::gui::OneTimeMessageDialog::showNextMessage()
{
  if (this->mCurrentMessage >= this->mMessages.size() - 1)
  {
    return;
  }

  ++this->mCurrentMessage;
  this->showMessage(this->mCurrentMessage);
}

void cedar::proc::gui::OneTimeMessageDialog::showPreviousMessage()
{
  if (this->mCurrentMessage == 0)
  {
    return;
  }

  --this->mCurrentMessage;
  this->showMessage(this->mCurrentMessage);
}
