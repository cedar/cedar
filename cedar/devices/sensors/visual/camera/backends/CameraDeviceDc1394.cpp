/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        CameraDeviceDc1394.cpp

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Implementation for the cedar::dev::sensors::visual::CameraDeviceDc1394 class

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/camera/backends/CameraDeviceDc1394.h"


// SYSTEM INCLUDES

#ifdef CEDAR_USE_LIB_DC1394

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::sensors::visual::CameraDeviceDc1394::CameraDeviceDc1394
(
//  cedar::dev::sensors::visual::CameraSettingsPtr pSettings,
//  cedar::dev::sensors::visual::CameraPropertiesPtr pProperties,
//  cv::VideoCapture videoCapture,
//  QReadWriteLock* pVideoCaptureLock
  cedar::dev::sensors::visual::CameraChannelPtr pCameraChannel
)
:
cedar::dev::sensors::visual::CameraDevice::CameraDevice(pCameraChannel)
//cedar::dev::sensors::visual::CameraDevice::CameraDevice
//(
//  pSettings,
//  pProperties,
//  videoCapture,
//  pVideoCaptureLock
//)
{
  //!@todo Implement the firewire backend:
  //
  // 1. lock
  // 2. if capture already there, delete it
  // 3  fill p_capabilities with the right values (depends on backend and camera)
  // 4. create cv::Videocapture
  // 4.1   apply settings from p_settings structure
  // 4.2   restore state of the device with the values in p_state

  // 5. done
}


cedar::dev::sensors::visual::CameraDeviceDc1394::~CameraDeviceDc1394()
{
}


