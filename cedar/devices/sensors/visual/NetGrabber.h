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

    File:        NetGrabber.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@rub.de
    Date:        2011 08 01

    Description: Header for the @em cedar::dev::sensors::visual::NetGrabber class.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_SENSORS_VISUAL_NET_GRABBER_H
#define CEDAR_DEV_SENSORS_VISUAL_NET_GRABBER_H


// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/GrabberInterface.h"
#include "cedar/auxiliaries/net/NetReader.h"

// SYSTEM INCLUDES


/*! \class cedar::dev::sensors::visual::NetGrabber
 *	\brief This grabber grabs images from a yarp-server located somewhere in the network
 *  \remarks This functionality is implemented by using the
 *		cedar::aux::net::NetWriter and
 *		cedar::aux::net::NetReader classes.
 */

class cedar::dev::sensors::visual::NetGrabber
:
public cedar::dev::sensors::visual::GrabberInterface
{

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //todo:typedef to NetReader
  typedef boost::shared_ptr<cedar::aux::net::NetReader<cv::Mat> > MatNetReaderPtr;


  /*! \struct PictureChannel
   *  \brief Additional data of a picture grabbing channel
   */
  struct NetChannel
  :
  cedar::dev::sensors::visual::GrabberInterface::GrabberChannel
  {
    std::string channelName;      ///! The name of the used yarp channel
    MatNetReaderPtr matNetReader; ///! The Yarp Reader class instantiated for cv::Mat
  };

  typedef boost::shared_ptr<NetChannel> NetChannelPtr;

//--------------------------------------------------------------------------------------------------------------------
//macros
//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
//constructors and destructor
//--------------------------------------------------------------------------------------------------------------------

public:

  /*! \brief  Constructor for a single channel grabber
   *  \param configFileName	Filename for the configuration
   *  \param YarpChannel	Channel to grab from
   */
  NetGrabber(
              const std::string& configFileName,
              const std::string& yarpChannelName
            );


  /*! \brief  Constructor for a stereo channel grabber
   *  \param configFileName	Filename for the configuration
   *  \param YarpChannel0	Name of a channel from the used yarp-server
   *  \param YarpChannel1	Another name of a channel from the used yarp-server
   */
  NetGrabber(
              const std::string& configFileName,
              const std::string& yarpChannelName0,
              const std::string& yarpChannelName1
            );

  //!@brief Destructor
  ~NetGrabber();

  //--------------------------------------------------------------------------------------------------------------------
  //public methods
  //--------------------------------------------------------------------------------------------------------------------

public:
  //none yet

  //--------------------------------------------------------------------------------------------------------------------
  //protected methods
  //--------------------------------------------------------------------------------------------------------------------

protected:

  //------------------------------------------------------------------------
  //From GrabberInterface
  //------------------------------------------------------------------------
  bool onInit();
  bool onGrab();
  bool onDeclareParameters();
  const std::string& onGetSourceInfo(
                               unsigned int channel
                             ) const;

  void onCleanUp();
  void onAddChannel();

  //--------------------------------------------------------------------------------------------------------------------
  //private methods
  //--------------------------------------------------------------------------------------------------------------------

private:
  //none yet

  //--------------------------------------------------------------------------------------------------------------------
  //members
  //--------------------------------------------------------------------------------------------------------------------

public:
  //none yet (hopefully never!)

protected:
  //none yet


private:

  ///! Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class NetChannelPtr
  inline NetChannelPtr getChannel(unsigned int channel)
  {
    //!@todo: change to asserted_cast
    //return cedar::aux::asserted_cast<NetChannelPtr>(mChannels.at(channel))
    return boost::static_pointer_cast<NetChannel>
           (
             cedar::dev::sensors::visual::GrabberInterface::mChannels.at(channel)
           );
  }

  //!@todo: after merging change to ConstCameraChannelPtr
  ///! Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class PictureChannelPtr
  inline boost::shared_ptr<const NetChannel> getChannel(unsigned int channel) const
  {
    //!@todo: change to asserted_cast
    //return cedar::aux::asserted_cast<NetChannelPtr>(mChannels.at(channel))
    return boost::static_pointer_cast<const NetChannel>
           (
             cedar::dev::sensors::visual::GrabberInterface::mChannels.at(channel)
           );
  }

  //--------------------------------------------------------------------------------------------------------------------
  //parameters
  //--------------------------------------------------------------------------------------------------------------------

public:
  //none yet (hopefully never!)

protected:
  //none yet

private:
  //none yet
  
}; //class cedar::dev::sensors::visual::NetGrabber

#endif
