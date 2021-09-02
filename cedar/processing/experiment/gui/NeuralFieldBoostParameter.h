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

    File:        NeuralFieldBoostParameter.h

    Maintainer:  Daniel Sabinasz
    Email:       daniel.sabinasz@ini.rub.de
    Date:        2021 09 02

    Description: Header file for the class cedar::proc::experiment::gui::NeuralFieldBoostParameter.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_GUI_STEP_PARAMETER_H
#define CEDAR_PROC_EXPERIMENT_GUI_STEP_PARAMETER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/auxiliaries/gui/Parameter.h"
#include "cedar/dynamics/fields/NeuralField.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/gui/NeuralFieldBoostParameter.fwd.h"
#include "cedar/dynamics/fields/NeuralField.fwd.h"

// SYSTEM INCLUDES
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QBoxLayout>

/*!@brief GUI implementation of cedar::proc::experiment::NeuralFieldBoostParameter
 *
 */
class cedar::proc::experiment::gui::NeuralFieldBoostParameter : public cedar::proc::aux::gui::Parameter {
    //--------------------------------------------------------------------------------------------------------------------
    // macros
    //--------------------------------------------------------------------------------------------------------------------
Q_OBJECT

    //--------------------------------------------------------------------------------------------------------------------
    // constructors and destructor
    //--------------------------------------------------------------------------------------------------------------------
public:
    //!@brief The standard constructor.
    NeuralFieldBoostParameter(QWidget *pParent = NULL);

    //!@brief Destructor
    virtual ~NeuralFieldBoostParameter();



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

    //!@brief handles a change in the boost strength
    void strengthChanged();

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
    QComboBox *mpElement;

    //!@brief The strength text edit
    QDoubleSpinBox *mpStrength;

}; // class cedar::proc::experiment::gui::NeuralFieldBoostParameter

#endif // CEDAR_PROC_EXPERIMENT_GUI_STEP_PARAMETER_H

