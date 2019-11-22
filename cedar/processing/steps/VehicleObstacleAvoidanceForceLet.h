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

    File:        VehicleObstacleAvoidanceForceLet.h

    Maintainer:  Daniel Sabinasz
    Email:       daniel.sabinasz@ini.rub.de
    Date:        2019 08 15

    Description: 

    Credits:

======================================================================================================================*/

#ifndef VEHICLE_OBSTACLE_AVOIDANCE_FORCELET_H_
#define VEHICLE_OBSTACLE_AVOIDANCE_FORCELET_H_

#include <cedar/processing/Step.h> // if we are going to inherit from cedar::proc::Step, we have to include the header
#include <cedar/auxiliaries/MatData.h>
#include <cedar/auxiliaries/DoubleParameter.h>
#include <cedar/auxiliaries/BoolParameter.h>
#include "cedar/processing/steps/VehicleObstacleAvoidanceForceLet.fwd.h"

class cedar::proc::steps::VehicleObstacleAvoidanceForceLet : public cedar::proc::Step
{
    Q_OBJECT

public slots:
  void constantChanged();

  public:
    VehicleObstacleAvoidanceForceLet();
    /*cedar::proc::DataSlot::VALIDITY determineInputValidity
                                    (
                                      cedar::proc::ConstDataSlotPtr slot,
                                      cedar::aux::ConstDataPtr data
                                    ) const;*/

  private:
    void recompute();
    void compute(const cedar::proc::Arguments&);
    void inputConnectionChanged(const std::string& inputName);
    void checkOptionalInputs();

  protected:
    // output
    cedar::aux::MatDataPtr mResult;

    // params
    cedar::aux::DoubleParameterPtr mRadius;
    cedar::aux::DoubleParameterPtr mSensorRange;
    cedar::aux::DoubleParameterPtr mBeta1;
    cedar::aux::DoubleParameterPtr mBeta2;

};

#endif /* VEHICLE_OBSTACLE_AVOIDANCE_FORCELET_H_ */
