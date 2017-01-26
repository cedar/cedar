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

    File:        NetGrabber.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@rub.de
    Date:        2011 08 01

    Description: Header for the @em cedar::dev::sensors::visual::NetGrabber class.

    Credits:

======================================================================================================================*/
#ifndef CEDAR_DEV_SENSORS_VISUAL_NET_GRABBER_H
#define CEDAR_DEV_SENSORS_VISUAL_NET_GRABBER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"
#ifdef CEDAR_USE_YARP

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/Grabber.h"
#include "cedar/devices/sensors/visual/NetChannel.h"
#include "cedar/auxiliaries/net/Reader.h"

// SYSTEM INCLUDES
#include <string>

/*! @class cedar::dev::sensors::visual::NetGrabber
 *  @brief This grabber grabs images from a yarp-server located somewhere in the network
 *
 *   This functionality is implemented by using the
 *    cedar::aux::net::NetWriter and
 *    cedar::aux::net::NetReader classes.
 */

class cedar::dev::sensors::visual::NetGrabber
:
public cedar::dev::sensors::visual::Grabber
{

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------
// macros
//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//--------------------------------------------------------------------------------------------------------------------

public:

  /*! @brief  Constructor for a single channel grabber
   *  @param yarpChannelName  Filename to grab from
   */
  NetGrabber
  (
    const std::string& yarpChannelName = "grabberYarpChannel"
  );

  /*! @brief Constructor for a stereo channel grabber
   *  @param grabberName  Name of the grabber
   *  @param yarpChannelName0 Filename to grab from for channel 0
   *  @param yarpChannelName1 Filename to grab from for channel 1
   */
  NetGrabber
  (
    const std::string& yarpChannelName0,
    const std::string& yarpChannelName1
  );

  //!@brief Destructor
  ~NetGrabber();

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
  void onCreateGrabber();
  void onGrab(unsigned int channel);
  std::string onGetSourceInfo(unsigned int channel);
  void onCleanUp();
  void onCloseGrabber();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------

private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------

public:
  // none yet (hopefully never!)

protected:
  // none yet

private:

  //! Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class NetChannelPtr
  inline NetChannelPtr getNetChannel(unsigned int channel)
  {
    return boost::static_pointer_cast<NetChannel>
           (
             cedar::dev::sensors::visual::Grabber::_mChannels->at(channel)
           );
  }

  //! Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class PictureChannelPtr
  inline ConstNetChannelPtr getNetChannel(unsigned int channel) const
  {
    return boost::static_pointer_cast<const NetChannel>
           (
             cedar::dev::sensors::visual::Grabber::_mChannels->at(channel)
           );
  }


  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------

protected:
  // none yet

private:
  // none yet
  
}; // class cedar::dev::sensors::visual::NetGrabber

#endif // CEDAR_USE_YARP
#endif // CEDAR_DEV_SENSORS_VISUAL_NET_GRABBER_H
