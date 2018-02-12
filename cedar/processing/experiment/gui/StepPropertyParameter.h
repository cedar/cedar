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

    File:        StepPropertyParameter.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 03 13

    Description: Header file for the class cedar::proc::experiment::gui::StepPropertyParameter.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_GUI_STEP_PROPERTY_PARAMETER_H
#define CEDAR_PROC_EXPERIMENT_GUI_STEP_PROPERTY_PARAMETER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/Parameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/gui/StepPropertyParameter.fwd.h"

// SYSTEM INCLUDES
#include <QComboBox>
#include <QBoxLayout>

/*!@brief GUI implementation of cedar::proc::experiment::StepPropertyParameter
 *
 */
class cedar::proc::experiment::gui::StepPropertyParameter : public cedar::aux::gui::Parameter
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
  StepPropertyParameter(QWidget *pParent = NULL);

  //!@brief Destructor
  virtual ~StepPropertyParameter();



  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

public slots:
  //!@brief handles a change of the associated parameter
  void parameterPointerChanged();
  //!@brief handles a change in the step combo box
  void stepChanged();
  //!@brief handles a change in the property combo box
  void propertyChanged();

//  void valueChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Updates the steps for the combo box
  void updateSteps();
  //!@brief Updates the properties the combo box
  void updateProperties();

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
  QComboBox* mpElement;

  //!@brief The property selector
  QComboBox* mpProperty;

  //!@brief The parameter widget. Only appears if the StepPropertyParameter is of type PARAMETER
  QWidget* mpPropertyCopy;

}; // class cedar::proc::experiment::gui::StepPropertyParameter

#endif // CEDAR_PROC_EXPERIMENT_GUI_STEP_PROPERTY_PARAMETER_H

