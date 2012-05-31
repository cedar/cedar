/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        InterfaceGrabber.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 04 23

    Description: Header for the @em @em cedar::dev::sensors::visual::InterfaceGrabber class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_INTERFACE_GRABBER_H
#define CEDAR_DEV_SENSORS_VISUAL_INTERFACE_GRABBER_H

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/Grabber.h"
#include "cedar/devices/sensors/visual/Grabbable.h"


// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>

/*! @brief A simple Grabber class for testing the Grabber interface
 *
 *  This grabber class is used to test the grabber interface. It
 *  creates a Grabber with a TestParam (default-value 123) and FPS set to 15
 *
 *  @remarks For grabber developers<br>
 *    This class can also be used as a template to create other classes derived from GrabberInstance
 *
 */
class cedar::dev::sensors::visual::InterfaceGrabber
:
public cedar::dev::sensors::visual::Grabber
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //!@cond SKIPPED_DOCUMENTATION

  /*! @struct TestChannel
   *  @brief Additional data of a grabbing channel
   *  @remarks For grabber developers<br>
   *    You don't have to create an extended channel structure, until you need more channel data.
   *    But when, then you have to implement the onAddChannel() member function as well
   */
  struct InterfaceChannel
  :
  cedar::dev::sensors::visual::Grabber::GrabberChannel
  {
    //! @brief The class to grab from
    cedar::dev::sensors::visual::GrabbableInterface* mpSourceInterfaceClass;
  };

  CEDAR_GENERATE_POINTER_TYPES(InterfaceChannel);

  //!@endcond

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! @brief The constructor for a single channel grabber.
   *  @param configFileName Filename for a file, where the configuration parameters should be stored
   *  @param sourceInterfaceClass Class to grab from. Have to be a deriviative of the
   *  cedar::dev::sensors::visual::GrabbableInterface class
   */
  InterfaceGrabber
  (
    std::string configFileName,
    cedar::dev::sensors::visual::GrabbableInterface* sourceInterfaceClass
  );

  /*! @brief The constructor for a stereo grabber.
   *  @param configFileName Filename for a file, where the configuration parameters should be stored
   *  @param sourceInterfaceClass0 Class to grab from. Have to be a deriviative of the
   *            cedar::dev::sensors::visual::GrabbableInterface class
   *  @param sourceInterfaceClass1 Class to grab from. Have to be a deriviative of the
   *            cedar::dev::sensors::visual::GrabbableInterface class   */
  InterfaceGrabber
  (
    std::string configFileName,
    cedar::dev::sensors::visual::GrabbableInterface* sourceInterfaceClass0,
    cedar::dev::sensors::visual::GrabbableInterface* sourceInterfaceClass1
  );

  //!@brief Destructor
  ~InterfaceGrabber();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //derived from Grabber
  bool onInit();
  void onCleanUp();
  bool onDeclareParameters();
  void onUpdateSourceInfo(unsigned int channel);
  bool onGrab();
  void onAddChannel();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  ///! Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class InterfaceChannelPtr
  inline InterfaceChannelPtr getChannel(unsigned int channel)
  {
    return boost::static_pointer_cast<InterfaceChannel>
           (
             cedar::dev::sensors::visual::Grabber::mChannels.at(channel)
           );
  }

  ///! Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class InterfaceChannellPtr
  inline ConstInterfaceChannelPtr getChannel(unsigned int channel) const
  {
    return boost::static_pointer_cast<const InterfaceChannel>
       (
         cedar::dev::sensors::visual::Grabber::mChannels.at(channel)
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
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::sensors::visual::InterfaceGrabber

#endif //CEDAR_DEV_SENSORS_VISUAL_INTERFACE_GRABBER_H
