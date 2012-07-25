/*======================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        StereoVideoGrabberTest.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Simple application to grab from an AVI-file (stereo-case)

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/VideoGrabber.h"

// SYSTEM INCLUDES

int main(int, char**)
{

  //--------------------------------------------------------------------------------------------------------------------
  //constants
  //--------------------------------------------------------------------------------------------------------------------

  const std::string FILE_NAME_0 = "/opt/matlab/R2011b/toolbox/images/imdemos/rhinos.avi";
  const std::string FILE_NAME_1 = "/opt/matlab/R2011b/toolbox/images/imdemos/traffic.avi";

  const std::string GRABBER_NAME = "stereo_p_grabber_testcase";
  const std::string CONFIG_FILE_NAME = "stereo_p_grabber_testcase.config";

  //title of highgui window
  const std::string highgui_window_name_0 = FILE_NAME_0;
  const std::string highgui_window_name_1 = FILE_NAME_1;

  //--------------------------------------------------------------------------------------------------------------------
  //main test
  //--------------------------------------------------------------------------------------------------------------------

  std::cout << "\n\nInteractive test of the VideoGrabber class\n";
  std::cout << "--------------------------------------------\n\n";


  //----------------------------------------------------------------------------------------
  //Create the grabber
  //----------------------------------------------------------------------------------------

  std::cout << "Create a VideoGrabber:\n";
  cedar::dev::sensors::visual::VideoGrabber *p_grabber=NULL;
  try
  {
     p_grabber = new cedar::dev::sensors::visual::VideoGrabber(FILE_NAME_0, FILE_NAME_1);
  }
  catch (cedar::aux::InitializationException &e)
  {
    //after an InitializationExeception the p_grabber class isn't initialized correctly
    //and can't be used
    std::cout << "Error on creation of the VideoGrabber class:\n"
              << e.exceptionInfo() << std::endl;

    if (p_grabber)
    {
      delete p_grabber;
    }

    return -1;
  }

  //test: installCrashHandler
  cedar::dev::sensors::visual::Grabber::installCrashHandler();

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

  // this step is essential. If you don't apply the default or loaded Parameter, the grabber will not work
  // check if grabber is created successfully
  if (! p_grabber->applyParameter())
  {
    // an error occured during initialization. Perhaps the file doesn't exist
    if (p_grabber)
    {
      delete p_grabber;
    }
    return -1;
  }

  //----------------------------------------------------------------------------------------
  //Create an OpenCV highgui window to show grabbed frames
  //----------------------------------------------------------------------------------------
  std::cout << "\nDisplay videos in highgui window\n";
  cv::namedWindow(highgui_window_name_0,CV_WINDOW_KEEPRATIO);
  cv::namedWindow(highgui_window_name_1,CV_WINDOW_KEEPRATIO);

  //the first frame is already grabbed on initialization
  cv::Mat frame0 = p_grabber->getImage();
  cv::Mat frame1 = p_grabber->getImage(1);

  //start the grabbing-thread 2 times faster then normal
  p_grabber->setSpeedFactor(2);


  std::cout << "VideoGrabber thread FPS    : " << p_grabber->getFps() << std::endl;
  p_grabber->start();

  unsigned int counter_stat = 0;
  unsigned int counter_end = 500;

  //get frames until avi is over
  //here: never because we have set loop to true
  while (!frame0.empty())
  {
    imshow(highgui_window_name_0,frame0);
    imshow(highgui_window_name_1,frame1);
    frame0 = p_grabber->getImage(0);
    frame1 = p_grabber->getImage(1);

    //status
    if (++counter_stat %= 3 )
    {
      std::cout << "Measured FPS: " << p_grabber->getFpsMeasured()
                << "\tPos_Rel: "<< p_grabber->getPositionRelative()
                << "\tPos_Abs: "<< p_grabber->getPositionAbsolute()
                << std::endl;
    }

    //stop after 500 frames
    if (--counter_end==0)
    {
      break;
    }

    //wait 10ms (needed for highgui)
    //you can change this to 100 or 500, and see the difference
    cv::waitKey(10);
  }


  //----------------------------------------------------------------------------------------
  //clean up
  //----------------------------------------------------------------------------------------
  cv::destroyWindow(highgui_window_name_0);
  cv::destroyWindow(highgui_window_name_1);

  //stop grabbing-thread if running
  //recording will also be stopped
  if (p_grabber->isRunning())
  {
    p_grabber->stop();
  }

  if (p_grabber)
  {
   delete p_grabber;
  }

  std::cout << "finished\n";

  return 0;
}
