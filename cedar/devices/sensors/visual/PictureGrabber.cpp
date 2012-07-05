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

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/PictureGrabber.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declared
    = cedar::dev::sensors::visual::Grabber::ChannelManagerSingleton::getInstance()
        ->registerType<cedar::dev::sensors::visual::PictureGrabber::PictureChannelPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
//constructors and destructor
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//Constructor for single-file grabber
cedar::dev::sensors::visual::PictureGrabber::PictureGrabber
(
  const std::string& pictureFileName,
  const std::string& grabberName
)
:
cedar::dev::sensors::visual::Grabber
(
  grabberName,
  cedar::dev::sensors::visual::PictureGrabber::PictureChannelPtr
  (
    new cedar::dev::sensors::visual::PictureGrabber::PictureChannel(pictureFileName)
  )
)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
}

//----------------------------------------------------------------------------------------------------
//Constructor for stereo-file grabber
cedar::dev::sensors::visual::PictureGrabber::PictureGrabber
(
  const std::string& pictureFileName0,
  const std::string& pictureFileName1,
  const std::string& grabberName
)
:
cedar::dev::sensors::visual::Grabber
(
  grabberName,
  cedar::dev::sensors::visual::PictureGrabber::PictureChannelPtr
  (
    new cedar::dev::sensors::visual::PictureGrabber::PictureChannel(pictureFileName0)
  ),
  cedar::dev::sensors::visual::PictureGrabber::PictureChannelPtr
  (
    new cedar::dev::sensors::visual::PictureGrabber::PictureChannel(pictureFileName1)
  )
)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
  //readInit(2);
  //getPictureChannel(0)->_mSourceFileName->setValue(pictureFileName0);
  //getPictureChannel(1)->_mSourceFileName->setValue(pictureFileName1);
  //applyInit();
}

//----------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::PictureGrabber::~PictureGrabber()
{
  doCleanUp();
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}


//--------------------------------------------------------------------------------------------------------------------
// configurable interface
//--------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::visual::PictureGrabber::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  //do readConfiguration for all childs. This is implemented in the base class
  cedar::aux::Configurable::readConfiguration(node);

  //initialize grabber:

}

//----------------------------------------------------------------------------------------------------------------------
// grabber
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::PictureGrabber::onInit()
{
  //local and/or stored parameters are already initialized
  const std::string name = this->getName();
  std::stringstream init_message;
  init_message << "Initialize picture grabber with " << mNumCams << " channel(s) ..." << std::endl;
  for (unsigned int i = 0; i < mNumCams; ++i)
  {
    init_message << "\tChannel " << i << ": capture from Picture: "
                 << getPictureChannel(i)->_mSourceFileName->getPath() << std::endl;
  }
  cedar::aux::LogSingleton::getInstance()->message
                                           (
                                             name + ": " + init_message.str(),
                                             "cedar::dev::sensors::visual::PictureGrabber::onInit()"
                                           );

  //for every channel, read from image-file
  for (unsigned int channel = 0; channel < mNumCams; ++channel)
  {
    cv::Mat frame = cv::imread(getPictureChannel(channel)->_mSourceFileName->getPath());

    if (!frame.empty())
    {
      getPictureChannel(channel)->mImageMat = frame;
    }
    else
    {
      cedar::aux::LogSingleton::getInstance()->error
                                               (
                                                  name + ": Grabbing failed on Channel "
                                                  + boost::lexical_cast<std::string>(channel) + " from \""
                                                  + getPictureChannel(channel)->_mSourceFileName->getPath() + "\"",
                                                "cedar::dev::sensors::visual::PictureGrabber::onInit()"
                                               );
      return false; //throws initialization exception
    }
  }
  //all grabbers successfully initialized

  return true;
}


//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::PictureGrabber::onUpdateSourceInfo(unsigned int channel)
{
  //value of channel is already checked by Grabber::getSourceInfo()
  getPictureChannel(channel)->mChannelInfo = getPictureChannel(channel)->_mSourceFileName->getPath();
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::PictureGrabber::onGrab()
{
  bool result = true;
  for (unsigned int channel = 0; channel < mNumCams; channel++)
  {
    result = !(getPictureChannel(channel)->mImageMat.empty()) && result;
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// picturegrabber
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::PictureGrabber::setSourceFile(unsigned int channel, const std::string& fileName)
{
  // todo: ask olli: all range checking:
  //CEDAR_ASSERT(index < this->size());
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"PictureGrabber::setPictureFileName");
  }

  getPictureChannel(channel)->_mSourceFileName->setValue(fileName);

  //lock image-matrix for writing
  mpReadWriteLock->lockForWrite();
  getPictureChannel(channel)->mImageMat = cv::imread(fileName);
  mpReadWriteLock->unlock();

  if (getPictureChannel(channel)->mImageMat.empty())
  {
    CEDAR_THROW(cedar::aux::InitializationException,"PictureGrabber: Error on reading from file \""+fileName+"\"");
  }
}

//----------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::PictureGrabber::setSourceFile(const std::string& fileName)
{
  setSourceFile(0,fileName);
}
