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
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
//constructors and destructor
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//Constructor for single-file grabber
cedar::dev::sensors::visual::PictureGrabber::PictureGrabber
(
  const std::string& configFileName,
  const std::string& pictureFileName
)
:
cedar::dev::sensors::visual::GrabberInterface(configFileName)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
  readInit(1,"PictureGrabber");

  //change/overwrite parameters with constructor values
  getChannel(0)->mSourceFileName = pictureFileName;
  applyInit();
}

//----------------------------------------------------------------------------------------------------
//Constructor for stereo-file grabber
cedar::dev::sensors::visual::PictureGrabber::PictureGrabber
(
  const std::string& configFileName,
  const std::string& pictureFileName0,
  const std::string& pictureFileName1
)
:
cedar::dev::sensors::visual::GrabberInterface(configFileName)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
  readInit(2,"PictureGrabber");
  getChannel(0)->mSourceFileName = pictureFileName0;
  getChannel(1)->mSourceFileName = pictureFileName1;
  applyInit();
}

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::PictureGrabber::~PictureGrabber()
{
  doCleanUp();
  cedar::aux::LogSingleton::getInstance()->freeing(this);
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
  std::stringstream init_message;
  init_message << ": Initialize picture grabber with " << mNumCams << " pictures ..." << std::endl;
  for (unsigned int i = 0; i < mNumCams; ++i)
  {
    init_message << "\tChannel " << i << ": capture from Picture: " << getChannel(i)->mSourceFileName << std::endl;
  }
  cedar::aux::LogSingleton::getInstance()->systemInfo
                                           (
                                             ConfigurationInterface::getName() + init_message.str(),
                                             "cedar::dev::sensors::visual::PictureGrabber::onInit()"
                                           );

  //for every channel, read from image-file
  for (unsigned int channel = 0; channel < mNumCams; ++channel)
  {
    cv::Mat frame = cv::imread(getChannel(channel)->mSourceFileName);

    if (!frame.empty())
    {
      getChannel(channel)->mImageMat = frame;
    }
    else
    {
      cedar::aux::LogSingleton::getInstance()->error
                                               (
                                                ConfigurationInterface::getName() + ": Grabbing failed on Channel "
                                                  + boost::lexical_cast<std::string>(channel) + " from \""
                                                  + getChannel(channel)->mSourceFileName + "\"",
                                                "cedar::dev::sensors::visual::PictureGrabber::onInit()"
                                               );
      return false; //throws initialization exception
    }
  }
  //all grabbers successfully initialized

  return true;
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::PictureGrabber::onDeclareParameters()
{
  return true;
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::PictureGrabber::onUpdateSourceInfo(unsigned int channel)
{
  //value of channel is already checked by GraberInterface::getSourceInfo()
  getChannel(channel)->mChannelInfo = getChannel(channel)->mSourceFileName;
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::PictureGrabber::onGrab()
{
  bool result = true;
  for (unsigned int channel = 0; channel < mNumCams; channel++)
  {
    result = !(getChannel(channel)->mImageMat.empty()) && result;
  }
  return result;
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::PictureGrabber::setSourceFile(unsigned int channel, const std::string& fileName)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"PictureGrabber::setPictureFileName");
  }

  getChannel(channel)->mSourceFileName = fileName;

  //lock image-matrix while writing
  mpReadWriteLock->lockForWrite();
  getChannel(channel)->mImageMat = cv::imread(fileName);
  mpReadWriteLock->unlock();

  if (getChannel(channel)->mImageMat.empty())
  {
    CEDAR_THROW(cedar::aux::InitializationException,"PictureGrabber: Error on reading from file \""+fileName+"\"");
  }
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::PictureGrabber::setSourceFile(const std::string& fileName)
{
  setSourceFile(0,fileName);
}
