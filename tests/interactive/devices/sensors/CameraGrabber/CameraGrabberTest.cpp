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


// PROJECT INCLUDES

// SYSTEM INCLUDES


//--------------------------------------------------------------------------------------------------------------------
//constants
//--------------------------------------------------------------------------------------------------------------------
#define CHANNEL_0_DEVICE 0
//#define CHANNEL_0_DEVICE 197055
#define FINISH_INITIALIZATION false
#define IS_GUID false

#define GRABBER_NAME "Camera_Grabber_TestCase"
#define CONFIG_FILE_NAME_0 "camera_grabber_testcase.config"


//--------------------------------------------------------------------------------------------------------------------
// main test program
//--------------------------------------------------------------------------------------------------------------------

using namespace cv;


int main(int , char **)
{
  //title of highgui window
  std::string highgui_window_name = (std::string) GRABBER_NAME + ": CHANNEL_0_DEVICE" ;


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
   *  - finishInitialization is true:
   *      - the first frame is already grabbed, so you can check the size or display the image
   *        getImage(), getSize() or something else
   *  - finishInitialization is false:
   *      - you can adjust the camera with following functions:
   *         setCameraFps()
   *         setCameraMode()
   *         setCameraIsoSpeed() (for Firewire cameras)
   *         or all Settings available with setCameraSetting()
   *
   *
   *
   * EITHER:
   *  No or new configuration file:
   *  - loopedThread isn't running (start threaded background grabbing with start() )
   *  - grabbername is set to default, i.e. CameraGrabber
   *
   * OR:
   *  Parameters loaded from configfile
   *  - thread isn't running
   *  - grabbername is restored from configfile
   */

    //a new name for our grabber
    camera_grabber->setName(GRABBER_NAME);
    std::cout << "\nGrab channel 0 from \"" << camera_grabber->getSourceInfo()<< "\"" << std::endl;

    //in this example initialization isn't finished (i.e. the first picture isn't already grabbed)


    //------------------------------------------------------------------------------------
    //so we can adjust the camera

    //these settings are only setable on initialization:
    std::cout << "\tCamera Mode : " << (int) camera_grabber->getCameraMode(0) << std::endl;
    cedar::dev::sensors::visual::CameraVideoMode::Id mode = camera_grabber->getCameraMode(0);
    std::cout << "\tCamera Mode : " << cedar::dev::sensors::visual::CameraVideoMode::type().get(mode).name() << std::endl;

    std::cout << "\tCamera FPS  : " << camera_grabber->getCameraFps(0) << std::endl;
    cedar::dev::sensors::visual::CameraFrameRate::Id fps = camera_grabber->getCameraFps(0);
    std::cout << "\tCamera FPS  : " << cedar::dev::sensors::visual::CameraFrameRate::type().get(fps).name() << std::endl;

    cv::Size size = camera_grabber->getCameraFrameSize(0);
    std::cout << "\tChannel 0: " << size.width <<" x " << size.height << std::endl;

    std::cout << "\tCamera ISO-Speed    : " << camera_grabber->getCameraIsoSpeed(0) << std::endl;
    cedar::dev::sensors::visual::CameraIsoSpeed::Id iso_speed = camera_grabber->getCameraIsoSpeed(0);
    std::cout << "\tCamera ISO-Speed    :" << cedar::dev::sensors::visual::CameraIsoSpeed::type().get(iso_speed).name() << std::endl;

    //Set the wanted mode for grabbing
    std::cout << "\n\nSet camera Fps to 7.5 FPS" << std::endl;
    camera_grabber->setCameraFps(0,cedar::dev::sensors::visual::CameraFrameRate::FRAMERATE_7_5);
    std::cout << "Set ISO-speed to 200" << std::endl;
    camera_grabber->setCameraIsoSpeed(0,cedar::dev::sensors::visual::CameraIsoSpeed::ISO_200);
    std::cout << "Set camera mode to \"MODE_160x120_YUV444\"" << std::endl;
    //set camera mode to DC1394_VIDEO_MODE_160x120_YUV444 (is supported by sony firewire cam
    camera_grabber->setCameraMode(0,cedar::dev::sensors::visual::CameraVideoMode::MODE_FW_160x120_YUV444);


    //This finish the initialization.
    //After this, there is no possibility to change the framerate or the camera mode
    camera_grabber->grab();

    std::cout << "\nChannel 0 settings:" << std::endl;
    std::cout << "\tCamera Mode      : " << cedar::dev::sensors::visual::CameraVideoMode::type().get(camera_grabber->getCameraMode(0)).name() << std::endl;
    std::cout << "\tCamera FPS       : " << cedar::dev::sensors::visual::CameraFrameRate::type().get(camera_grabber->getCameraFps(0)).name() << std::endl;
    std::cout << "\tCamera ISO-Speed : " << cedar::dev::sensors::visual::CameraIsoSpeed::type().get(camera_grabber->getCameraIsoSpeed(0)).name() << std::endl;


    std::cout << "\n\nChannel 0 properties: Values of -1 means, that the feature is not supported by the actual used camera" << std::endl;

    //it is possible to use opencv constants.
    //std::cout << "\tContrast:    " << camera_grabber->getCameraProperty(CV_CAP_PROP_CONTRAST) << std::endl;

    //better use the enum-types from cedar
    std::cout << "\tBrightness: " << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_BRIGHTNESS) << std::endl;
    std::cout << "\tSaturation: " << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_SATURATION) << std::endl;
    std::cout << "\tHue:        " << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_HUE) << std::endl;
    std::cout << "\tGain:       " << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_GAIN) << std::endl;
    //std::cout << "\tGamma:      " << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_GAMMA) << std::endl;
    std::cout << "\tTemperature:" << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_TEMPERATURE) << std::endl;

    std::cout << "\n\tZoom:       " << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_ZOOM) << std::endl;
    std::cout << "\tFocus:      " << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_FOCUS) << std::endl;

    std::cout << "\n\tExposure:      " << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_EXPOSURE) << std::endl;
    std::cout << "\tAuto-Exposure: " << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_AUTO_EXPOSURE) << std::endl;

    std::cout << "\tTrigger:       " << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_TRIGGER) << std::endl;
    std::cout << "\tTrigger-Delay: " << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_TRIGGER_DELAY) << std::endl;

    //for all Properties, you can use a loop
    std::cout << "\n----------------------------\n" << std::endl;
    std::cout << "All properties of channel 0:" << std::endl;

    //camera_grabber->printAllSettings(0);


    int num_properties = cedar::dev::sensors::visual::CameraProperty::type().list().size();
    for (int i=0; i<num_properties; i++)
    {
      cedar::dev::sensors::visual::CameraProperty::Id prop_id;
      prop_id = cedar::dev::sensors::visual::CameraProperty::type().list().at(i).id();
      std::string prop_name = cedar::dev::sensors::visual::CameraProperty::type().get(prop_id).name();
      std::string prop_description = cedar::dev::sensors::visual::CameraProperty::type().get(prop_id).prettyString();
      int prop_value = camera_grabber->getCameraProperty(prop_id);

      std::cout << "\tId: " << prop_id
                << "\tName: " << prop_name
                << "\tValue: " << prop_value
                << "\tDescription: " << prop_description
                << std::endl;
    }


    //------------------------------------------------------------------------------------
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



    //------------------------------------------------------------------
    //grab first image. After this, settings couldn't be set any more
    std::cout << "\n----------------------------\n" << std::endl;
    std::cout << "Grab first image" << std::endl;
    camera_grabber->grab();

    //now first image is grabbed from the camera.

    // Save a snapshot from channel 0 only
    // Be aware, that existing files will be overwritten without any question
    //camera_grabber->saveSnapshot(0);


    //------------------------------------------------------------------
    //Size of camera images
    std::cout << "\nSize of loaded frame: " ;
    cv::Size ch0_size = camera_grabber->getSize(0);
    std::cout << size.width <<" x " << size.height << std::endl;

    //check framerate of the grabber-thread (thread isn't started yet)
    //remember: grabberthread have to be started to get new content.
    //          or call CameraGrabber.grab() manually
    std::cout << "CameraGrabber FPS : " << camera_grabber->getFps() << std::endl;


  //------------------------------------------------------------------
  //Create an OpenCV highgui window to show grabbed frames
  std::cout << "\nDisplay camera\n";
  namedWindow(highgui_window_name,CV_WINDOW_KEEPRATIO);

  //the first frame is already grabbed
  cv::Mat frame = camera_grabber->getImage();

  //start the grabber-thread for updating camera images
  //camera_grabber->setFps(15);
  std::cout << "Start grabbing in the background" << std::endl;
  camera_grabber->start();

  //start recording
  //std::cout << "\nStart Recording\n";
  //camera_grabber->startRecording(15);

  //get frames for a while
  unsigned int counter=0;

  while (!frame.empty())
  {
    //to reset values to default-value
    int white_balance_orig=0;

    //the wanted Property to change
    cedar::dev::sensors::visual::CameraProperty::Id prop_id;

    imshow(highgui_window_name,frame);

    //get new images, this is independent from camera-thread
    //if camera-thread is faster than your processing, images will be skipped
    frame = camera_grabber->getImage();
    counter++;

    //every second
    if (! (counter % 100)) {
      //display real reached fps
      std::cout << "Thread FPS: " << camera_grabber->getFpsMeasured() << std::endl;
    }



    //after one second, set camera options
    if (counter == 100)
    {
      prop_id = cedar::dev::sensors::visual::CameraProperty::PROP_BRIGHTNESS;

      double brightness = camera_grabber->getCameraProperty(prop_id);
      std::cout << "Brightness before: " << brightness << std::endl;
      bool test = camera_grabber->setCameraProperty(prop_id, 150);
      std::cout << "Set Brightness to 150. Result = " << std::boolalpha << test << std::endl;
      brightness = camera_grabber->getCameraProperty(prop_id);
      std::cout << "Brightness after: " << brightness << std::endl;
    }


    //after another second, set camera options
    if (counter == 200)
    {
      prop_id = cedar::dev::sensors::visual::CameraProperty::PROP_BRIGHTNESS;

      double brightness = camera_grabber->getCameraProperty(prop_id);
      std::cout << "Brightness before: " << brightness << std::endl;
      bool test = camera_grabber->setCameraProperty(prop_id, 100);
      std::cout << "Set Brightness to 100. Result = " << test << std::endl;
      brightness = camera_grabber->getCameraProperty(prop_id);
      std::cout << "Brightness after: " << brightness << std::endl;
    }

    if (counter == 300)
    {
      //wanted id to change
      prop_id = cedar::dev::sensors::visual::CameraProperty::PROP_WHITE_BALANCE_BLUE_U;

      //check old value with new one
      white_balance_orig = static_cast<int>(camera_grabber->getCameraProperty(prop_id));
      std::cout << "White Balance: " << white_balance_orig << std::endl;
      bool test = camera_grabber->setCameraProperty(prop_id, 10);
      std::cout << "Set White Balance to 10. Result = " <<  test << std::endl;
      double white_balance = camera_grabber->getCameraProperty(prop_id);
      std::cout << "White Balance after: " << white_balance << std::endl;
    }

    if (counter == 400)
    {
      prop_id = cedar::dev::sensors::visual::CameraProperty::PROP_EXPOSURE;

      bool test = camera_grabber->setCameraProperty(prop_id, CAMERA_PROPERTY_MODE_AUTO);
      std::cout << "Set White Balance to AUTO. Result = " <<  test << std::endl;
      double white_balance = camera_grabber->getCameraProperty(prop_id);
      std::cout << "White Balance after: " << white_balance << std::endl;
    }

    //exit after 5 seconds
    if (counter == 500)
    {
      break;
    }

    //wait 10ms (needed for highgui) => get images from thread with 100 fps
    waitKey(10);
  }

  //------------------------------------------------------------------
  //clean up

  destroyWindow(highgui_window_name);

  //save actual configuration
  camera_grabber->writeConfiguration();

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
