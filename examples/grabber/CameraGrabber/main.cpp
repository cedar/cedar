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
#include "cedar/devices/sensors/camera/Grabber.h"
#include "cedar/auxiliaries/gui/ImagePlot.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/sleepFunctions.h"

// SYSTEM INCLUDES
#include <QtGui/QApplication>
#include <opencv2/opencv.hpp>
#include <boost/lexical_cast.hpp>

/* Program to test the CameraGrabber class
 *
 */
int main(int argc, char* argv[])
{

  // grab from device number on the bus (as an integer)
  const int CHANNEL_0_DEVICE = 0;
  //  const int CHANNEL_0_DEVICE = 197055;

  // is CHANNEL_0_DEVICE the bus-id or the guid?
  const bool  IS_GUID = false;

  // the name of the grabber
  // only used in the configuration file
  const std::string GRABBER_NAME = "CameraGrabber_TestCase";

  // the basename for the configuration file
  const std::string CONFIG_FILE_NAME = "cameragrabber_simple_testcase.config";

  //title of highgui window
  std::string window_title = GRABBER_NAME + ": " + boost::lexical_cast<std::string>(CHANNEL_0_DEVICE);


  std::cout << "\n\nInteractive test of the CameraGrabber class\n";
  std::cout << "-------------------------------------------\n\n";


  //------------------------------------------------------------------
  //Create the grabber
  std::cout << "Create a CameraGrabber:\n";
  cedar::dev::sensors::camera::GrabberPtr p_grabber;
  try
  {
    p_grabber = cedar::dev::sensors::camera::GrabberPtr
                (
                  new cedar::dev::sensors::camera::Grabber
                      (
                        CHANNEL_0_DEVICE,
                        IS_GUID //,
                        //cedar::dev::sensors::camera::BackendType::DC1394
                      )
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

  // activate crash-handler if there is any hardware-related stuff which has to be cleaned up
   //p_grabber->installCrashHandler();

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

#ifdef CEDAR_USE_LIB_DC1394
  //only available for firewire cameras:
  p_grabber->setCameraIsoSpeed(0,cedar::dev::sensors::camera::IsoSpeed::ISO_200);
#endif // CEDAR_USE_LIB_DC1394

  // you have to check if the framerate is supported by the used camera
  // on firewirecameras only suppertd framerates could be set
  p_grabber->setCameraFps(0,cedar::dev::sensors::camera::FrameRate::FPS_15);


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
  std::string info = std::string(p_grabber->getSourceInfo());
  std::cout << "\nGrab channel 0 from \"" << p_grabber->getSourceInfo()<< "\"" << std::endl;

  //----------------------------------------------------------------------------------------
  // print the settings and properties from the camera
  //----------------------------------------------------------------------------------------

  std::cout << "Default settings:" << std::endl;
  std::cout << "\tCamera Mode :\t" << (int) p_grabber->getCameraVideoMode(0);
  cedar::dev::sensors::camera::VideoMode::Id mode = p_grabber->getCameraVideoMode(0);
  std::cout << " (" << cedar::dev::sensors::camera::VideoMode::type().get(mode).name() << ")" << std::endl;

  std::cout << "\tCamera FPS  :\t" << p_grabber->getCameraFps(0);
  cedar::dev::sensors::camera::FrameRate::Id fps = p_grabber->getCameraFps(0);
  std::cout << " (" << cedar::dev::sensors::camera::FrameRate::type().get(fps).name() << ")" << std::endl;

#ifdef CEDAR_USE_LIB_DC1394
  std::cout << "\tCamera ISO-Speed :\t" << p_grabber->getCameraIsoSpeed(0);
  cedar::dev::sensors::camera::IsoSpeed::Id iso_speed = p_grabber->getCameraIsoSpeed(0);
  std::cout << " (" << cedar::dev::sensors::camera::IsoSpeed::type().get(iso_speed).name() << ")" << std::endl;
#endif // CEDAR_USE_LIB_DC1394

  //read properties of camera
  std::cout << "\n\nChannel 0 properties:\n Negative values have special meaning and belongs to the mode.\n"
            << "\tValue of " << CAMERA_PROPERTY_NOT_SUPPORTED << ": Mode not supported\n"
            << "\tValue of " << CAMERA_PROPERTY_MODE_AUTO << ": Mode AUTO\n"
            << "\tValue of " << CAMERA_PROPERTY_MODE_DEFAULT << ": Mode DEFAULT (the backend decides which value)\n"
            << std::endl;

  //use the enum-types from cedar to get information about the used cameras
  std::cout << "\tBrightness: "
    << p_grabber->getProperty(cedar::dev::sensors::camera::Property::PROP_BRIGHTNESS) << std::endl;
  std::cout << "\tSaturation: "
    << p_grabber->getProperty(cedar::dev::sensors::camera::Property::PROP_SATURATION) << std::endl;
  std::cout << "\tHue:        "
    << p_grabber->getProperty(cedar::dev::sensors::camera::Property::PROP_HUE) << std::endl;
  std::cout << "\tGain:       "
    << p_grabber->getProperty(cedar::dev::sensors::camera::Property::PROP_GAIN) << std::endl;
  std::cout << "\tGamma:      "
    << p_grabber->getProperty(cedar::dev::sensors::camera::Property::PROP_GAMMA) << std::endl;
  std::cout << "\tTemperature:"
    << p_grabber->getProperty(cedar::dev::sensors::camera::Property::PROP_TEMPERATURE) << std::endl;

  std::cout << "\tZoom:       "
    << p_grabber->getProperty(cedar::dev::sensors::camera::Property::PROP_ZOOM) << std::endl;
  std::cout << "\tFocus:      "
    << p_grabber->getProperty(cedar::dev::sensors::camera::Property::PROP_FOCUS) << std::endl;

  std::cout << "\tExposure:      "
    << p_grabber->getProperty(cedar::dev::sensors::camera::Property::PROP_EXPOSURE) << std::endl;
  std::cout << "\tAuto-Exposure: "
    << p_grabber->getProperty(cedar::dev::sensors::camera::Property::PROP_AUTO_EXPOSURE) << std::endl;

  std::cout << "\n--------------------------------------------------------------------------------\n" << std::endl;


  //----------------------------------------------------------------------------------------
  // set property
  //----------------------------------------------------------------------------------------

  // in order to set a property, you have to switch over to manual mode for that property
  cedar::dev::sensors::camera::Property::Id prop_id;
  prop_id = cedar::dev::sensors::camera::Property::PROP_BRIGHTNESS;


  if (p_grabber->setPropertyMode(0,prop_id,cedar::dev::sensors::camera::PropertyMode::MANUAL))
  {
    bool test = p_grabber->setProperty(prop_id, 100);
    std::cout << "Set " << cedar::dev::sensors::camera::Property::type().get(prop_id).name()
              << " to " << 100
              << ": Result: " << std::boolalpha << test << std::endl;
    std::cout << "Value after: " << p_grabber->getProperty(prop_id) << std::endl;
    std::cout << "Raw value from camera: " << p_grabber->getPropertyValue(0,prop_id) << std::endl;

  }


  //----------------------------------------------------------------------------------------
  // grab first frame
  //----------------------------------------------------------------------------------------

  //grab a picture
  p_grabber->grab();


  //the first frame is now grabbed and could be read
  //always use the QReadWriteLock for locking the cv::Mat image object
  //on access
  QReadWriteLock* p_lock = p_grabber->getReadWriteLockPointer();

  //the local image buffer
  cv::Mat frame0;

  //get the picture from the grabber
  p_lock->lockForRead();
  frame0 = p_grabber->getImage();
  p_lock->unlock();

  //----------------------------------------------------------------------------------------
  // Create a cedar::aux::gui ImagePlot widget to show grabbed frames
  //----------------------------------------------------------------------------------------

  QApplication app(argc, argv);
  cedar::aux::gui::ImagePlotPtr p_plot = cedar::aux::gui::ImagePlotPtr(new cedar::aux::gui::ImagePlot());
  cedar::aux::MatDataPtr p_data = cedar::aux::MatDataPtr(new cedar::aux::MatData(frame0));
  p_plot->plot(p_data,window_title);
  p_plot->show();
  p_plot->resize(frame0.cols,frame0.rows);

  while (QApplication::hasPendingEvents())
  {
    QApplication::processEvents();
  }


  //start the grabber-thread for updating camera images with 30 fps
  p_grabber->setFps(30);
  std::cout << "Start grabbing in the background" << std::endl;
  p_grabber->startGrabber();

  // get the image from the image buffer
  p_lock->lockForRead();
  p_data->lockForWrite();
  frame0 = p_grabber->getImage();
  p_data->unlock();
  p_lock->unlock();

  //startGrabber recording
  //std::cout << "\nStart Recording\n";
  //p_grabber->startRecording(15);

  // If the camera need a decode filter like from an debayer-filter or something like this
  //p_grabber->setDecodeFilter(cedar::dev::sensors::camera::Decoding::GB_TO_BGR);

  unsigned int counter=0;
  std::cout << "\nDisplay camera frames\n";

  //get frames for a while
  while (!frame0.empty() && p_plot->isVisible())
  {
    while (QApplication::hasPendingEvents())
    {
      QApplication::processEvents();
    }

    //get new images, this is independent from camera-thread
    //if camera-thread is faster than your processing, images will be skipped
    p_lock->lockForRead();
    p_data->lockForWrite();
    frame0 = p_grabber->getImage();
    p_data->unlock();
    p_lock->unlock();

    //every second
    if (! (++counter %= 1000))
    {
      //display real reached fps
      std::cout << "Thread FPS: " << p_grabber->getFpsMeasured() << std::endl;
    }

    cedar::aux::sleep(cedar::unit::Milliseconds(1));
  }


  //----------------------------------------------------------------------------------------
  // save configuration. this step is optional.
  //----------------------------------------------------------------------------------------
  //p_grabber->writeJson(CONFIG_FILE_NAME);

  //----------------------------------------------------------------------------------------
  //clean up
  //----------------------------------------------------------------------------------------

  //stopGrabber grabbing-thread if running
  //recording will also be stopped
  if (p_grabber->isRunning())
  {
    p_grabber->stopGrabber();
  }
  std::cout << "finished\n";

  // p_grabber is deleted in the shared-pointer class
  return 0;
}
