/*========================================================================================================================

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
#include "cedar/devices/sensors/visual/CameraGrabber.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <boost/lexical_cast.hpp>

//--------------------------------------------------------------------------------------------------------------------
// main test program
//--------------------------------------------------------------------------------------------------------------------

using namespace cv;


int main(int , char **)
{

  // grab from device number on the bus (as an integer)
  const int CHANNEL_0_DEVICE = 0;
  const int CHANNEL_1_DEVICE = 1;

  // finish the initialization, i.e. grab the first image on creation of the grabber
  // in this case, it isn't possible to set camera settings (like mode, fps, iso-speed)
  const bool  FINISH_INITIALIZATION = true;

  // channel_0_device is the bus-id or the guid
  const bool  IS_GUID = false;

  // the name of the grabber
  // only used in the configuration file
  const std::string GRABBER_NAME = "Camera_Grabber_TestCase";

  // the basename for the configuration file
  // overall there are more config.files, this one for the grabber itself (call it the main configuration),
  // one for each channel for the camera-capabilities (it's name is stored in the main config.)
  // one for the actual setting of the camera (it's name is derived from the main config.filename
  // with an appendix (_ch0, _ch1, ...)
  const std::string CONFIG_FILE_NAME = "camera_grabber_simple_testcase.config";


  //title of highgui window
  std::string highgui_window_name_0 = "0: "+GRABBER_NAME + ": " + boost::lexical_cast<std::string>(CHANNEL_0_DEVICE);
  std::string highgui_window_name_1 = "1: "+GRABBER_NAME + ": " + boost::lexical_cast<std::string>(CHANNEL_1_DEVICE);


  std::cout << "\n\nInteractive test of the CameraGrabber class (stereo)\n";
  std::cout << "--------------------------------------------\n\n";


  //------------------------------------------------------------------
  //Create the grabber
  std::cout << "Create a CameraGrabber:\n";
  cedar::dev::sensors::visual::CameraGrabber *camera_grabber=NULL;
  try
  {
    camera_grabber = new cedar::dev::sensors::visual::CameraGrabber
                         (
                           CONFIG_FILE_NAME,
                           CHANNEL_0_DEVICE,
                           CHANNEL_1_DEVICE,
                           IS_GUID,
                           FINISH_INITIALIZATION
                         );
  }
  catch (cedar::aux::exc::InitializationException &e)
  {
    //after an InitializationExeception the video_grabber class isn't initialized correctly
    //and can't be used
    std::cout << "Error on creation of the VideoGrabber class:\n"
              << e.exceptionInfo() << std::endl;

    if (camera_grabber)
    {
      delete camera_grabber;
    }

    return -1;
  }

  //------------------------------------------------------------------
  /*After initialization of a CameraGrabber:
   *
   * ALWAYS:
   *  - the first frame is already grabbed, so you can check the size or display the image
   *    getImage(), getSize() or something else
   *
   * EITHER:
   *  No or new configuration file:
   *  - loopedThread isn't running (startGrabber auto-grabbing with startGrabber() )
   *  - grabbername is set to default, i.e. CameraGrabber
   *
   * OR:
   *  Parameters loaded from configfile
   *  - thread isn't running
   *  - grabbername is restored from configfile
   */

  camera_grabber->setName(GRABBER_NAME);
  std::cout << "\nGrab channel 0 from \"" << camera_grabber->getSourceInfo()<< "\"" << std::endl;
  std::cout << "\nGrab channel 1 from \"" << camera_grabber->getSourceInfo(1)<< "\"" << std::endl;

  //Size of camera images
  std::cout << "\nSize of loaded frame:\n";
  cv::Size ch0_size = camera_grabber->getSize(0);
  cv::Size ch1_size = camera_grabber->getSize(1);
  std::cout << "\tChannel 0: " << ch0_size.width <<" x " << ch0_size.height << std::endl;
  std::cout << "\tChannel 1: " << ch1_size.width <<" x " << ch1_size.height << std::endl;

  //check framerate of the grabber-thread (thread isn't started yet)
  //remember: grabberthread have to be started to get new content.
  //          or call CameraGrabber.grab() manually
  std::cout << "CameraGrabber thread default FPS : " << camera_grabber->getFps() << std::endl;

  //------------------------------------------------------------------
  //Create an OpenCV highgui window to show grabbed frames
  std::cout << "\nDisplay cameras\n";
  cv::namedWindow(highgui_window_name_0,CV_WINDOW_KEEPRATIO);
  cv::namedWindow(highgui_window_name_1,CV_WINDOW_KEEPRATIO);

  //always use the QReadWriteLock for locking the cv::Mat image object
  //on access
  QReadWriteLock *p_lock;
  p_lock = camera_grabber->getReadWriteLockPointer();

  //the first frame is already grabbed on initialization
  p_lock->lockForRead();
  cv::Mat frame0 = camera_grabber->getImage();
  cv::Mat frame1 = camera_grabber->getImage(1);
  p_lock->unlock();

  unsigned int size_w = frame0.cols + frame1.cols+5;
  unsigned int size_h = frame0.rows > frame1.rows ? frame0.rows : frame1.rows;
  cv::Mat pic =  cv::Mat::zeros(size_h, size_w, CV_8UC3);

  cv::Rect left_pic_size(0,0,frame0.cols,frame0.rows);
  cv::Mat left_pic = pic(left_pic_size);
  cv::Rect right_pic_size(frame0.cols+5,0,frame1.cols,frame1.rows);
  cv::Mat right_pic = pic(right_pic_size);

  frame0.copyTo(left_pic);
  frame1.copyTo(right_pic);



  //startGrabber the grabber-thread for updating camera images
  camera_grabber->startGrabber();

  //get frames for a while
  unsigned int counter=0;

  while (!frame0.empty())
  {
    cv::imshow(highgui_window_name_0,pic);

    //get new images, this is independent from camera-thread
    //if camera-thread is faster than your processing, images will be skipped
    p_lock->lockForRead();
    frame0 = camera_grabber->getImage();
    frame1 = camera_grabber->getImage(1);
    p_lock->unlock();

    frame0.copyTo(left_pic);
    frame1.copyTo(right_pic);
    counter++;

    //every second
    if (! (counter % 100))
    {
      //display real reached fps
      std::cout << "Thread FPS: " << camera_grabber->getFpsMeasured() << std::endl;
    }

    //exit after another second
    if (counter == 500)
    {
      break;
    }

    //wait 100ms (needed for highgui) => get images from thread with 10 fps
    waitKey(10);
  }

  //------------------------------------------------------------------
  //clean up

  cv::destroyWindow(highgui_window_name_0);
  cv::destroyWindow(highgui_window_name_1);

  //stopGrabber grabbing-thread if running
  //recording will also be stopped
  if (camera_grabber->isRunning())
  {
    camera_grabber->stopGrabber();
  }

  if (camera_grabber)
  {
   delete camera_grabber;
  }

  std::cout << "finished\n";

  return 0;
}
