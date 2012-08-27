/*========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        PictureGrabberTest.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Simple application to grab from a Picture (mono-case)

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/PictureGrabber.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>


int main(int , char **)
{
  //--------------------------------------------------------------------------------------------------------------------
  //constants
  //--------------------------------------------------------------------------------------------------------------------

  const std::string FILE_NAME_0 = "/usr/share/wallpapers/Vector_Sunset/contents/images/1024x768.jpg";
  const std::string FILE_NAME_1 = "/usr/share/wallpapers/Vector_Sunset/contents/images/1280x1024.jpg";

  const std::string GRABBER_NAME = "picture_grabber_TestCase";
  const std::string CONFIG_FILE_NAME = "picture_grabber_testcase.config";

  //title of highgui window
  const std::string highgui_window_name_0 = FILE_NAME_0;

  //--------------------------------------------------------------------------------------------------------------------
  //main test
  //--------------------------------------------------------------------------------------------------------------------

  std::cout << "\n\nInteractive test of the PictureGrabber class (mono)\n";
  std::cout << "-----------------------------------------------------\n\n";

  //----------------------------------------------------------------------------------------
  //Create the grabber
  //----------------------------------------------------------------------------------------
  std::cout << "Create a PictureGrabber:\n";
  cedar::dev::sensors::visual::PictureGrabber* p_grabber = NULL;
  try
  {
    // create grabber with a filename
    p_grabber = new cedar::dev::sensors::visual::PictureGrabber(FILE_NAME_0);
  }
  catch (cedar::aux::InitializationException &e)
  {
    //after an InitializationExeception the net_grabber class isn't initialized correctly
    //and can't be used
    std::cout << "Error on creation of the NetGrabber class:\n"
              << e.exceptionInfo() << std::endl;

    if (p_grabber)
    {
      delete p_grabber;
    }

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
  // change settings and properties
  //----------------------------------------------------------------------------------------

  /*After initialization of a picture grabber:
   *
   *  The picture is already initialized and the first frame grabbed, so you can check the file using
   *  getImage(), getSize() or something else
   *
   */


  // Set the name of our grabber
  p_grabber->setName(GRABBER_NAME);
  std::cout << "\nGrab from \"" << p_grabber->getSourceInfo()<<"\"" << std::endl;

  // check filesize
  cv::Size ch0_size = p_grabber->getSize(0);
  std::cout << "\nSize of loaded picture: " << ch0_size.width <<" x " << ch0_size.height << std::endl;

  // It is possible to create a snapshot of the pics.
  // In the case, you set another extension as in the source, the image will be converted
  // look at the documentation of setSnapshotName for details
  p_grabber->setSnapshotName("snap.bmp");

  // Check the constructed filenames
  std::cout << "\nCheck filenames of snapshots" << std::endl;
  std::cout << "SnapshotName:\t" << p_grabber->getSnapshotName() <<std::endl;

  // Save a snapshot of the current images
  // Be aware, that existing files will be overwritten without any question
  //p_grabber->saveSnapshotAllCams();


  //----------------------------------------------------------------------------------------
  //Create an OpenCV highgui window to show grabbed frames
  //----------------------------------------------------------------------------------------

  std::cout << "\nShow pictures\n";
  cv::namedWindow(highgui_window_name_0,CV_WINDOW_KEEPRATIO);

  //the first frame is already grabbed on initialization
  cv::Mat frame0 = p_grabber->getImage();

  //get frames for a while
  unsigned int counter=0;

  while (!frame0.empty())
  {
    imshow(highgui_window_name_0,frame0);

    //it is not necessary to do this, unless a new picture should be used
    frame0 = p_grabber->getImage();
    counter++;

    //after one second, set new source-pictures
    if (counter == 10)
    {
      // Grab from another picture
      p_grabber->setSourceFile(0,FILE_NAME_1);
    }

    //exit after another two second
    if (counter == 30)
    {
      break;
    }

    //wait 100ms (needed for highgui)
    cv::waitKey(100);
  }

  //----------------------------------------------------------------------------------------
  //clean up
  //----------------------------------------------------------------------------------------

  // write configuration
  // p_grabber->writeJson(CONFIG_FILE_NAME);

  cv::destroyWindow(highgui_window_name_0);

  //stop grabbing-thread if running
  //recording will also be stopped
  p_grabber->stopGrabber();
  p_grabber->wait();

  if (p_grabber)
  {
    delete p_grabber;
  }

  std::cout << "finished\n";

  return 0;
}
