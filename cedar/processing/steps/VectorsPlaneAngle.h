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

    File:        VectorsPlaneAngle.cpp

    Maintainer:  Mathäus Muzalewski
    Email:       mathaeus.muzalewski@ruhr-uni-bochum.de
    Date:        2014 August 28th

    Description: Takes two vectors and computes the angle, the plane normal vector and the scaled vector of most directional influence 

    Credits:

======================================================================================================================*/

#ifndef VECTORS_PLANE_ANGLE_H_
#define VECTORS_PLANE_ANGLE_H_

#include <cedar/processing/Step.h>
#include <cedar/auxiliaries/MatData.h>

// includes for visualisation
#include "cedar/auxiliaries/gl/GlobalScene.h"
#include "cedar/auxiliaries/gl/Sphere.h"

#include "cedar/processing/steps/VectorsPlaneAngle.fwd.h"

class cedar::proc::steps::VectorsPlaneAngle : public cedar::proc::Step
{
  Q_OBJECT
  public:
    VectorsPlaneAngle();
    cedar::proc::DataSlot::VALIDITY determineInputValidity
                                    (
                                      cedar::proc::ConstDataSlotPtr slot,
                                      cedar::aux::ConstDataPtr data
                                    ) const;

  private:
    void compute(const cedar::proc::Arguments&);
    void inputConnectionChanged(const std::string& inputName);

    // input
    cedar::aux::ConstMatDataPtr mpEndeffectorVelocity;
    cedar::aux::ConstMatDataPtr mpEndeffectorPosition;
    cedar::aux::ConstMatDataPtr mpTargetPosition;

    // output
    cedar::aux::MatDataPtr mpAngle;
    cedar::aux::MatDataPtr mpOrthogonalAcceleration;   

  private slots:
    void visualisationChanged();

  protected:
    //!@brief Visualise the target position in scene or not
    cedar::aux::BoolParameterPtr _mVisualiseTarget;

    // corresponding visualisation id to perform deletion
    int _mVisualisationID;

    cedar::aux::gl::ObjectVisualizationPtr mVisualisationPtr;
};

#endif /* VECTORS_PLANE_ANGLE_H_ */
