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
#include "cedar/processing/sources/GrabberBase.h"
#include "cedar/devices/sensors/camera/Grabber.h"
#include "cedar/auxiliaries/FileParameter.h"
#include "cedar/auxiliaries/NumericParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/sources/Camera.fwd.h"

// SYSTEM INCLUDES


//!@brief A camera source for the processing framework.
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

  ///! @brief Action-method for applying the new parameter
  void applyParameter();

  ///! @brief gets a new frame from the grabber (if already created)
  void updateFrame();

public slots:

  ///! @brief invoke this slot, when the size of the grabbed frame have been changed
  void changedFrameSize();

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
  void onStop();

  //!@brief Cast the base GrabberBasePtr to derived class CameraGrabberPtr
  inline cedar::dev::sensors::camera::GrabberPtr getCameraGrabber()
  {
    return boost::static_pointer_cast<cedar::dev::sensors::camera::Grabber>
           (
             this->cedar::proc::sources::GrabberBase::mpGrabber
           );
  }

  //!@brief Cast the base GrabberBasePtr to derived class CameraGrabberPtr
  inline cedar::dev::sensors::camera::ConstGrabberPtr getCameraGrabber() const
  {
    return boost::static_pointer_cast<const cedar::dev::sensors::camera::Grabber>
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
