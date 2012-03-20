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
#include <QReadWriteLock>


//--------------------------------------------------------------------------------------------------------------------
// grabber config
//--------------------------------------------------------------------------------------------------------------------

// grab from device number on the bus (as an integer)
#define CHANNEL_0_DEVICE 0
//#define CHANNEL_0_DEVICE 197055

// finish the initialization, i.e. grab the first image on creation of the grabber
// in this case, it isn't possible to set camera settings (like mode, fps, iso-speed)
#define FINISH_INITIALIZATION true

// channel_0_device is the bus-id or the guid
#define IS_GUID false

// the name of the grabber
// only used in the configuration file
#define GRABBER_NAME "Camera_Grabber_TestCase"

// the basename for the configuration file
// overall there are more config.files, this one for the grabber itself (call it the main configuration),
// one for each channel for the camera-capabilities (it's name is stored in the main config.)
// one for the actual setting of the camera (it's name is derived from the main config.filename
// with an appendix (_ch0, _ch1, ...)
#define CONFIG_FILE_NAME "camera_grabber_simple_testcase.config"


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
                                                                     CONFIG_FILE_NAME,
                                                                     CHANNEL_0_DEVICE,
                                                                     IS_GUID,
                                                                     FINISH_INITIALIZATION
                                                                   );
  }
  catch (cedar::aux::exc::InitializationException &e)
  {
    //after an InitializationExeception the video_grabber class isn't initialized correctly
    //and couldn't be used
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
   *      - all settings available through the setCameraSetting() functions
   *
   *
   * EITHER:
   *  No or new configuration file:
   *  - loopedThread isn't running (startGrabber threaded background grabbing with startGrabber() )
   *  - grabbername is set to default, i.e. CameraGrabber
   *
   * OR:
   *  Parameters loaded from configfile
   *  - thread isn't running
   *  - grabbername is restored from configfile
   *
   * REMARKS:
   *  For a firewire camera a configuration file for the capabilities of the used camera is needed.
   *  If there is no such file, a new one will be created (e.g. "camera_123456.capabilities" or
   *  "camera_0.capabilities where 0 is the bus-id and 123456 is the guid of the camera).
   *  In this file there are only default values stored. Please update this file
   *  with the right parameters from your camera.
   *  If there is an other file you want to use for the capabilities, set the filename in the configuration
   *  file under ch0_cameraCapabilityFileName where 0 is the channel number
   *
   *  To read the capabilities from a camera you can use the
   *  tools "fwCamInfo", "fwBusInfo" or "saveFwCamFeatures" from the directory cedar/bin/
   *
   */

  //first lets set a custom name for our grabber. This name will be used in the configuration file
  camera_grabber->setName(GRABBER_NAME);
  std::cout << "\nGrab channel 0 from \"" << camera_grabber->getSourceInfo()<< "\"" << std::endl;

  //------------------------------------------------------------------------------------
  //in this example initialization is finished (i.e. the first picture is already grabbed)
  //so we can't adjust the camera mode, fps or iso-speed anymore. The default values (or the ones from
  //an existing configuration file) are used

  std::cout << "Default settings:" << std::endl;
  std::cout << "\tCamera Mode :\t" << (int) camera_grabber->getCameraMode(0);
  cedar::dev::sensors::visual::CameraVideoMode::Id mode = camera_grabber->getCameraMode(0);
  std::cout << " (" << cedar::dev::sensors::visual::CameraVideoMode::type().get(mode).name() << ")" << std::endl;

  std::cout << "\tCamera FPS  :\t" << camera_grabber->getCameraFps(0);
  cedar::dev::sensors::visual::CameraFrameRate::Id fps = camera_grabber->getCameraFps(0);
  std::cout << " (" << cedar::dev::sensors::visual::CameraFrameRate::type().get(fps).name() << ")" << std::endl;

  std::cout << "\tCamera ISO-Speed :\t" << camera_grabber->getCameraIsoSpeed(0);
  cedar::dev::sensors::visual::CameraIsoSpeed::Id iso_speed = camera_grabber->getCameraIsoSpeed(0);
  std::cout << " (" << cedar::dev::sensors::visual::CameraIsoSpeed::type().get(iso_speed).name() << ")" << std::endl;


  //todo getCameraFrameSize

  //read properties of camera
  std::cout << "\n\nChannel 0 properties: Values of -1 means, that the feature \n"
            << "is not supported or not readable by the actual used camera" << std::endl;

  //use the enum-types from cedar to get information about the used cameras
  std::cout << "\tBrightness: " << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_BRIGHTNESS) << std::endl;
  std::cout << "\tSaturation: " << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_SATURATION) << std::endl;
  std::cout << "\tHue:        " << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_HUE) << std::endl;
  std::cout << "\tGain:       " << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_GAIN) << std::endl;
  std::cout << "\tGamma:      " << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_GAMMA) << std::endl;
  std::cout << "\tTemperature:" << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_TEMPERATURE) << std::endl;

  std::cout << "\n\tZoom:       " << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_ZOOM) << std::endl;
  std::cout << "\tFocus:      " << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_FOCUS) << std::endl;

  std::cout << "\n\tExposure:      " << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_EXPOSURE) << std::endl;
  std::cout << "\tAuto-Exposure: " << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_AUTO_EXPOSURE) << std::endl;

  std::cout << "\tTrigger:       " << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_TRIGGER) << std::endl;
  std::cout << "\tTrigger-Delay: " << camera_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_TRIGGER_DELAY) << std::endl;

  std::cout << "\n--------------------------------------------------------------------------------\n" << std::endl;

  cedar::dev::sensors::visual::CameraProperty::Id prop_id_1;
  prop_id_1 = cedar::dev::sensors::visual::CameraProperty::PROP_BRIGHTNESS;
  bool test = camera_grabber->setCameraProperty(prop_id_1, 100);
  std::cout << "Set " << cedar::dev::sensors::visual::CameraProperty::type().get(prop_id_1).name()
            << " to " << 100
            << ": Result: " << std::boolalpha << test << std::endl;
  std::cout << "Value after: " << camera_grabber->getCameraProperty(prop_id_1) << std::endl;



  //------------------------------------------------------------------
  //Create an OpenCV highgui window to show grabbed frames
  namedWindow(highgui_window_name,CV_WINDOW_KEEPRATIO);

  //the first frame is already grabbed and could be read
  //always use the QReadWriteLock for locking the cv::Mat image object
  //on access
  QReadWriteLock * pReadWriteLock;
  pReadWriteLock = camera_grabber->getReadWriteLockPointer();

  pReadWriteLock->lockForRead();
  cv::Mat frame = camera_grabber->getImage();
  pReadWriteLock->unlock();

  //startGrabber the grabber-thread for updating camera images
  //camera_grabber->setFps(15);
  std::cout << "Start grabbing in the background" << std::endl;
  camera_grabber->startGrabber();

  //startGrabber recording
  //std::cout << "\nStart Recording\n";
  //camera_grabber->startRecording(15);

  unsigned int counter=0;
  std::cout << "\nDisplay camera frames\n";

  //get frames for a while
  while (!frame.empty())
  {
    imshow(highgui_window_name,frame);

    //get new images, this is independent from camera-thread
    //if camera-thread is faster than your processing, images will be skipped
    pReadWriteLock->lockForRead();
    frame = camera_grabber->getImage();
    pReadWriteLock->unlock();

    counter++;

    //exit after 5 seconds
    if (counter == 500)
    {
      break;
    }

    //every second
    if (! (counter % 100))
    {
      //display real reached fps
      std::cout << "Thread FPS: " << camera_grabber->getFpsMeasured() << std::endl;
    }

    //wait 10ms (needed for highgui) => get images from thread with 100 fps
    waitKey(10);
  }

  //------------------------------------------------------------------
  //clean up

  //stopGrabber grabbing-thread if running
  //recording will also be stopped
  if (camera_grabber->isRunning())
  {
    camera_grabber->stopGrabber();
  }

  destroyWindow(highgui_window_name);
  //camera_grabber->writeConfiguration();

  if (camera_grabber)
  {
    delete camera_grabber;
  }

  std::cout << "finished\n";

  return 0;
}
