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

    File:        Camera.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.ruhr-uni-bochum.d
    Date:        2012 04 20

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_SOURCES_CAMERA_H
#define CEDAR_PROC_SOURCES_CAMERA_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/sources/namespace.h"
#include "cedar/processing/sources/GrabberBase.h"
#include "cedar/devices/sensors/visual/CameraGrabber.h"

#include "cedar/auxiliaries/FileParameter.h"
#include "cedar/auxiliaries/NumericParameter.h"

// SYSTEM INCLUDES


//!@brief A camera source for the processingIde
class cedar::proc::sources::Camera
:
public cedar::proc::sources::GrabberBase
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Camera();

  //!@brief Destructor
  ~Camera();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

//public slots:
//
//  //!@brief Set the debayer function on or off
//  void setDeBayer();
//
//  /*!@brief Set the busId
//   *
//   * If this value is set on grabbing, the cameragrabber will be destroyed and with the new busId recreated
//   */
//  void setBusId();

  void applyParameter();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void compute(const cedar::proc::Arguments&);
  void onStart();
  //void onCreateGrabber();

  //!@brief Cast the base GrabberBasePtr to derived class CameraGrabberPtr
  inline cedar::dev::sensors::visual::CameraGrabberPtr getCameraGrabber()
  {
    return boost::static_pointer_cast<cedar::dev::sensors::visual::CameraGrabber>
           (
             this->cedar::proc::sources::GrabberBase::mpGrabber
           );
  }

  //!@brief Cast the base GrabberBasePtr to derived class CameraGrabberPtr
  inline cedar::dev::sensors::visual::ConstCameraGrabberPtr getCameraGrabber() const
  {
    return boost::static_pointer_cast<const cedar::dev::sensors::visual::CameraGrabber>
           (
            cedar::proc::sources::GrabberBase::mpGrabber
           );
  }

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
  //!@ Bayer conversion from the camera image
  cedar::aux::BoolParameterPtr mDeBayer;

  //!@ busid
  cedar::aux::UIntParameterPtr mBusId;

}; // class cedar::proc::sources::Camera

#endif // CEDAR_PROC_SOURCES_CAMERA_H
