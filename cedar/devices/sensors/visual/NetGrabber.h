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


// LOCAL INCLUDES
#include "GrabberInterface.h"
#include "../../../auxiliaries/net/NetReader.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*! \class cedar::dev::sensors::visual::NetGrabber
 *	\brief This grabber grabs images from a yarp-server located somewhere in the network
 *  \remarks This functionality is implemented by using the
 *		cedar::aux::net::NetWriter and
 *		cedar::aux::net::NetReader classes.
 */

class cedar::dev::sensors::visual::NetGrabber
:
public GrabberInterface
{
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
              const std::string& YarpChannel
            );


  /*! \brief  Constructor for a stereo channel grabber
   *  \param configFileName	Filename for the configuration
   *  \param YarpChannel0	Name of a channel from the used yarp-server
   *  \param YarpChannel1	Another name of a channel from the used yarp-server
   */
  NetGrabber(
              const std::string& configFileName,
              const std::string& YarpChannel0,
              const std::string& YarpChannel1
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
  std::string onGetSourceInfo(
                               unsigned int channel
                             ) const;

  void onCleanUp();

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


  /*! \brief The names of the used yarp channels
   *
   */
  std::vector<std::string> mYarpChannels;

  /*! \brief This vector contains the needed Yarp Readers.
   *		One for every channel.
   *   \see
   *       mImageMatVector
   */
  std::vector<cedar::aux::net::NetReader<cv::Mat>*> mYarpReaderVector;

private:
  //none yet

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
