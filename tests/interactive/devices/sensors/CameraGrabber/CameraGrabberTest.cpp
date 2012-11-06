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
#include <boost/lexical_cast.hpp>



int main(int , char **)
{

  // grab from device number on the bus (as an integer)
  const int CHANNEL_0_DEVICE = 0;
  //  const int CHANNEL_0_DEVICE = 197055;

  // channel_0_device is the bus-id or the guid
  const bool  IS_GUID = false;

  // the name of the grabber
  // only used in the configuration file
  const std::string GRABBER_NAME = "Grabber_TestCase";

  // the basename for the configuration file
  const std::string CONFIG_FILE_NAME = "grabber_simple_testcase.config";

  //title of highgui window
  std::string highgui_window_name = GRABBER_NAME + ": " + boost::lexical_cast<std::string>(CHANNEL_0_DEVICE);


  std::cout << "\n\nInteractive test of the CameraGrabber class\n";
  std::cout << "--------------------------------------------\n\n";


  //------------------------------------------------------------------
  //Create the grabber
  std::cout << "Create a CameraGrabber:\n";
  cedar::dev::sensors::visual::CameraGrabberPtr p_grabber;
  try
  {
    p_grabber = cedar::dev::sensors::visual::CameraGrabberPtr
                (
                  new cedar::dev::sensors::visual::CameraGrabber(CHANNEL_0_DEVICE,IS_GUID)
                );
  }
  catch (cedar::aux::InitializationException &e)
  {
    //after an InitializationExeception the grabber class isn't initialized correctly
    //and couldn't be used
    std::cout << "Error on creation of the VideoGrabber class:\n"
              << e.exceptionInfo() << std::endl;

    //delete p_grabber is done via the shared-pointer class
    return -1;
  }

  //----------------------------------------------------------------------------------------
  // load configuration or apply settings manual.
  // this step is optional.
  //----------------------------------------------------------------------------------------
  try
  {
    //p_grabber->readJson(CONFIG_FILE_NAME);
  }
  catch (...)
  {
    std::cout << "No configuration exists!" << std::endl;
  }

  //only available for firewire cameras:
  p_grabber->setCameraIsoSpeed(0,cedar::dev::sensors::visual::CameraIsoSpeed::ISO_200);

  // you have to check if the framerate is supported by the used camera
  // on firewirecameras only suppertd framerates could be set
  p_grabber->setCameraFps(0,cedar::dev::sensors::visual::CameraFrameRate::FPS_15);



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

  //------------------------------------------------------------------
  /*After initialization of a CameraGrabber:
   *
   * No frames are loaded from the camera into the videobuffer. You have to do it manually
   * with the grab() method
   *
   * WITHOUT configuration file (readJson):
   *  - loopedThread isn't running (startGrabber threaded background grabbing with startGrabber() )
   *  - grabbername is set to default, i.e. CameraGrabber
   *  - the first found camera is used with default backend parameter values
   *
   * WITH configuration file:
   *  the values depends on the configuration
   */

  //first lets set a custom name for our grabber. This name will be used later in the configuration file
  p_grabber->setName(GRABBER_NAME);
  std::cout << "\nGrab channel 0 from \"" << p_grabber->getSourceInfo()<< "\"" << std::endl;


  //----------------------------------------------------------------------------------------
  // print the settings and properties from the camera
  //----------------------------------------------------------------------------------------

  std::cout << "Default settings:" << std::endl;
  std::cout << "\tCamera Mode :\t" << (int) p_grabber->getCameraVideoMode(0);
  cedar::dev::sensors::visual::CameraVideoMode::Id mode = p_grabber->getCameraVideoMode(0);
  std::cout << " (" << cedar::dev::sensors::visual::CameraVideoMode::type().get(mode).name() << ")" << std::endl;

  std::cout << "\tCamera FPS  :\t" << p_grabber->getCameraFps(0);
  cedar::dev::sensors::visual::CameraFrameRate::Id fps = p_grabber->getCameraFps(0);
  std::cout << " (" << cedar::dev::sensors::visual::CameraFrameRate::type().get(fps).name() << ")" << std::endl;

  std::cout << "\tCamera ISO-Speed :\t" << p_grabber->getCameraIsoSpeed(0);
  cedar::dev::sensors::visual::CameraIsoSpeed::Id iso_speed = p_grabber->getCameraIsoSpeed(0);
  std::cout << " (" << cedar::dev::sensors::visual::CameraIsoSpeed::type().get(iso_speed).name() << ")" << std::endl;

  //read properties of camera
  std::cout << "\n\nChannel 0 properties:\n Negative values have special meaning and belongs to the mode.\n"
            << "\tValue of " << CAMERA_PROPERTY_NOT_SUPPORTED << ": Mode not supported\n"
            << "\tValue of " << CAMERA_PROPERTY_MODE_AUTO << ": Mode AUTO\n"
            << "\tValue of " << CAMERA_PROPERTY_MODE_DEFAULT << ": Mode DEFAULT (the backend decides which value)\n"
            << std::endl;

  //use the enum-types from cedar to get information about the used cameras
  std::cout << "\tBrightness: "
    << p_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_BRIGHTNESS) << std::endl;
  std::cout << "\tSaturation: "
    << p_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_SATURATION) << std::endl;
  std::cout << "\tHue:        "
    << p_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_HUE) << std::endl;
  std::cout << "\tGain:       "
    << p_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_GAIN) << std::endl;
  std::cout << "\tGamma:      "
    << p_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_GAMMA) << std::endl;
  std::cout << "\tTemperature:"
    << p_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_TEMPERATURE) << std::endl;

  std::cout << "\tZoom:       "
    << p_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_ZOOM) << std::endl;
  std::cout << "\tFocus:      "
    << p_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_FOCUS) << std::endl;

  std::cout << "\tExposure:      "
    << p_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_EXPOSURE) << std::endl;
  std::cout << "\tAuto-Exposure: "
    << p_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_AUTO_EXPOSURE) << std::endl;

  std::cout << "\tTrigger:       "
    << p_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_TRIGGER) << std::endl;
  std::cout << "\tTrigger-Delay: "
    << p_grabber->getCameraProperty(cedar::dev::sensors::visual::CameraProperty::PROP_TRIGGER_DELAY) << std::endl;

  std::cout << "\n--------------------------------------------------------------------------------\n" << std::endl;


  //----------------------------------------------------------------------------------------
  // set property
  //----------------------------------------------------------------------------------------

  // in order to set a property, you have to switch over to manual mode for that property
  cedar::dev::sensors::visual::CameraProperty::Id prop_id;
  prop_id = cedar::dev::sensors::visual::CameraProperty::PROP_BRIGHTNESS;


  if (p_grabber->setCameraPropertyMode(0,prop_id,cedar::dev::sensors::visual::CameraPropertyMode::MANUAL))
  {
    bool test = p_grabber->setCameraProperty(prop_id, 100);
    std::cout << "Set " << cedar::dev::sensors::visual::CameraProperty::type().get(prop_id).name()
              << " to " << 100
              << ": Result: " << std::boolalpha << test << std::endl;
    std::cout << "Value after: " << p_grabber->getCameraProperty(prop_id) << std::endl;
  }


  //----------------------------------------------------------------------------------------
  // show frames
  //----------------------------------------------------------------------------------------

  //Create an OpenCV highgui window to show grabbed frames
  cv::namedWindow(highgui_window_name,CV_WINDOW_KEEPRATIO);

  //the first frame is already grabbed and could be read
  //always use the QReadWriteLock for locking the cv::Mat image object
  //on access
  QReadWriteLock* p_lock;
  p_lock = p_grabber->getReadWriteLockPointer();

  //grab a picture
  p_grabber->grab();

  //startGrabber the grabber-thread for updating camera images
  p_grabber->setFps(15);
  std::cout << "Start grabbing in the background" << std::endl;
  p_grabber->startGrabber();

  // get the image from the image buffer
  p_lock->lockForRead();
  cv::Mat frame = p_grabber->getImage();
  p_lock->unlock();

  //startGrabber recording
  //std::cout << "\nStart Recording\n";
  //p_grabber->startRecording(15);

  unsigned int counter=0;
  std::cout << "\nDisplay camera frames\n";

  //get frames for a while
  while (!frame.empty())
  {
    cv::imshow(highgui_window_name,frame);

    //get new images, this is independent from camera-thread
    //if camera-thread is faster than your processing, images will be skipped
    p_lock->lockForRead();
    frame = p_grabber->getImage();
    p_lock->unlock();

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
      std::cout << "Thread FPS: " << p_grabber->getFpsMeasured() << std::endl;
    }

    //wait 10ms (needed for highgui) => get images from thread with 100 fps
    cv::waitKey(10);
  }

  //------------------------------------------------------------------
  //clean up

  //stopGrabber grabbing-thread if running
  //recording will also be stopped
  if (p_grabber->isRunning())
  {
    p_grabber->stopGrabber();
  }

  cv::destroyWindow(highgui_window_name);
  //p_grabber->writeConfiguration();

  //delete p_grabber is done into the shared-pointer class

  std::cout << "finished\n";

  return 0;
}
