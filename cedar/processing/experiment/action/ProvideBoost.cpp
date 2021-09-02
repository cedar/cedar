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

    File:        ProvideBoost.cpp

    Maintainer:  Daniel Sabinasz
    Email:       daniel.sabinasz@ini.rub.de
    Date:        2021 08 04

    Description: Source file for the class cedar::proc::experiment::ProvideBoost.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/action/ProvideBoost.h"
#include "cedar/auxiliaries/ParameterDeclaration.h"
#include "cedar/processing/experiment/NeuralFieldBoostParameter.h"
#include "cedar/processing/experiment/Experiment.h"
#include "cedar/dynamics/fields/NeuralField.h"
#include "cedar/processing/experiment/Supervisor.h"

#include <iostream>

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register class
//----------------------------------------------------------------------------------------------------------------------

namespace {
    bool declare() {
        cedar::proc::experiment::action::ActionManagerSingleton::getInstance()->
                registerType<cedar::proc::experiment::action::ProvideBoostPtr>();
        cedar::proc::experiment::action::ActionManagerSingleton::getInstance()->
        addDeprecatedName<cedar::proc::experiment::action::ProvideBoostPtr>
                (
                        "cedar.proc.experiment.action.ProvideBoost"
                );
        return true;
    }

    bool declared = declare();
}
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::action::ProvideBoost::ProvideBoost()
        :
        _mNeuralFieldBoostParameter
        (
                new cedar::proc::experiment::NeuralFieldBoostParameter(this, "Neural field step")
        )
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::proc::experiment::action::ProvideBoost::checkValidity
        (
                std::vector<std::string> &errors,
                std::vector<std::string> & /* warnings */
        )
const {
    std::string parameter_error;
    bool parameter_valid = this->_mNeuralFieldBoostParameter->checkValidity(parameter_error);
    if (!parameter_valid) {
        errors.push_back("Cannot set parameter value: " + parameter_error);
        return false;
    }

    return true;
}

cedar::proc::experiment::NeuralFieldBoostParameterPtr cedar::proc::experiment::action::ProvideBoost::getNeuralFieldBoostParameter() const {
    return this->_mNeuralFieldBoostParameter;
}

void cedar::proc::experiment::action::ProvideBoost::run() {

    if (!_mNeuralFieldBoostParameter->isParameterSelected()) {
        return;
    }

    //boost::intrusive_ptr<cedar::dyn::NeuralField>
    auto neural_field = boost::dynamic_pointer_cast<cedar::dyn::NeuralField>(_mNeuralFieldBoostParameter->getStep());
    double strength = _mNeuralFieldBoostParameter->getStrength();
    int dimensionality = neural_field->getDimensionality();
    std::vector<int> sizes = neural_field->getSizes();
    if (dimensionality == 0) {
        neural_field->setInputSumExperimentData(cv::Mat(1, 1, CV_32F, cv::Scalar(strength)));
    } else if (dimensionality == 1) {
        neural_field->setInputSumExperimentData(cv::Mat(sizes[0], 1, CV_32F, cv::Scalar(strength)));
    } else {
        neural_field->setInputSumExperimentData(cv::Mat(dimensionality, &sizes.at(0), CV_32F, cv::Scalar(strength)));
    }
    cedar::proc::experiment::SupervisorSingleton::getInstance()->log("boost", "provide boost of strength " + std::to_string(strength));
}
//error: cannot convert ‘cedar::dyn::NeuralField*’ to ‘const cedar::aux::IntrusivePtrBase*’
void cedar::proc::experiment::action::ProvideBoost::preExperiment() {
    if (!_mNeuralFieldBoostParameter->isParameterSelected()) {
        return;
    }

}

void cedar::proc::experiment::action::ProvideBoost::postExperiment() {
    if (!_mNeuralFieldBoostParameter->isParameterSelected()) {
        return;
    }

    // Reset neural field experimental inputs
    auto neural_field = boost::dynamic_pointer_cast<cedar::dyn::NeuralField>(_mNeuralFieldBoostParameter->getStep());
    int dimensionality = neural_field->getDimensionality();
    std::vector<int> sizes = neural_field->getSizes();
    if (dimensionality == 0) {
        neural_field->setInputSumExperimentData(cv::Mat(1, 1, CV_32F, cv::Scalar(0)));
    } else if (dimensionality == 1) {
        neural_field->setInputSumExperimentData(cv::Mat(sizes[0], 1, CV_32F, cv::Scalar(0)));
    } else {
        neural_field->setInputSumExperimentData(cv::Mat(dimensionality, &sizes.at(0), CV_32F, cv::Scalar(0)));
    }
}