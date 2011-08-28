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

// PROJECT INCLUDES

// SYSTEM INCLUDES


#include <auxiliaries/exceptions/IndexOutOfRangeException.h>

using namespace cv;
using namespace cedar::dev::sensors::visual;


//----------------------------------------------------------------------------------------------------
// Constructor for single-file grabber
PictureGrabber::PictureGrabber(std::string grabberName,
                       std::string configFileName,
                       std::string pictureFileName)
    :   GrabberInterface(configFileName)
{
    mSourceFileName.push_back(pictureFileName);
    doInit(grabberName, mSourceFileName.size());

}



//----------------------------------------------------------------------------------------------------
// Constructor for stereo-file grabber
PictureGrabber::PictureGrabber(std::string grabberName,
           std::string configFileName,
           std::string pictureFileName0,
           std::string pictureFileName1)
:   GrabberInterface(configFileName)
{
  mSourceFileName.push_back(pictureFileName0);
  mSourceFileName.push_back(pictureFileName1);
  doInit(grabberName, mSourceFileName.size());
}

//----------------------------------------------------------------------------------------------------
PictureGrabber::~PictureGrabber()
{
  //std::cout<<"VideoGrabber::Destructor\n";
  //VideoCaptures are released automatically within the Vector mCaptureVector
} 


//----------------------------------------------------------------------------------------------------
bool PictureGrabber::onInit()
{
  
  //local and/or stored parameters are already initialized

#if defined SHOW_INIT_INFORMATION_PICTUREGRABBER
  std::cout << "PictureGrabber: Initialize Grabber with " << mNumCams << " pictures ..." << std::endl;
  for(unsigned int i=0; i<mNumCams;++i)
  {
     std::cout << "Channel "<< i<<": capture from Picture: " << mSourceFileName.at(i) << "\n";
  }
  std::cout << std::flush;
#endif

  mImageMatVector.clear();

  for(unsigned int i=0; i<mNumCams;++i)
  {
    cv::Mat frame = cv::imread(mSourceFileName.at(i));

    if (!frame.empty())
    {
      mImageMatVector.push_back(frame);
    }
    else
    {
      std::cout << "ERROR: Grabbing failed (Channel "<< i << "): \""<< mSourceFileName.at(i)<<"\"." << std::endl;
      return false;
      //exception thrown in GrabberInterface
    }


  }
  // all grabbers successfully initialized

  //TODO
  //set fps 
  //until now, it is set to default value of loopedThread
  //maybe read fps and check against default value from loopedthread
  //to decide if it was load from config-file 

  #if defined DEBUG_PICTUREGRABBER
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
std::string PictureGrabber::onGetPhysicalSourceInformation(unsigned int channel) const
{
	return mSourceFileName.at(channel);
}
//----------------------------------------------------------------------------------------------------
bool PictureGrabber::onGrab()
{
  //@todo: check if matrizes are ok!!
   return true;
}



//----------------------------------------------------------------------------------------------------
bool PictureGrabber::setPictureFileName(unsigned int channel, const std::string& FileName )
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
    CEDAR_THROW(cedar::aux::exc::InitializationException,"PictureGrabber::setPictureFileName");
  }

  return true;

}

