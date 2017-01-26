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

    File:        ExceptionDialog.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 07 12

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_EXCEPTION_DIALOG_H
#define CEDAR_AUX_GUI_EXCEPTION_DIALOG_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/ui_ExceptionDialog.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/ExceptionDialog.fwd.h"
#include "cedar/auxiliaries/ExceptionBase.fwd.h"

// SYSTEM INCLUDES
#include <QDialog>


/*!@brief A dialog for showing exceptions.
 */
class cedar::aux::gui::ExceptionDialog : public QDialog, public Ui_ExceptionDialog
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
  ExceptionDialog();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Set a string that is shown before the exception information.
  void setAdditionalString(const std::string& intro);

  //! Display the given cedar exception.
  void displayCedarException(const cedar::aux::ExceptionBase& exception);

  //! Display a std::exception.
  void displayStdException(const std::exception& exception);

  //! Display generic exception information
  void displayGenericException(const std::string& what, const std::string& type);

  //! Display an unknown exception.
  void displayUnknownException();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void addAdditionalText();

  void display(const std::string& message, const std::string& details);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  QString mAdditionalString;

}; // class cedar::aux::gui::ExceptionDialog

#endif // CEDAR_AUX_GUI_EXCEPTION_DIALOG_H

