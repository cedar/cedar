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
#include "cedar/devices/sensors/visual/exceptions.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declared
    = cedar::dev::sensors::visual::Grabber::ChannelManagerSingleton::getInstance()
        ->registerType<cedar::dev::sensors::visual::PictureChannelPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------


// Constructor for single-file grabber
cedar::dev::sensors::visual::PictureGrabber::PictureGrabber
(
  const std::string& pictureFileName
)
:
cedar::dev::sensors::visual::Grabber
(
  "PictureGrabber",
  cedar::dev::sensors::visual::PictureChannelPtr
  (
    new cedar::dev::sensors::visual::PictureChannel(pictureFileName)
  )
)
{
  init();
}


// Constructor for stereo-file grabber
cedar::dev::sensors::visual::PictureGrabber::PictureGrabber
(
  const std::string& pictureFileName0,
  const std::string& pictureFileName1
)
:
cedar::dev::sensors::visual::Grabber
(
  "StereoPictureGrabber",
  cedar::dev::sensors::visual::PictureChannelPtr
  (
    new cedar::dev::sensors::visual::PictureChannel(pictureFileName0)
  ),
  cedar::dev::sensors::visual::PictureChannelPtr
  (
    new cedar::dev::sensors::visual::PictureChannel(pictureFileName1)
  )
)
{
  init();
}


void cedar::dev::sensors::visual::PictureGrabber::init()
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  // watch filename on every channel
  for (unsigned int channel=0; channel<_mChannels->size(); ++channel)
  {
    QObject::connect
             (
               getPictureChannel(channel)->_mSourceFileName.get(),
               SIGNAL(valueChanged()),
               this,
               SLOT(fileNameChanged())
             );
  }

  // watch if channel added to ObjectParameterList
  _mChannels->connectToObjectAddedSignal
              (
                boost::bind(&cedar::dev::sensors::visual::PictureGrabber::channelAdded,this,_1)
              );

}


cedar::dev::sensors::visual::PictureGrabber::~PictureGrabber()
{
  doCleanUp();
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}

//--------------------------------------------------------------------------------------------------------------------
// slots
//--------------------------------------------------------------------------------------------------------------------

void cedar::dev::sensors::visual::PictureGrabber::channelAdded(int index)
{
  QObject::connect
           (
             getPictureChannel(index)->_mSourceFileName.get(),
             SIGNAL(valueChanged()),
             this,
             SLOT(fileNameChanged())
           );
}


void cedar::dev::sensors::visual::PictureGrabber::fileNameChanged()
{
  // get sender
  cedar::aux::FileParameterPtr p_sender
    = cedar::aux::FileParameterPtr(cedar::aux::asserted_cast<cedar::aux::FileParameter * >(QObject::sender()));

  // search for the changed channel
  unsigned int num_channels = getNumChannels();
  unsigned int channel = 0;

  while ((channel < num_channels) && (p_sender != getPictureChannel(channel)->_mSourceFileName))
  {
    channel++;
  }

  // if found, read new image for that channel
  if (p_sender == getPictureChannel(channel)->_mSourceFileName)
  {
    const std::string filename = getPictureChannel(channel)->_mSourceFileName->getPath(true);

    // lock image-matrix for writing
    mpReadWriteLock->lockForWrite();
    getImageMat(channel) = cv::imread(filename);
    mpReadWriteLock->unlock();

    if (getImageMat(channel).empty())
    {
      std::string message = this->getName() + ": FileNameChanged: Grabbing failed on Channel "
                            + cedar::aux::toString(channel) + " from \"" + filename + "\"" ;
      cedar::aux::LogSingleton::getInstance()->error
                                               (
                                                 message,
                                                 "cedar::dev::sensors::visual::PictureGrabber::fileNameChanged()"
                                               );
      this->closeGrabber();
    }
    else
    {
      setIsCreated(true);
      onGetSourceInfo(channel);
      emit pictureChanged();
    }
  }

}


void cedar::dev::sensors::visual::PictureGrabber::onCloseGrabber()
{
  // nothing to reset here
}


void cedar::dev::sensors::visual::PictureGrabber::onCreateGrabber()
{
  const std::string name = this->getName();
  unsigned int num_channels = getNumChannels();

  // for every channel, read from image-file
  for (unsigned int channel = 0; channel < num_channels; ++channel)
  {
    // read from file
    cv::Mat frame = cv::imread(getPictureChannel(channel)->_mSourceFileName->getPath(true));

    // test if successful
    if (frame.empty())
    {
      std::string msg = name + " Channel " + cedar::aux::toString(channel)
                        + ": Could not read from \""+ getPictureChannel(channel)->_mSourceFileName->getPath(true) + "\"";
      CEDAR_THROW(cedar::dev::sensors::visual::CreateGrabberException,msg);
    }

    // copy frame to the channel image matrix
    getImageMat(channel) = frame;
  }

  // signal to indicate, that there is a new frame
  emit pictureChanged();
}


void cedar::dev::sensors::visual::PictureGrabber::onGrab(unsigned int channel)
{
  if (getImageMat(channel).empty())
  {
    std::string msg = "Image is empty on channel " + cedar::aux::toString(channel);
    CEDAR_THROW(cedar::dev::sensors::visual::GrabberGrabException,msg)
  }
}


void cedar::dev::sensors::visual::PictureGrabber::setSourceFile(unsigned int channel, const std::string& fileName)
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"PictureGrabber::setSourceFile");
  }

  // signal onChanged() connected to fileNameChanged()
  getPictureChannel(channel)->_mSourceFileName->setValue(fileName);
}


void cedar::dev::sensors::visual::PictureGrabber::setSourceFile(const std::string& fileName)
{
  setSourceFile(0,fileName);
}


const std::string cedar::dev::sensors::visual::PictureGrabber::getSourceFile(unsigned int channel) const
{
  if (channel >= getNumChannels())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException,"PictureGrabber::setSourceFile");
  }
  return getPictureChannel(channel)->_mSourceFileName->getPath(true);
}


std::string cedar::dev::sensors::visual::PictureGrabber::onGetSourceInfo(unsigned int channel)
{
  return getPictureChannel(channel)->_mSourceFileName->getPath(true);
}
