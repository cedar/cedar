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

// MAKE YARP OPTIONAL
#include "cedar/configuration.h"
#ifdef CEDAR_USE_YARP

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/Grabber.h"
#include "cedar/auxiliaries/net/Reader.h"

// SYSTEM INCLUDES


#define SHOW_INIT_INFORMATION_NETGRABBER

/*! @class cedar::dev::sensors::visual::NetGrabber
 *	@brief This grabber grabs images from a yarp-server located somewhere in the network
 *
 *   This functionality is implemented by using the
 *		cedar::aux::net::NetWriter and
 *		cedar::aux::net::NetReader classes.
 */

class cedar::dev::sensors::visual::NetGrabber
:
public cedar::dev::sensors::visual::Grabber
{

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //!@cond SKIPPED_DOCUMENTATION

  //todo:typedef to NetReader
  typedef boost::shared_ptr<cedar::aux::net::Reader<cv::Mat> > MatNetReaderPtr;


  /*! @struct PictureChannel
   *  @brief Additional data of a net grabbing channel
   */
  struct NetChannel
  :
  cedar::dev::sensors::visual::Grabber::GrabberChannel
  {
    //! The name of the used yarp channel
    std::string mChannelName;

    //! The Yarp Reader class instantiated for cv::Mat
    MatNetReaderPtr mMatNetReader;
  };

  CEDAR_GENERATE_POINTER_TYPES(NetChannel);

  //!@endcond

//--------------------------------------------------------------------------------------------------------------------
//macros
//--------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------
//constructors and destructor
//--------------------------------------------------------------------------------------------------------------------

public:

  /*! @brief Constructor for a single channel grabber
   *  @param configFileName	Filename for the configuration
   *  @param yarpChannelName Channel to grab from
   */
  NetGrabber(const std::string& configFileName,const std::string& yarpChannelName);


  /*! @brief Constructor for a stereo channel grabber
   *  @param configFileName	Filename for the configuration
   *  @param yarpChannelName0	Name of a channel from the used yarp-server used for channel 0
   *  @param yarpChannelName1	Another name of a channel from the used yarp-server used for channel 1
   */
  NetGrabber(const std::string& configFileName,const std::string& yarpChannelName0,const std::string& yarpChannelName1);

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
  //From Grabber
  //------------------------------------------------------------------------
  bool onInit();
  bool onGrab();
  bool onDeclareParameters();
  void onUpdateSourceInfo(unsigned int channel);

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
    return boost::static_pointer_cast<NetChannel>
           (
             cedar::dev::sensors::visual::Grabber::mChannels.at(channel)
           );
  }

  ///! Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class PictureChannelPtr
  inline ConstNetChannelPtr getChannel(unsigned int channel) const
  {
    return boost::static_pointer_cast<const NetChannel>
           (
             cedar::dev::sensors::visual::Grabber::mChannels.at(channel)
           );
  }

  //!@brief The default name for the grabber
  virtual inline std::string defaultGrabberName() const
  {
    return "NetGrabber";
  }

  //--------------------------------------------------------------------------------------------------------------------
  //parameters
  //--------------------------------------------------------------------------------------------------------------------

protected:
  //none yet

private:
  //none yet
  
}; //class cedar::dev::sensors::visual::NetGrabber

#endif //CEDAR_USE_YARP
#endif //CEDAR_DEV_SENSORS_VISUAL_NET_GRABBER_H