//  //if create by grabber id, search all cameras and get the device-nr on the bus
//  if (getCameraChannel(0)->_mByGuid->getValue())
//  {
//    //all videocaptures already created, but not opened
//
//    // search for the right guid, so open it one by one
//    unsigned int cam = 0;
//    unsigned int found_cams=0;
//    bool all_cams_found = false;
//    bool no_more_cams = false;
//
//    //search all cams, i.e. all bus-ids
//    cedar::aux::LogSingleton::getInstance()->debugMessage
//                                            (
//                                              "Searching for cameras on the bus...",
//                                              "cedar::dev::sensors::visual::CameraGrabber::onInit()"
//                                            );
//
//    while (! (all_cams_found || no_more_cams))
//    {
//      //open one by one
//      cv::VideoCapture capture(cam);
//      if (capture.isOpened())
//      {
//        unsigned int guid = capture.get((int)CV_CAP_PROP_GUID);
//
//        cedar::aux::LogSingleton::getInstance()->debugMessage
//                                                (
//                                                  "\tFound at Bus-Id " + boost::lexical_cast<std::string>(cam)
//                                                    + ": GUID " + boost::lexical_cast<std::string>(guid),
//                                                  "cedar::dev::sensors::visual::CameraGrabber::onInit()"
//                                                );
//
//        //check if guid of camera is in our guid-vector
//        for (unsigned int channel = 0; channel < num_cams; ++channel)
//        {
//          unsigned int test_guid = getCameraChannel(channel)->_mGuid->getValue();
//          if (guid == test_guid )
//          {
//            //yes: store bus_id and videocapture
//            getCameraChannel(channel)->_mBusId->setValue(cam);
//            getCameraChannel(channel)->mVideoCapture = capture;
//            found_cams++;
//          }
//        }
//
//        //check if all cams found
//        if (found_cams == num_cams)
//        {
//          all_cams_found = true;
//          cedar::aux::LogSingleton::getInstance()->debugMessage
//                                                  (
//                                                    "All needed cameras found. Stop searching.",
//                                                    "cedar::dev::sensors::visual::CameraGrabber::onInit()"
//                                                  );
//        }
//      }
//      else
//      {
//        //there are no more cams connected
//        no_more_cams = true;
//      }
//      cam++;
//    }
//
//    if (! all_cams_found)
//    {
//      cedar::aux::LogSingleton::getInstance()->error
//                                               (
//                                                 this->getName() + "At least ore camera not found!",
//                                                 "cedar::dev::sensors::visual::CameraGrabber::onInit()"
//                                               );
//      return false; //throws an initialization-exception
//    }
//  }
//  else
//  {
//  // create by ongoing number
//    for (unsigned int channel = 0; channel < num_cams; ++channel)
//    {
//      //open the camera
//      cv::VideoCapture capture(getCameraChannel(channel)->_mBusId->getValue());
//      if (! capture.isOpened())
//      {
//        cedar::aux::LogSingleton::getInstance()->error
//                                                 (
//                                                   this->getName()
//                                                     + ": Couldn't open camera (Channel "
//                                                     + boost::lexical_cast<std::string>(channel) + ")",
//                                                   "cedar::dev::sensors::visual::CameraGrabber::onInit()"
//                                                 );
//        return false; //throws an initialization-exception
//      }
//
//      //store it in our vector
//      getCameraChannel(channel)->mVideoCapture = capture;
//    }
//  }
//
//  //now initialize all videocaptures
//  for (unsigned int channel = 0; channel < num_cams; ++channel)
//   {
//
//    //if grabber created by busId, guid is zero, so read it
//    if ( !getCameraChannel(0)->_mByGuid->getValue())
//    {
//      getCameraChannel(channel)
//          ->_mGuid->setValue(static_cast<unsigned int>(getCameraChannel(channel)->mVideoCapture.get(CV_CAP_PROP_GUID)));
//    }
//
//    //create and load the camera capabilities
//    //@todo config:
//    std::string conf_file_name = "default_cam_capabilities";
//    //conf_file_name = ConfigurationInterface::getConfigFileName()+"_ch"+boost::lexical_cast<std::string>(channel);
//
//    //if capabilities filename match with definition for autogenerated filename, create filename
//    std::string cap_file_name = "todo";//getCameraChannel(channel)->mCamCapabilitiesFileName;
//    unsigned int guid = getCameraChannel(channel)->_mGuid->getValue();
//
//
//    //create camera configuration
//    try
//    {
//     /* CameraStateAndConfigPtr cam_conf
//                              (
//                                new CameraStateAndConfig
//                                    (
//                                      getCameraChannel(channel)->mVideoCapture,
//                                      getCameraChannel(channel)->pmVideoCaptureLock,
//                                      channel,
//                                      conf_file_name,
//                                      cap_file_name
//                                    )
//                              );
//      getCameraChannel(channel)->mCamStateAndConfig = cam_conf;
//      */
//    }
//    catch (...)
//    {
//      return false; //throws an initialization exception
//    }
//  }
//
//  //Check if initialization should be finished.
//  for (unsigned int channel = 0; channel < num_cams; ++channel)
//  {
//    getCameraChannel(channel)->mVideoCapture >> getCameraChannel(channel)->mImageMat;
//    if (getCameraChannel(channel)->mImageMat.empty())
//    {
//      cedar::aux::LogSingleton::getInstance()->error
//                                       (
//                                         this->getName()
//                                           + ": Couldn't retrieve frame from camera (Channel "
//                                           + boost::lexical_cast<std::string>(channel) + ")",
//                                         "cedar::dev::sensors::visual::CameraGrabber::onInit()"
//                                       );
//      return false; //throws an initialization-exception
//    }
//    setChannelInfo(channel);
//  }
//
//  init_message.clear();
//  init_message << "Camera opened:" << std::endl;
//  for (unsigned int i = 0; i < num_cams; ++i)
//  {
//    init_message << "Channel " << i << ": GUID: "<< getCameraChannel(i)->_mGuid->getValue() << "\n";
//  }
//  cedar::aux::LogSingleton::getInstance()->systemInfo
//                                           (
//                                             this->getName() + init_message.str(),
//                                             "cedar::dev::sensors::visual::CameraGrabber::onInit()"
//                                           );
//  //all grabbers are now successfully initialized
//
//  //check for largest FPS
//  double fps = getCameraChannel(0)->mVideoCapture.get(CV_CAP_PROP_FPS);
//  for (unsigned int i = 1; i < getNumCams(); ++i)
//  {
//    double fps_test = getCameraChannel(i)->mVideoCapture.get(CV_CAP_PROP_FPS);
//    if (fps_test > fps)
//    {
//      fps = fps_test;
//    }
//  }
//
//
//  //set stepsize for LoopedThread
//  setFps(fps);
//
//  mCaptureDeviceCreated = true;


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::sensors::visual::CameraDeviceDc1394::fillCapabilities()
{

}

bool cedar::dev::sensors::visual::CameraDeviceDc1394::createCaptureDevice()
{

}

void cedar::dev::sensors::visual::CameraDeviceDc1394::applySettingsToCamera()
{

}

void cedar::dev::sensors::visual::CameraDeviceDc1394::applyStateToCamera()
{

}

#endif // CEDAR_USE_LIB_DC1394
