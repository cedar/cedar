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

    File:        GrabbableGrabber.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 04 23

    Description: Header for the @em @em cedar::dev::sensors::visual::GrabbableGrabber class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_GRABBABLE_GRABBER_H
#define CEDAR_DEV_SENSORS_VISUAL_GRABBABLE_GRABBER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/Grabber.h"
#include "cedar/devices/sensors/visual/GrabbableChannel.h"
#include "cedar/auxiliaries/Grabbable.h"


// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <string>


/*! @brief A simple Grabber class for testing the Grabber interface
 *
 *  This grabber class is used to test the grabber interface. It
 *  creates a Grabber with a TestParam (default-value 123) and FPS set to 15
 *
 *  @remarks For grabber developers<br>
 *    This class can also be used as a template to create other classes derived from GrabberInstance
 *
 */
class cedar::dev::sensors::visual::GrabbableGrabber
:
public cedar::dev::sensors::visual::Grabber
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! @brief  Constructor for a single channel grabber
   *  @param grabberName  Name of the grabber
   *  @param grabbableObject Object to grab from (have to implement the Grabbable-Interface)
   */
  GrabbableGrabber
  (
    cedar::aux::Grabbable* grabbableObject = NULL,
    const std::string& grabberName = "GrabbableGrabber"
  );

  /*! @brief Constructor for a stereo channel grabber
   *  @param grabberName  Name of the grabber
   *  @param grabbableObject0 Object to grab from (have to implement the Grabbable-Interface) for channel 0
   *  @param grabbableObject1 Object to grab from (have to implement the Grabbable-Interface) for channel 1
   */
  GrabbableGrabber
  (
    cedar::aux::Grabbable* grabbableObject0,
    cedar::aux::Grabbable* grabbableObject1,
    const std::string& grabberName = "StereoGrabbableGrabber"
  );

  //!@brief Destructor
  ~GrabbableGrabber();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  // inherited from Grabber
  void onCleanUp();
  void onGrab(unsigned int channel);
  void onCreateGrabber();
  void onCloseGrabber();
  std::string onGetSourceInfo(unsigned int channel);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  //! Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class InterfaceChannelPtr
  inline GrabbableChannelPtr getGrabbableChannel(unsigned int channel)
  {
    return boost::static_pointer_cast<GrabbableChannel>
           (
             cedar::dev::sensors::visual::Grabber::_mChannels->at(channel)
           );
  }

  //! Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class InterfaceChannellPtr
  inline ConstGrabbableChannelPtr getGrabbableChannel(unsigned int channel) const
  {
    return boost::static_pointer_cast<const GrabbableChannel>
       (
         cedar::dev::sensors::visual::Grabber::_mChannels->at(channel)
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
  // none yet

}; // class cedar::dev::sensors::visual::GrabbableGrabber

#endif // CEDAR_DEV_SENSORS_VISUAL_GRABBABLE_GRABBER_H
