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

    File:        NetChannel.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 09 28

    Description: Class NetChannel contains additional data of a net-grabbing channel

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_NET_CHANNEL_H
#define CEDAR_DEV_SENSORS_VISUAL_NET_CHANNEL_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/namespace.h"
#include "cedar/devices/sensors/visual/GrabberChannel.h"
#include "cedar/auxiliaries/net/Reader.h"
#include "cedar/auxiliaries/StringParameter.h"

// SYSTEM INCLUDES


//!@brief NetChannel contains additional data of a net-grabbing channel
class cedar::dev::sensors::visual::NetChannel
:
public cedar::dev::sensors::visual::GrabberChannel
{
  //!@cond SKIPPED_DOCUMENTATION
  //friend class of NetGrabber for direct access to the members
  friend class cedar::dev::sensors::visual::NetGrabber;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:

  //! internally used type of the NetReader with cv::Mat as data
  typedef cedar::aux::net::Reader<cv::Mat> MatNetReader;
  typedef boost::shared_ptr<cedar::aux::net::Reader<cv::Mat> > MatNetReaderPtr;
  //!@endcond

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  NetChannel(const std::string& yarpChannelName = "grabberYarpChannel")
  :
  cedar::dev::sensors::visual::GrabberChannel(),
  _mpYarpChannelName(new cedar::aux::StringParameter(this, "yarpChannel", yarpChannelName)),
  mpMatNetReader(MatNetReaderPtr()) // empty MatNetReaderPtr
  {
  };

  //!@brief Destructor
  virtual ~NetChannel()
  {
  };

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

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
  //! @brief The name of the used yarp channel
  cedar::aux::StringParameterPtr _mpYarpChannelName;

  //! @brief The Yarp Reader class instantiated for cv::Mat
  MatNetReaderPtr mpMatNetReader;

private:
  // none yet

}; // class cedar::dev::sensors::visual::NetChannel

#endif // CEDAR_DEV_SENSORS_VISUAL_NET_CHANNEL_H

