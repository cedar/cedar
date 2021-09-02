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
    Date:        2021 08 05

    Description: Header file for the class cedar::proc::experiment::NeuralFieldBoostParameter.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_STEP_PARAMETER_H
#define CEDAR_PROC_EXPERIMENT_STEP_PARAMETER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/processing/Connectable.fwd.h"
#include "cedar/processing/DataRole.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/NeuralFieldBoostParameter.fwd.h"

// SYSTEM INCLUDES
#include <QObject>
#include <vector>
#include <string>
#include <cedar/auxiliaries/DoubleParameter.fwd.h>


/*!@brief A parameter to set a property of a certain step
 *
 *          The property could either be a parameter or an output or a buffer.
 *          The property type should be defined when creating an instance of this parameter.
 */
class cedar::proc::experiment::NeuralFieldBoostParameter : public cedar::aux::Parameter {
    Q_OBJECT

public:
    //!@brief The standard constructor.
    NeuralFieldBoostParameter(cedar::aux::Configurable *pOwner, const std::string &name);

    //!@brief Destructor
    virtual ~NeuralFieldBoostParameter();

    //!@brief set the step to a value, read from a configuration node
    void readFromNode(const cedar::aux::ConfigurationNode& node);

    //!@brief write value to a configuration node
    void writeToNode(cedar::aux::ConfigurationNode& root) const;

    //!@brief set to default
    void makeDefault();

    //!@brief Sets the step.
    void setStep(cedar::proc::ConnectablePtr step);

    //!@brief Returns the step name
    cedar::proc::ConnectablePtr getStep() const;

    //!@brief Sets the path of the element.
    void setElementPath(const std::string &step);

    //! Returns the path of the currently selected element.
    std::string getElementPath() const;

    //!@brief Sets the strength.
    void setStrength(double strength);

    //!@brief Returns the strength
    double getStrength() const;

    //! Returns true if a parameter is selected
    bool isParameterSelected() const;

    //! Checks the validity of the selected step.
    bool checkValidity(std::string& errors) const;

protected:


private:
    //!@brief The path of the field element.
    std::string mElementPath;

    //! Weak pointer to the field element.
    cedar::proc::ConnectableWeakPtr mElement;

    double mStrength;


}; // class cedar::proc::experiment::NeuralFieldBoostParameter

#endif // CEDAR_PROC_EXPERIMENT_STEP_PARAMETER_H

