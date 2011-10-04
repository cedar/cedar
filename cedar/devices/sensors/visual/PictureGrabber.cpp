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

    File:        ConfigurationInterface.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@rub.de
    Date:        2011 01 08

    Description: Grabber to grab from pictures.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "PictureGrabber.h"
#include "../../../auxiliaries/exceptions/IndexOutOfRangeException.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES




using namespace cv;
using namespace cedar::dev::sensors::visual;

//----------------------------------------------------------------------------------------------------------------------
//constructors and destructor
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
//Constructor for single-file grabber
PictureGrabber::PictureGrabber(
                                const std::string& configFileName,
                                const std::string& pictureFileName
                              )
  : GrabberInterface(configFileName)
{
  mSourceFileName.push_back(pictureFileName);
  doInit(mSourceFileName.size(),"PictureGrabber");
}



//----------------------------------------------------------------------------------------------------
//Constructor for stereo-file grabber
PictureGrabber::PictureGrabber(
                                const std::string& configFileName,
                                const std::string& pictureFileName0,
                                const std::string& pictureFileName1
                              )
  : GrabberInterface(configFileName)
{
  mSourceFileName.push_back(pictureFileName0);
  mSourceFileName.push_back(pictureFileName1);
  doInit(mSourceFileName.size(),"PictureGrabber");
}

//----------------------------------------------------------------------------------------------------
PictureGrabber::~PictureGrabber()
{
  #ifdef DEBUG_PICTUREGRABBER
    std::cout<<"[PictureGrabber::Destructor]"<< std::endl;
  #endif
  //VideoCaptures are released automatically within the Vector mCaptureVector
}


//----------------------------------------------------------------------------------------------------------------------
//methods
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------
bool PictureGrabber::onInit()
{

  //local and/or stored parameters are already initialized

  #ifdef SHOW_INIT_INFORMATION_PICTUREGRABBER
    std::cout << "PictureGrabber: Initialize Grabber with " << mNumCams << " pictures ..." << std::endl;

    for (unsigned int i = 0; i < mNumCams; ++i)
    {
      std::cout << "Channel " << i << ": capture from Picture: " << mSourceFileName.at(i) << "\n";
    }
    std::cout << std::flush;
  #endif


  mImageMatVector.clear();

  for (unsigned int i = 0; i < mNumCams; ++i)
  {
    cv::Mat frame = cv::imread(mSourceFileName.at(i));

    if (!frame.empty())
    {
      mImageMatVector.push_back(frame);
    }
    else
    {
      std::cout << "ERROR: Grabbing failed (Channel " << i << "): \"" << mSourceFileName.at(i) << "\"." << std::endl;
      return false;
      //exception thrown in GrabberInterface
    }
  }
  //all grabbers successfully initialized

  #ifdef DEBUG_PICTUREGRABBER
    std::cout << "[PictureGrabber::onInit] finished" << std::endl;
  # endif

  return true;
}


//----------------------------------------------------------------------------------------------------
bool PictureGrabber::onDeclareParameters()
{
  return true;
}

//----------------------------------------------------------------------------------------------------
std::string PictureGrabber::onGetSourceInfo(unsigned int channel) const
{
  return mSourceFileName.at(channel);
}
//----------------------------------------------------------------------------------------------------
bool PictureGrabber::onGrab()
{
  bool result = true;
  for (unsigned int i=0; i<mNumCams; i++)
  {
    result &= !mImageMatVector.at(i).empty();
  }
  return true;
}



//----------------------------------------------------------------------------------------------------
bool PictureGrabber::setSourceFile(unsigned int channel, const std::string& FileName )
{
  if (channel >= mNumCams)
  {
    CEDAR_THROW(cedar::aux::exc::IndexOutOfRangeException,"PictureGrabber::setPictureFileName");
  }

  //lock image-matrix while writing
  mpReadWriteLock->lockForWrite();
  mSourceFileName.at(channel) = FileName;
  mImageMatVector.at(channel) = cv::imread(FileName);
  mpReadWriteLock->unlock();

  if (mImageMatVector.at(channel).empty())
  {
    CEDAR_THROW(cedar::aux::exc::InitializationException,"PictureGrabber::setSourceFile");
  }

  return true;
}
