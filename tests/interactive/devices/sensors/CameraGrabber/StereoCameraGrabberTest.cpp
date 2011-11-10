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
#include "cedar/auxiliaries/LogFile.h"
#include "cedar/devices/sensors/visual/CameraGrabber.h"
//#include <devices/sensors/visual/CameraGrabber.h>
//#include <auxiliaries/LogFile.h>

// PROJECT INCLUDES


// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>

//--------------------------------------------------------------------------------------------------------------------
//constants
//--------------------------------------------------------------------------------------------------------------------
#define CHANNEL_0_DEVICE 0
#define CHANNEL_1_DEVICE 1

#define GRABBER_NAME_0 "Stereo_Camera_Grabber_TestCase"
#define CONFIG_FILE_NAME_0 "stereo_camera_grabber_testcase.config"


//--------------------------------------------------------------------------------------------------------------------
// main test program
//--------------------------------------------------------------------------------------------------------------------

using namespace cv;


int main(int , char **)
{
  //title of highgui window
  std::string highgui_window_name_0 = (std::string) GRABBER_NAME_0 + ": CHANNEL_0_DEVICE" ;
  std::string highgui_window_name_1 = (std::string) GRABBER_NAME_0 + ": CHANNEL_1_DEVICE" ;


  std::cout << "\n\nInteractive test of the CameraGrabber class\n";
  std::cout << "--------------------------------------------\n\n";


  //------------------------------------------------------------------
  //Create the grabber
  std::cout << "Create a CameraGrabber:\n";
  cedar::dev::sensors::visual::CameraGrabber *camera_grabber=NULL;
  try
  {
    camera_grabber = new cedar::dev::sensors::visual::CameraGrabber(
                          CONFIG_FILE_NAME_0,
                          CHANNEL_0_DEVICE,
                          CHANNEL_1_DEVICE
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

    //Set the name for the recording file
    //This function have special cases
    //In the mono case, filename is used without changes
    //In the stereo case, the filenames are constructed like
    //filename[ch0].ext and filename[ch1].ext
    //If you want to set the names independently, you can use it with the camera-parameter
    //Start recording with startRecording() (thread will be started on startRecording!)
    camera_grabber->setRecordName("Camera_Grabber_TestCase_Recording.avi");
    //record-filenames now set to Camera_Grabber_TestCase_Recording[ch0].avi
    //and Camera_Grabber_TestCase_Recording[ch1].avi


    //Set Snapshotnames without channel-number (in mono-case: default value is 0)
    //This is the same behavior as setRecordName()
    //The type of the file (e.g. bitmap or jpg or something else) depends on extension
    //look at the documentation of setSnapshotName for details
    // SnapshotFromCameraGrabber[ch0].bmp and SnapshotFromCameraGrabber[ch1].bmp
    camera_grabber->setSnapshotName("SnapshotFromCameraGrabber.bmp");

    // rename channel 0 to SnapshotFromCameraGrabber.jpg. Channel 1 isn't altered
    camera_grabber->setSnapshotName(0,"SnapshotFromCameraGrabber01.jpg");


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


  //------------------------------------------------------------------
  //Create an OpenCV highgui window to show grabbed frames
  std::cout << "\nDisplay cameras\n";
  namedWindow(highgui_window_name_0,CV_WINDOW_KEEPRATIO);
  namedWindow(highgui_window_name_1,CV_WINDOW_KEEPRATIO);

  //the first frame is already grabbed on initialization
  cv::Mat frame0 = camera_grabber->getImage();
  cv::Mat frame1 = camera_grabber->getImage();

  //start the grabber-thread for updating camera images
  camera_grabber->setFps(20);
  camera_grabber->start();

  //start recording
  std::cout << "\nStart Recording\n";
  camera_grabber->startRecording(20);

  //get frames for a while
  unsigned int counter=0;

  while (!frame0.empty())
  {
    imshow(highgui_window_name_0,frame0);
    imshow(highgui_window_name_1,frame1);

    //get new images, this is independent from camera-thread
    //if camera-thread is faster than your processing, images will be skipped
    frame0 = camera_grabber->getImage();
    frame1 = camera_grabber->getImage(1);
    counter++;

    //after one second, set camera options
    if (counter == 100)
    {

    }

    //exit after another second
    if (counter == 200)
    {
      break;
    }

    //wait 100ms (needed for highgui) => get images from thread with 10 fps
    waitKey(10);
  }

  //stop grabbing-thread if running
  if (camera_grabber->isRunning())
  {
    camera_grabber->stop();
  }

  //------------------------------------------------------------------
  //clean up highgui
  destroyWindow(highgui_window_name_0);
  destroyWindow(highgui_window_name_1);
  std::cout << "finished\n";

  return 0;
}
