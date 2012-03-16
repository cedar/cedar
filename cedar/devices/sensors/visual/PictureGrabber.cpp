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

    File:        PictureGrabber.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 01 08

    Description: Implementation of the @em cedar::dev::sensors::visual::PictureGrabber class..

    Credits:

======================================================================================================================*/


// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/PictureGrabber.h"
#include "cedar/auxiliaries/exceptions/IndexOutOfRangeException.h"

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
//constructors and destructor
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//Constructor for single-file grabber
cedar::dev::sensors::visual::PictureGrabber::PictureGrabber(
                                const std::string& configFileName,
                                const std::string& pictureFileName
                              )
:
cedar::dev::sensors::visual::GrabberInterface(configFileName)
{
  readInit(1,"PictureGrabber");

  //change/overwrite parameters with constructor values
  getChannel(0)->sourceFileName = pictureFileName;

  applyInit();
}

//----------------------------------------------------------------------------------------------------
//Constructor for stereo-file grabber
cedar::dev::sensors::visual::PictureGrabber::PictureGrabber(
                                const std::string& configFileName,
                                const std::string& pictureFileName0,
                                const std::string& pictureFileName1
                              )
:
cedar::dev::sensors::visual::GrabberInterface(configFileName)
{
  readInit(2,"PictureGrabber");

  getChannel(0)->sourceFileName = pictureFileName0;
  getChannel(1)->sourceFileName = pictureFileName1;

  applyInit();
}

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::PictureGrabber::~PictureGrabber()
{
  doCleanUp();
  #ifdef DEBUG_PICTUREGRABBER
    std::cout<<"[PictureGrabber::Destructor]"<< std::endl;
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
//methods
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::PictureGrabber::onAddChannel()
{
  //create the channel structure for one channel
  PictureChannelPtr channel(new PictureChannel);
  mChannels.push_back(channel);
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::PictureGrabber::onInit()
{
  //local and/or stored parameters are already initialized
  #ifdef SHOW_INIT_INFORMATION_PICTUREGRABBER
    std::cout << "PictureGrabber: Initialize Grabber with " << mNumCams << " pictures ..." << std::endl;
    for (unsigned int i = 0; i < mNumCams; ++i)
    {
      std::cout << "Channel " << i << ": capture from Picture: " << mSourceFileNames.at(i) << "\n";
    }
    std::cout << std::flush;
  #endif

  //for every channel, read from image-file
  for (unsigned int channel = 0; channel < mNumCams; ++channel)
  {
    cv::Mat frame = cv::imread(getChannel(channel)->sourceFileName);

    if (!frame.empty())
    {
      getChannel(channel)->imageMat = frame;
    }
    else
    {
      //todo: exception raus
      std::cout << "[PictureGrabber::onInit] ERROR: Grabbing failed\n"
                << "\tChannel " << channel << ": \"" << getChannel(channel)->sourceFileName << "\"."
                << std::endl;
      return false; //throws initialization exception
    }
  }
  //all grabbers successfully initialized

  #ifdef DEBUG_PICTUREGRABBER
    std::cout << "[PictureGrabber::onInit] finished" << std::endl;
  # endif

  return true;
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::PictureGrabber::onDeclareParameters()
{
  return true;
}

//----------------------------------------------------------------------------------------------------
const std::string& cedar::dev::sensors::visual::PictureGrabber::onGetSourceInfo(unsigned int channel) const
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"PictureGrabber::onGetSourceInfo");
  }
  return getChannel(channel)->sourceFileName;
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::PictureGrabber::onGrab()
{
  bool result = true;
  for (unsigned int channel = 0; channel < mNumCams; channel++)
  {
    result = !(getChannel(channel)->imageMat.empty()) && result;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::PictureGrabber::setSourceFile(unsigned int channel, const std::string& fileName)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"PictureGrabber::setPictureFileName");
  }

  getChannel(channel)->sourceFileName = fileName;

  //lock image-matrix while writing
  mpReadWriteLock->lockForWrite();
  getChannel(channel)->imageMat = cv::imread(fileName);
  mpReadWriteLock->unlock();

  if (getChannel(channel)->imageMat.empty())
  {
    CEDAR_THROW(cedar::aux::exc::InitializationException,"PictureGrabber: Error on reading from file \""+fileName+"\"");
  }
}
