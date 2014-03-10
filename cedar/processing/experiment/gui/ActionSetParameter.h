/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ActionSetParameter.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 03 07

    Description: Header file for the class cedar::proc::experiment::gui::ActionSetParameter.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_GUI_ACTION_SET_PARAMETER_H
#define CEDAR_PROC_EXPERIMENT_GUI_ACTION_SET_PARAMETER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/gui/Action.h"
#include "cedar/auxiliaries/gui/Parameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/gui/ActionSetParameter.fwd.h"

// SYSTEM INCLUDES

#include <QComboBox>
#include <QDoubleSpinBox>


/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::experiment::gui::ActionSetParameter  : public cedar::proc::experiment::gui::Action
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ActionSetParameter();

  //!@brief Destructor
  virtual ~ActionSetParameter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public slots:
  void updateParameters();
  void updateSteps();
  void parameterSelected();
  void stepSelected();
  void updateDesiredValue();
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void redraw();

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
  QComboBox* mStepSelector;
  QComboBox* mParameterSelector;
  cedar::aux::gui::Parameter* mDesiredValue;

}; // class cedar::proc::experiment::gui::ActionSetParameter

#endif // CEDAR_PROC_EXPERIMENT_GUI_ACTION_SET_PARAMETER_H

