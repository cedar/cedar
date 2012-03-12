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
  mSourceFileNames.push_back(pictureFileName);
  readInit(mSourceFileNames.size(),"PictureGrabber");
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
  mSourceFileNames.push_back(pictureFileName0);
  mSourceFileNames.push_back(pictureFileName1);

  readInit(mSourceFileNames.size(),"PictureGrabber");
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

  mImageMatVector.clear();

  //for every channel, read from image-file
  for (unsigned int i = 0; i < mNumCams; ++i)
  {
    cv::Mat frame = cv::imread(mSourceFileNames.at(i));

    if (!frame.empty())
    {
      mImageMatVector.push_back(frame);
    }
    else
    {
      //todo: exception raus
      std::cout << "[PictureGrabber::onInit] ERROR: Grabbing failed\n"
                << "\tChannel " << i << ": \"" << mSourceFileNames.at(i) << "\"."
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
  return mSourceFileNames.at(channel);
}

//----------------------------------------------------------------------------------------------------
bool cedar::dev::sensors::visual::PictureGrabber::onGrab()
{
  bool result = true;
  for (unsigned int i = 0; i < mNumCams; i++)
  {
    result = !mImageMatVector.at(i).empty() && result;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------
//todo void
bool cedar::dev::sensors::visual::PictureGrabber::setSourceFile(unsigned int channel, const std::string& fileName)
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"PictureGrabber::setPictureFileName");
  }

  //lock image-matrix while writing
  mpReadWriteLock->lockForWrite();
  mSourceFileNames.at(channel) = fileName;
  mImageMatVector.at(channel) = cv::imread(fileName);
  mpReadWriteLock->unlock();

  if (mImageMatVector.at(channel).empty())
  {
    CEDAR_THROW(cedar::aux::exc::InitializationException,"PictureGrabber::setSourceFile");
  }
  return true;
}
