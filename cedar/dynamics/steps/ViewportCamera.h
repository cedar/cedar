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

    File:        ViewportCamera.h

    Maintainer:  Lars Janssen, Raul Grieben
    Email:       lars.janssen@ini.rub.de, raul.grieben@ini.rub.de
    Date:        2023 02 26

    Description: Header file for the class cedar::dyn::steps::ViewportCamera.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DYN_STEPS_VIEWPORT_CAMERA_H
#define CEDAR_DYN_STEPS_VIEWPORT_CAMERA_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/dynamics/Dynamics.h"
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/auxiliaries/UIntVectorParameter.fwd.h"
#include "cedar/auxiliaries/BoolParameter.h"

// FORWARD DECLARATIONS
#include "cedar/dynamics/steps/ViewportCamera.fwd.h"

// SYSTEM INCLUDES


/*! Viewport Camera
 *
 * Crops a given input image to a viewpoint defined by the activation of a second input
 */
class cedar::dyn::steps::ViewportCamera : public cedar::dyn::Dynamics
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ViewportCamera();

  //!@brief Destructor
  virtual ~ViewportCamera();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Updates the output matrix.
  void eulerStep(const cedar::unit::Time& time);
  void reset();

public slots:
  //!@brief Slot that reacts to changes in the viewport size.

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void getCenterPoint(double &centerX, double &centerY, double &max);

  //!@brief Reacts to a change in the input connection.
  void inputConnectionChanged(const std::string& inputName);


  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief MatrixData representing the input. Storing it like this saves time during computation.
  cedar::aux::ConstMatDataPtr mInput;

  //!@brief MatrixData representing the input. Storing it like this saves time during computation.
  cedar::aux::ConstMatDataPtr mViewportCenterInput;

  //!@brief The data containing the output.
  cedar::aux::MatDataPtr mOutput;
  cedar::aux::MatDataPtr mOutputCOS;
  cedar::aux::MatDataPtr mOutputKernel;

private:
    float _mElapsedTime;
    double _lastX;
    double _lastY;
    bool _startSC;
    bool _endSC;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief The size of the viewport
  cedar::aux::UIntVectorParameterPtr mViewportSize;
  cedar::aux::BoolParameterPtr _msimplified;

private:
  // none yet

}; // class cedar::dyn::steps::ViewportCamera

#endif // CEDAR_DYN_STEPS_VIEWPORT_CAMERA_H

