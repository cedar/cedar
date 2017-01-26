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

    File:        OneTimeMessageDialog.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 11 11

    Description: Header file for the class cedar::proc::gui::OneTimeMessageDialog.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_ONE_TIME_MESSAGE_DIALOG_H
#define CEDAR_PROC_GUI_ONE_TIME_MESSAGE_DIALOG_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ui_OneTimeMessageDialog.h"
#include "cedar/processing/gui/Settings.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/OneTimeMessageDialog.fwd.h"

// SYSTEM INCLUDES
#include <QDialog>
#include <vector>


/*!@brief A dialog for displaying one-time messages.
 */
class cedar::proc::gui::OneTimeMessageDialog : public QDialog, public Ui_OneTimeMessageDialog
{
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  OneTimeMessageDialog
  (
    const std::vector<cedar::proc::gui::Settings::OneTimeMessagePtr>& messages,
    bool markAsRead = false,
    QWidget* pParent = nullptr
  );

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

public slots:
  void showNextMessage();
  void showPreviousMessage();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void showMessage(size_t index);

private slots:
  void dialogAccepted();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! The messages to be displayed
  std::vector<cedar::proc::gui::Settings::OneTimeMessagePtr> mMessages;

  //! If true, the messages will be marked as read when the dialog is closed.
  bool mMarkAsRead;

  size_t mCurrentMessage;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::OneTimeMessageDialog

#endif // CEDAR_PROC_GUI_ONE_TIME_MESSAGE_DIALOG_H

