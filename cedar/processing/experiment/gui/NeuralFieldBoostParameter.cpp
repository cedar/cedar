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

    File:        NeuralFieldBoostParameter.cpp

    Maintainer:  Daniel Sabinasz
    Email:       daniel.sabinasz@ini.rub.de
    Date:        2021 09 02

    Description: Source file for the class cedar::proc::experiment::gui::NeuralFieldBoostParameter.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/gui/NeuralFieldBoostParameter.h"
#include "cedar/processing/experiment/NeuralFieldBoostParameter.h"
#include "cedar/processing/experiment/Supervisor.h"
#include "cedar/processing/auxiliaries/TypeBasedFactory.h"
#include "cedar/processing/auxiliaries/Singleton.h"
#include "cedar/processing/auxiliaries/gui/Parameter.h"
#include "cedar/dynamics/fields/NeuralField.h"

// SYSTEM INCLUDES
#include <QFormLayout>
#include <QLabel>
#include <string>
#include <vector>


//----------------------------------------------------------------------------------------------------------------------
// associate aux::gui parameter with the aux parameter
//----------------------------------------------------------------------------------------------------------------------
namespace {
    bool registered = cedar::proc::aux::gui::ParameterFactorySingleton::getInstance()->add
            <
                    cedar::proc::experiment::NeuralFieldBoostParameter,
                    cedar::proc::experiment::gui::NeuralFieldBoostParameter
            >();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::gui::NeuralFieldBoostParameter::NeuralFieldBoostParameter(QWidget *pParent)
        :
        cedar::proc::aux::gui::Parameter(pParent),
        mpElement(new QComboBox()),
        mpStrength(new QDoubleSpinBox()) {
    this->mpElement->setEditable(true);

    this->mpStrength->setMinimum(0);
    this->mpStrength->setMaximum(50);
    this->mpStrength->setSingleStep(0.1);

    auto layout = new QFormLayout();
    layout->setMargin(0);
    this->setLayout(layout);
    layout->addRow(new QLabel("Element"), mpElement);
    layout->addRow(new QLabel("Strength"), mpStrength);
    QObject::connect(this, SIGNAL(parameterPointerChanged()), this, SLOT(parameterPointerChanged()));
}

cedar::proc::experiment::gui::NeuralFieldBoostParameter::~NeuralFieldBoostParameter() {

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::proc::experiment::gui::NeuralFieldBoostParameter::parameterPointerChanged() {
    updateSteps();

    cedar::proc::experiment::NeuralFieldBoostParameterPtr neuralFieldBoostParameter;
    neuralFieldBoostParameter = boost::dynamic_pointer_cast<cedar::proc::experiment::NeuralFieldBoostParameter>(this->getParameter());
    if (neuralFieldBoostParameter) {
        auto path = neuralFieldBoostParameter->getElementPath();
        this->mpElement->setEditText(QString::fromStdString(path));
    }
    this->stepChanged();

    double strength = neuralFieldBoostParameter->getStrength();
    this->mpStrength->setValue(strength);
    this->strengthChanged();


    connect(this->mpElement, SIGNAL(editTextChanged(const QString&)), this, SLOT(stepChanged()));
    connect(this->mpStrength, SIGNAL(valueChanged(double)), this, SLOT(strengthChanged()));
}

void cedar::proc::experiment::gui::NeuralFieldBoostParameter::stepChanged() {
    auto parameter = boost::dynamic_pointer_cast<cedar::proc::experiment::NeuralFieldBoostParameter>(this->getParameter());
    parameter->setElementPath(this->mpElement->currentText().toStdString());
}

void cedar::proc::experiment::gui::NeuralFieldBoostParameter::strengthChanged() {
    auto parameter = boost::dynamic_pointer_cast<cedar::proc::experiment::NeuralFieldBoostParameter>(this->getParameter());
    parameter->setStrength(this->mpStrength->value());
}

void cedar::proc::experiment::gui::NeuralFieldBoostParameter::updateSteps() {
    mpElement->clear();

    std::vector<std::string> steps = SupervisorSingleton::getInstance()->getExperiment()->getGroupStepsMatching(
            [](cedar::proc::ConstElementPtr ptr) -> bool {
                const cedar::proc::Element *elementPtr = ptr.get();
                std::string name = typeid(*elementPtr).name();
                return name.find("NeuralField") != std::string::npos;
            });
    for (std::string step : steps) {
        mpElement->addItem(QString::fromStdString(step));
    }
}
