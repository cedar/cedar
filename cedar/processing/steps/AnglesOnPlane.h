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

    File:        AnglesOnPlane.cpp

    Maintainer:  Nico Kuerschner
    Email:       Nico.Kuerschner@ini.ruhr-uni-bochum.de
    Date:        2017 January 25th

    Description: Takes two vectors and computes the angle, the plane normal vector and the scaled vector of most directional influence

    Credits:

======================================================================================================================*/

#ifndef ROTATION_ON_PLANE_H_
#define ROTATION_ON_PLANE_H_

#include <cedar/processing/Step.h>
#include <cedar/auxiliaries/MatData.h>
#include <cedar/auxiliaries/DoubleParameter.h>
#include <cedar/auxiliaries/DoubleVectorParameter.h>
#include "cedar/processing/steps/AnglesOnPlane.fwd.h"

class cedar::proc::steps::AnglesOnPlane : public cedar::proc::Step
{
  Q_OBJECT
  public:
    AnglesOnPlane();
    cedar::proc::DataSlot::VALIDITY determineInputValidity
                                    (
                                      cedar::proc::ConstDataSlotPtr slot,
                                      cedar::aux::ConstDataPtr data
                                    ) const;

  private:
    void compute(const cedar::proc::Arguments&);
    void inputConnectionChanged(const std::string& inputName);

    // input
    cedar::aux::ConstMatDataPtr mVector1;
    cedar::aux::ConstMatDataPtr mVector2;

    // output
    cedar::aux::MatDataPtr mAngle;
    cedar::aux::MatDataPtr mOrthogonalVector1;

    // parameters
    //cedar::aux::DoubleParameterPtr mUndefindeAngleReplacement;
    cedar::aux::DoubleVectorParameterPtr mUndefinedOrthogonalVector1Replacement;
};

#endif /* ROTATION_ON_PLANE_H_ */
