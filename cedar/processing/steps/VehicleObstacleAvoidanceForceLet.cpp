/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        VehicleObstacleAvoidanceForceLet.cpp

    Maintainer:  Daniel Sabinasz
    Email:       daniel.sabinasz@ini.rub.de
    Date:        2019 08 15

    Description: Implements a force-let function

    Credits:

======================================================================================================================*/
// LOCAL INCLUDES
#include "VehicleObstacleAvoidanceForceLet.h"
#include "cedar/processing/ElementDeclaration.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace {
    bool declare() {
        using cedar::proc::ElementDeclarationPtr;
        using cedar::proc::ElementDeclarationTemplate;

        ElementDeclarationPtr declaration
                (
                        new ElementDeclarationTemplate<cedar::proc::steps::VehicleObstacleAvoidanceForceLet>
                                (
                                        "Robotics",
                                        "cedar.processing.steps.VehicleObstacleAvoidanceForceLet"
                                )
                );
        declaration->setIconPath(":/steps/vehicle_obstacle_avoidance_forcelet.svg");
        declaration->setDescription
                (
                        "Implements an obstacle avoidance force-let for the heading direction of a vehicle on a 2D surface. Formula: lambda * (phi - psi_obs) * exp(-0.5 (phi - psi_obs)^2 / sigma^2). lambda = beta_1 * exp(-d_i / beta_2). sigma = arctan(tan(Delta_theta) + R / (R + d)). For details see 'Bicho, E., Mallet, P., and Schöner, G. (2000). Target representation on an autonomous vehicle with low-level sensors. International Journal of Robotics Research, 19(5):424–447'.\n"
                );

        declaration->declare();

        return true;
    }

    bool declared = declare();
}


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::VehicleObstacleAvoidanceForceLet::VehicleObstacleAvoidanceForceLet()
        :
        cedar::proc::Step(false),
        mResult(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
        mRadius(new cedar::aux::DoubleParameter(this, "robot radius (R)", 0.0265)),
        mSensorRange(new cedar::aux::DoubleParameter(this, "sensor range (Delta_theta)", 0.7854)),
        mBeta1(new cedar::aux::DoubleParameter(this, "maximal strength (beta_1)", 1.0)),
        mBeta2(new cedar::aux::DoubleParameter(this, "distance scale (beta_2)", 1.0)) {
    this->declareInput("input");
    this->declareInput("obstacle angle (psi)");
    this->declareInput("obstacle distance (d)");
    this->declareInput("maximal strength (beta_1; optional)", false); // optional
    this->declareInput("distance scale (beta_2; optional)", false); // optional

    this->declareOutput("result", mResult);

    QObject::connect(mRadius.get(), SIGNAL(valueChanged()), this, SLOT(constantChanged()));
    QObject::connect(mSensorRange.get(), SIGNAL(valueChanged()), this, SLOT(constantChanged()));
    QObject::connect(mBeta1.get(), SIGNAL(valueChanged()), this, SLOT(constantChanged()));
    QObject::connect(mBeta2.get(), SIGNAL(valueChanged()), this, SLOT(constantChanged()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::VehicleObstacleAvoidanceForceLet::recompute() {
    auto phiInput = getInput("input");
    if (!phiInput)
        return;
    auto phiInputData = boost::dynamic_pointer_cast<const cedar::aux::MatData>(phiInput);
    if (!phiInputData)
        return;
    const float &phi = phiInputData->getData().at<float>(0, 0);

    auto psiInput = getInput("obstacle angle (psi)");
    if (!psiInput)
        return;
    auto psiInputData = boost::dynamic_pointer_cast<const cedar::aux::MatData>(psiInput);
    if (!psiInputData)
        return;
    const float &psi = psiInputData->getData().at<float>(0, 0);

    auto dInput = getInput("obstacle distance (d)");
    if (!dInput)
        return;
    auto dInputData = boost::dynamic_pointer_cast<const cedar::aux::MatData>(dInput);
    if (!dInputData)
        return;
    const float &d = dInputData->getData().at<float>(0, 0);

    float beta1;
    bool has_beta1_input = false;
    auto beta1input = getInput("maximal strength (beta 1; optional)");
    if (beta1input) {
        auto beta1data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(beta1input);
        if (beta1data
            && !beta1data->getData().empty()) {
            has_beta1_input = true;
            beta1 = beta1data->getData().at<float>(0, 0);
        }
    }
    if (!has_beta1_input) {
        beta1 = mBeta1->getValue();
    }

    float beta2;
    bool has_beta2_input = false;
    auto beta2input = getInput("distance scale (beta 2; optional)");
    if (beta2input) {
        auto beta2data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(beta2input);
        if (beta2data
            && !beta2data->getData().empty()) {
            has_beta2_input = true;
            beta2 = beta2data->getData().at<float>(0, 0);
        }
    }
    if (!has_beta2_input) {
        beta2 = mBeta2->getValue();
    }

    float sensorRange = mSensorRange->getValue();
    float radius = mRadius->getValue();

    float sigma = atan(tan(sensorRange/2) + radius / (radius + d));
    float lambda = beta1 * exp(-d/beta2);
    float out = lambda * (phi - psi) * exp(
            -pow(phi-psi, 2)
            / (2*pow(sigma, 2))
            );

    out += lambda * (phi - psi - 2*M_PI) * exp(
            -pow(phi-psi-2*M_PI, 2)
            / (2*pow(sigma, 2))
    );
    out += lambda * (phi - psi + 2*M_PI) * exp(
            -pow(phi-psi + 2*M_PI, 2)
            / (2*pow(sigma, 2))
    );

    mResult->getData().at<float>(0, 0) = out;

    checkOptionalInputs();
    // need to call this here, because determineinputValidity does not
    // get called for optional inputs that get removed by the user.
}

void cedar::proc::steps::VehicleObstacleAvoidanceForceLet::compute(const cedar::proc::Arguments &) {
    recompute();
}

void cedar::proc::steps::VehicleObstacleAvoidanceForceLet::checkOptionalInputs() {

    auto beta1Input = getInput("maximal strength (beta 1; optional)");
    bool has_beta1_input = false;
    if (beta1Input) {
        auto beta1Data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(beta1Input);
        if (beta1Data
            && !beta1Data->getData().empty()) {
            has_beta1_input = true;
        }
    }
    if (has_beta1_input) {
        // ausgrauen des Parameters:
        mBeta1->setConstant(true);
    } else {
        // Parameter aktivieren:
        mBeta1->setConstant(false);
    }


    auto beta2Input = getInput("distance scale (beta 2; optional)");
    bool has_beta2_input = false;
    if (beta2Input) {
        auto beta2Data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(beta2Input);
        if (beta2Data
            && !beta2Data->getData().empty()) {
            has_beta2_input = true;
        }
    }
    if (has_beta2_input) {
        // ausgrauen des Parameters:
        mBeta2->setConstant(true);
    } else {
        // Parameter aktivieren:
        mBeta2->setConstant(false);
    }

}

//// validity check
/*cedar::proc::DataSlot::VALIDITY cedar::proc::steps::VehicleObstacleAvoidanceForceLet::determineInputValidity
        (
                cedar::proc::ConstDataSlotPtr slot,
                cedar::aux::ConstDataPtr data
        ) const {
    //all inputs have same type
    cedar::aux::ConstMatDataPtr _input = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
    if (slot->getName() == "input") {
        if (_input && _input->getDimensionality() == 0 && _input->getData().type() == CV_32F) {
            return cedar::proc::DataSlot::VALIDITY_VALID;
        }
    } else if (slot->getName() == "range (sigma; optional)") {
        if (_input && _input->getDimensionality() == 0 && _input->getData().type() == CV_32F) {
            return cedar::proc::DataSlot::VALIDITY_VALID;
        }
    } else if (slot->getName() == "maximal strength (beta 1; optional)") {
        if (_input && _input->getDimensionality() == 0 && _input->getData().type() == CV_32F) {
            return cedar::proc::DataSlot::VALIDITY_VALID;
        }
    } else if (slot->getName() == "distance scale (beta 2; optional)") {
        if (_input && _input->getDimensionality() == 0 && _input->getData().type() == CV_32F) {
            return cedar::proc::DataSlot::VALIDITY_VALID;
        }
    }

    // else
    return cedar::proc::DataSlot::VALIDITY_ERROR;
}*/

void cedar::proc::steps::VehicleObstacleAvoidanceForceLet::inputConnectionChanged(const std::string &) {
    recompute();
}

void cedar::proc::steps::VehicleObstacleAvoidanceForceLet::constantChanged() {
    recompute();
}

