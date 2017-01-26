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

    File:        StereoCameraGrabber.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Simple application to grab from a camera (stereo-case)

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/devices/sensors/camera/Grabber.h"
#include "cedar/auxiliaries/gui/ImagePlot.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <opencv2/opencv.hpp>
#ifndef Q_MOC_RUN
  #include <boost/lexical_cast.hpp>
#endif
#include <ios>

// ---------------------------------------------------------------------------------------------------------------------
// Local methods
// ---------------------------------------------------------------------------------------------------------------------
namespace
{
  void processQtEvents()
  {
#ifdef CEDAR_OS_APPLE
    unsigned int event_counter = 0;
    while (QApplication::hasPendingEvents() && (++event_counter < 1000))
#else
    while (QApplication::hasPendingEvents())
#endif
    {
      QApplication::processEvents();
    }
  }
}

// ---------------------------------------------------------------------------------------------------------------------
// Interactive test program
// ---------------------------------------------------------------------------------------------------------------------

// Use this, if you want to test with a firewire camera
#define USE_FIREWIRE_BACKEND

int main(int argc, char* argv[])
{
  // grab from device number on the bus (as an integer)
  const int DEVICE_ID_CHANNEL0 = 0;
  const int DEVICE_ID_CHANNEL1 = 1;

  // given device-ID is the guid of the cam. or the id on the bus
  const bool  IS_GUID = false;

  // the name of the grabber
  // only used in the configuration file
  const std::string GRABBER_NAME = "StereoCameraGrabber_TestCase";

  // the name of the configuration file
  const std::string CONFIG_FILE_NAME = "cameragrabber_stereo_testcase.config";

  //title of plot window
  std::string window_title = "Cameras: " + boost::lexical_cast<std::string>(DEVICE_ID_CHANNEL0)
      + ", " + boost::lexical_cast<std::string>(DEVICE_ID_CHANNEL1);


  //--------------------------------------------------------------------------------------------------------------------
  //main test
  //--------------------------------------------------------------------------------------------------------------------
  std::cout.setf(std::ios::fixed,std::ios::floatfield);
  std::cout.precision(3);

  std::cout << "\n\nInteractive test of the CameraGrabber class (stereo)\n";
  std::cout << "--------------------------------------------\n\n";


  //----------------------------------------------------------------------------------------
  //Create the grabber
  //----------------------------------------------------------------------------------------
  std::cout << "Create a CameraGrabber (stereo):\n";
  cedar::dev::sensors::camera::GrabberPtr p_grabber;
  try
  {
    p_grabber = cedar::dev::sensors::camera::GrabberPtr
                (
                  new cedar::dev::sensors::camera::Grabber
                      (
                        DEVICE_ID_CHANNEL0,
                        DEVICE_ID_CHANNEL1,
                        IS_GUID
#ifdef USE_FIREWIRE_BACKEND
                        ,
                        cedar::dev::sensors::camera::BackendType::DC1394
#endif //USE_FIREWIRE_BACKEND
                      )
                );
  }
  catch (cedar::aux::InitializationException &e)
  {
    //after an InitializationExeception the grabber class isn't initialized correctly
    //and couldn't be used
    std::cout << "Error on creation of the CameraGrabber class:\n"
              << e.exceptionInfo() << std::endl;

    //delete p_grabber is done via the shared-pointer class
    return -1;
  }

  // activate crash-handler if there is any hardware-related stuff which has to be cleaned up
  p_grabber->installCrashHandler();

  //----------------------------------------------------------------------------------------
  // load configuration. this step is optional.
  //----------------------------------------------------------------------------------------
  try
  {
    //p_grabber->readJson(CONFIG_FILE_NAME);
  }
  catch (...)
  {
    std::cout << "No configuration exists!" << std::endl;
  }

  //----------------------------------------------------------------------------------------
  // change settings or properties
  //----------------------------------------------------------------------------------------


#ifdef USE_FIREWIRE_BACKEND
#ifdef CEDAR_USE_LIB_DC1394
  //only available for firewire cameras:
  p_grabber->setCameraIsoSpeed(0,cedar::dev::sensors::camera::IsoSpeed::ISO_200);
#endif // CEDAR_USE_LIB_DC1394
#endif // USE_FIREWIRE_BACKEND

  // you have to check if the framerate is supported by the used camera
  // on firewirecameras only supperted framerates could be set
  p_grabber->setCameraFramerate(0,cedar::dev::sensors::camera::FrameRate::FPS_15);

  //----------------------------------------------------------------------------------------
  // apply configuration. this step is essential.
  //----------------------------------------------------------------------------------------
  // if you don't apply the default or loaded Parameter, the grabber will not work
  // check if grabber is created successfully
  if (! p_grabber->applyParameter())
  {
    // an error occured during initialization. Perhaps the file doesn't exist
    return -1;
  }

  //first lets set a custom name for our grabber. This name will be used later in the configuration file
  p_grabber->setName(GRABBER_NAME);

  std::cout << "\nGrab channel 0 from \"" << p_grabber->getSourceInfo()<< "\"" << std::endl;
  std::cout << "\nGrab channel 1 from \"" << p_grabber->getSourceInfo(1)<< "\"" << std::endl;

  //----------------------------------------------------------------------------------------
  // print the settings and properties from the camera
  //----------------------------------------------------------------------------------------

  //Size of camera images
  std::cout << "\nSize of loaded frame:\n";
  cv::Size ch0_size = p_grabber->getSize(0);
  cv::Size ch1_size = p_grabber->getSize(1);
  std::cout << "\tChannel 0: " << ch0_size.width <<" x " << ch0_size.height << std::endl;
  std::cout << "\tChannel 1: " << ch1_size.width <<" x " << ch1_size.height << std::endl;

  //check framerate of the grabber-thread (thread isn't started yet)
  //remember: grabberthread have to be started to get new content.
  //          or call CameraGrabber.grab() manually
  std::cout << "CameraGrabber thread default FPS : " << p_grabber->getFramerate() << std::endl;

  //----------------------------------------------------------------------------------------
  // grab first frame
  //----------------------------------------------------------------------------------------

  //grab a picture
  p_grabber->grab();


  //the first frame is now grabbed and could be read
  //always use the QReadWriteLock for locking the cv::Mat image object on access
  QReadWriteLock* p_lock = p_grabber->getReadWriteLockPointer();

  //the local image buffer
  cv::Mat frame0;
  cv::Mat frame1;

  //get the picture from the grabber
  p_lock->lockForRead();
  frame0 = p_grabber->getImage();
  frame1 = p_grabber->getImage(1);
  p_lock->unlock();

  //copy both channels to one picture
  unsigned int size_w = frame0.cols + frame1.cols+5;
  unsigned int size_h = frame0.rows > frame1.rows ? frame0.rows : frame1.rows;
  cv::Mat stereo_pic =  cv::Mat::zeros(size_h, size_w, CV_8UC3);
  cv::Rect left_pic_size(0,0,frame0.cols,frame0.rows);
  cv::Mat left_pic = stereo_pic(left_pic_size);
  cv::Rect right_pic_size(frame0.cols+5,0,frame1.cols,frame1.rows);
  cv::Mat right_pic = stereo_pic(right_pic_size);
  frame0.copyTo(left_pic);
  frame1.copyTo(right_pic);

  //----------------------------------------------------------------------------------------
  // Create a cedar::aux::gui ImagePlot widget to show grabbed frames
  //----------------------------------------------------------------------------------------

  QApplication app(argc, argv);
  cedar::aux::gui::ImagePlotPtr p_plot = cedar::aux::gui::ImagePlotPtr(new cedar::aux::gui::ImagePlot());
  cedar::aux::MatDataPtr p_data = cedar::aux::MatDataPtr(new cedar::aux::MatData(stereo_pic));
  p_plot->plot(p_data,window_title);
  p_plot->setWindowTitle(QString::fromStdString(window_title));
  p_plot->show();
  p_plot->resize(stereo_pic.cols,stereo_pic.rows);

  //process events of the ImagePlot widget
  processQtEvents();

  //----------------------------------------------------------------------------------------
  //start the grabber-thread for updating camera images with 30 fps
  //----------------------------------------------------------------------------------------
  //read the video-files with 30 fps. If the video is slower, then grabber-steps will be bypassed
  //on creation, the framerate will be set, to the value read from the video-file
  p_grabber->setFramerate(30);
  std::cout << "Start grabbing in the background" << std::endl;
  p_grabber->start();

  //get frames
  std::cout << "\nDisplay camera frames\n";
  unsigned int counter=0;
  while (!frame0.empty() && p_plot->isVisible())
  {
    //process events of the ImagePlot widget
    processQtEvents();

    //get new images, this is independent from camera-thread
    //if camera-thread is faster than your processing, images will be skipped
    p_lock->lockForRead();
    p_data->lockForWrite();
    frame0 = p_grabber->getImage();
    frame1 = p_grabber->getImage(1);

    //copy to stereo picture for plot
    frame0.copyTo(left_pic);
    frame1.copyTo(right_pic);
    p_data->unlock();
    p_lock->unlock();

    // state messages
    if (! (++counter % 200))
    {
      //display real reached fps
      std::cout << "Thread FPS: " << p_grabber->getMeasuredFramerate() << std::endl;
    }

    // change to 100 ms or 1 second
    cedar::aux::sleep(cedar::unit::Time(1.0 * cedar::unit::milli * cedar::unit::second));
  }


  //----------------------------------------------------------------------------------------
  // save configuration. this step is optional.
  //----------------------------------------------------------------------------------------
  //p_grabber->writeJson(CONFIG_FILE_NAME);

  //----------------------------------------------------------------------------------------
  //clean up
  //----------------------------------------------------------------------------------------

  //stop grabbing-thread if running
  //recording will also be stopped
  if (p_grabber->isRunningNolocking())
  {
    p_grabber->stop();
  }
  std::cout << "finished\n";

  // p_grabber is deleted in the shared-pointer class
  return 0;
}
