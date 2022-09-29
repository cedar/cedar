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

    File:        AutoPositionStepsDialog.h

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2022 09 12

    Description: Header file for the class cedar::proc::gui::AutoPositionStepsDialog.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_AUTO_POSITION_STEPS_DIALOG_H
#define CEDAR_PROC_GUI_AUTO_POSITION_STEPS_DIALOG_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ui_AutoPositionStepsDialog.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/AutoPositionStepsDialog.fwd.h"
#include "cedar/processing/gui/layout/ForceDirectedLayout.fwd.h"

// SYSTEM INCLUDES
#include <QDialog>

/*! Auto-position steps dialog
 *
 * Dialog to enable the user to rearrange the steps in an automatic manner
 */
class cedar::proc::gui::AutoPositionStepsDialog : public QDialog, public Ui_AutoPositionStepsDialog
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  AutoPositionStepsDialog(QWidget *pParent = NULL);

  //!@brief Destructor
  virtual ~AutoPositionStepsDialog();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  void setGroup(cedar::proc::gui::GroupPtr pGroup);

public slots:

  void liveUpdateChecked(bool toggled);

  void arrange();

  void reset();

  void attractionForceValueChanged();

  void attractionForceParameterPointerChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:

  cedar::proc::gui::GroupPtr mpGroup;
  cedar::proc::gui::layout::ForceDirectedLayoutPtr mpForceDirectedLayout;
  cedar::aux::DoubleParameterPtr mpAttractionForceParameter;
  bool liveUpdate;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::AutoPositionStepsDialog

#endif // CEDAR_PROC_GUI_AUTO_POSITION_STEPS_DIALOG_H

