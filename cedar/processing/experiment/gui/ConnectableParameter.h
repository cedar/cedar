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

    File:        ConnectableParameter.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2015 01 16

    Description: Header file for the class cedar::proc::experiment::gui::ConnectableParameter.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_GUI_CONNECTABLE_PARAMETER_H
#define CEDAR_PROC_EXPERIMENT_GUI_CONNECTABLE_PARAMETER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/Parameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/gui/ConnectableParameter.fwd.h"

// SYSTEM INCLUDES
#include <QComboBox>
#include <QBoxLayout>

/*!@brief GUI implementation of cedar::proc::experiment::ConnectableParameter
 *
 */
class cedar::proc::experiment::gui::ConnectableParameter : public cedar::aux::gui::Parameter
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ConnectableParameter(QWidget* pParent = NULL);

  //!@brief Destructor
  virtual ~ConnectableParameter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

public slots:
  //!@brief handles a change in the step combo box
  void connectableChanged();
  //!@brief handles a change of the associated parameter
  void parameterPointerChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

signals:
  //!@brief react to a changing connectable
  void connectableChangedSignal();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Updates the steps for the combo box
  void updateConnectables();

  //!@brief Updates the parameter widget when the property has changed
  void updateValue();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //!@brief The element selector
  QComboBox* mpConnectableComboBox;

}; // class cedar::proc::experiment::gui::ConnectableParameter

#endif // CEDAR_PROC_EXPERIMENT_GUI_CONNECTABLE_PARAMETER_H

