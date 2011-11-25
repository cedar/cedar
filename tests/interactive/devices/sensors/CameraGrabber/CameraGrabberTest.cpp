/*========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        CameraGrabberTest.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Simple application to grab from a camera (mono-case)

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/devices/sensors/visual/CameraGrabber.h"
//#include <devices/sensors/visual/CameraGrabber.h>

// PROJECT INCLUDES

// SYSTEM INCLUDES
//#include <opencv2/opencv.hpp>
//#include <dc1394/1394.h>  //for all firewire stuff
//#include <dc1394/video.h>  //for video modes like  DC1394_FRAMERATE_15, DC1394_ISO_SPEED_400
#include <dc1394/types.h>  //for constants like DC1394_VIDEO_MODE_320x240_YUV422


//--------------------------------------------------------------------------------------------------------------------
//constants
//--------------------------------------------------------------------------------------------------------------------
#define CHANNEL_0_DEVICE (unsigned int) 0

#define GRABBER_NAME_0 "Camera_Grabber_TestCase"
#define CONFIG_FILE_NAME_0 "camera_grabber_testcase.config"


//--------------------------------------------------------------------------------------------------------------------
// main test program
//--------------------------------------------------------------------------------------------------------------------

using namespace cv;


int main(int , char **)
{
  //title of highgui window
  std::string highgui_window_name_0 = (std::string) GRABBER_NAME_0 + ": CHANNEL_0_DEVICE" ;


  std::cout << "\n\nInteractive test of the CameraGrabber class\n";
  std::cout << "--------------------------------------------\n\n";


  //------------------------------------------------------------------
  //Create the grabber
  std::cout << "Create a CameraGrabber:\n";
  cedar::dev::sensors::visual::CameraGrabber *camera_grabber=NULL;
  try
  {
    camera_grabber = new cedar::dev::sensors::visual::CameraGrabber( CONFIG_FILE_NAME_0 , CHANNEL_0_DEVICE , true);
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
   *  - loopedThread isn't running (start auto-grabbing with start() )
   *  - grabbername is set to default, i.e. CameraGrabber
   *
   * OR:
   *  Parameters loaded from configfile
   *  - thread isn't running
   *  - grabbername is restored from configfile
   */

    camera_grabber->setName(GRABBER_NAME_0);
    std::cout << "\nGrab channel 0 from \"" << camera_grabber->getSourceInfo()<< "\"" << std::endl;

    //Size of camera images
    std::cout << "\nSize of loaded frame:\n";
    cv::Size ch0_size = camera_grabber->getSize(0);
    std::cout << "\tChannel 0: " << ch0_size.width <<" x " << ch0_size.height << std::endl;

    //check framerate of the grabber-thread (thread isn't started yet)
    //remember: grabberthread have to be started to get new content.
    //          or call CameraGrabber.grab() manually
    std::cout << "CameraGrabber thread default FPS : " << camera_grabber->getFps() << std::endl;

    //You can create an avi file from your grabbing
    camera_grabber->setRecordName("Camera_Grabber_TestCase_Recording.avi");

    //It is possible to create a snaptshot of the pics.
    //In the case, you set another extenstion as in the source, the image will be converted

    //Set Snapshotnames without channel-number (in mono-case: default value is 0)
    //The type of the file (e.g. bitmap or jpg or something else) depend on extension
    //look at the documentation of setSnapshotName for details
    camera_grabber->setSnapshotName("SnapshotFromCameraGrabber_mono.bmp");

    //Check the constructed filenames
    std::cout << "Check filenames of snapshots and recordings\n" << std::endl;

    std::cout << "SnapshotName:\t" << camera_grabber->getSnapshotName() <<std::endl;
    std::cout << "RecordName:\t" << camera_grabber->getRecordName() <<std::endl;

    //enforcing an error and catch it
    try
    {
      std::cout << "\nTry to enforce an exception:\n";
      std::cout << "SnapshotName_1: " << camera_grabber->getSnapshotName(1) <<std::endl;
    }
    catch (cedar::aux::exc::ExceptionBase& e)
    {
      //std::cout << "Exception: " <<e.what() << std::endl; //until now: buggy cedar implementation
      //all grabber exceptions (look at cedar/devices/sensors/visual/exceptions/namespace.h)
      //can be used without separate include-files
      std::cout <<e.exceptionInfo()<<std::endl;
    }

    // Save a snapshot from channel 0 only
    // Be aware, that existing files will be overwritten without any question
    //camera_grabber->saveSnapshot(0);

    std::cout << "Channel 0 settings:" << std::endl;
    std::cout << "\tContrast:    " << camera_grabber->getCameraProperty(CV_CAP_PROP_CONTRAST) << std::endl;
    std::cout << "\tBrightness:  " << camera_grabber->getCameraProperty(CV_CAP_PROP_BRIGHTNESS) << std::endl;
    std::cout << "\tSaturation:  " << camera_grabber->getCameraProperty(CV_CAP_PROP_SATURATION) << std::endl;
    std::cout << "\tHue:         " << camera_grabber->getCameraProperty(CV_CAP_PROP_HUE) << std::endl;
    std::cout << "\tGain:        " << camera_grabber->getCameraProperty(CV_CAP_PROP_GAIN) << std::endl;
    std::cout << "\tExposure:    " << camera_grabber->getCameraProperty(CV_CAP_PROP_EXPOSURE) << std::endl;
    std::cout << "\n\tFourCC(Enc): " << camera_grabber->getCameraProperty(CV_CAP_PROP_FOURCC) << std::endl;
    std::cout << "\tCamera Mode  : " << camera_grabber->getCameraProperty(CV_CAP_PROP_MODE) << std::endl;
    std::cout << "\tCamera FPS  : " << camera_grabber->getCameraProperty(CV_CAP_PROP_FPS) << std::endl;

  //------------------------------------------------------------------
  //Create an OpenCV highgui window to show grabbed frames
  std::cout << "\nDisplay camera\n";
  namedWindow(highgui_window_name_0,CV_WINDOW_KEEPRATIO);

  //the first frame is already grabbed on initialization
  cv::Mat frame0 = camera_grabber->getImage();

  //start the grabber-thread for updating camera images
  camera_grabber->setFps(15);
  std::cout << "Start grabbing in the background" << std::endl;
  camera_grabber->start();

  //start recording
  //std::cout << "\nStart Recording\n";
  //camera_grabber->startRecording(15);

  //get frames for a while
  unsigned int counter=0;

  while (!frame0.empty())
  {
    imshow(highgui_window_name_0,frame0);

    //get new images, this is independent from camera-thread
    //if camera-thread is faster than your processing, images will be skipped
    frame0 = camera_grabber->getImage();
    counter++;

    //every 100 ms
    if (! (counter % 10)) {
    }

    //every second
    if (! (counter % 100)) {
      std::cout << "Thread FPS: " << camera_grabber->getFpsMeasured() << std::endl;
    }



    //after one second, set camera options
    if (counter == 100)
    {
      camera_grabber->setCameraProperty(CV_CAP_PROP_BRIGHTNESS, 10);
      double brightness = camera_grabber->getCameraProperty(CV_CAP_PROP_BRIGHTNESS);
      std::cout << "Brightness: " << brightness << std::endl;
    }


    //after another second, set camera options
    if (counter == 200)
    {
      camera_grabber->setCameraProperty(CV_CAP_PROP_BRIGHTNESS, 200);
      double brightness = camera_grabber->getCameraProperty(CV_CAP_PROP_BRIGHTNESS);
      std::cout << "Brightness: " << brightness << std::endl;
    }

    if (counter == 300)
    {

      std::cout <<"Set capture size: "<<std::endl;

      cv::Size ch0_size = camera_grabber->getSize(0);
      std::cout << "\tOld size: " << ch0_size.width <<" x " << ch0_size.height << std::endl;

      /* cv::Size newSize;
      newSize.width = 320;
      newSize.height = 240;
      camera_grabber->setCameraParamSize(0, newSize);
      */

      //does nothing
      camera_grabber->setCameraProperty(0,CV_CAP_PROP_MODE,DC1394_VIDEO_MODE_320x240_YUV422);


      //camera_grabber->setCameraParamMode(0,(double)2);
      std::cout << "\tCamera Mode  : " << camera_grabber-> getCameraProperty(CV_CAP_PROP_MODE) << std::endl;

      ch0_size = camera_grabber->getSize(0);
      std::cout << "\tNew Size: " << ch0_size.width <<" x " << ch0_size.height << std::endl;
    }

    if (counter == 400)
    {
      std::cout << "Set Fps to 7.5 FPS" << std::endl;
      std::cout << "\tOld Camera FPS  : " << camera_grabber->getCameraProperty(CV_CAP_PROP_FPS) << std::endl;
      camera_grabber->setCameraProperty(CV_CAP_PROP_FPS,30);
      //camera_grabber->setFps(7.5);
      std::cout << "\tNew Camera FPS  : " << camera_grabber->getCameraProperty(CV_CAP_PROP_FPS) << std::endl;
    }

    //exit after 10 seconds
    if (counter == 1000)
    {
      break;
    }

    //wait 10ms (needed for highgui) => get images from thread with 10 fps
    waitKey(10);
  }

  //------------------------------------------------------------------
  //clean up

  destroyWindow(highgui_window_name_0);

  //stop grabbing-thread if running
  //recording will also be stopped
  if (camera_grabber->isRunning())
  {
    camera_grabber->stop();
  }

  if (camera_grabber)
  {
    delete camera_grabber;
  }

  std::cout << "finished\n";

  return 0;
}
