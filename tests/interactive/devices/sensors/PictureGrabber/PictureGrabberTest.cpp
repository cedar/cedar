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

// LOCAL INCLUDES
#include "cedar/devices/sensors/visual/PictureGrabber.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>


int main(int , char **)
{

  const std::string FILE_NAME_0 = "/usr/share/wallpapers/Vector_Sunset/contents/images/1024x768.jpg";
  const std::string FILE_NAME_1 = "/usr/share/wallpapers/Vector_Sunset/contents/images/1280x1024.jpg";

  const std::string GRABBER_NAME_0 = "Picture_Grabber_TestCase";
  const std::string CONFIG_FILE_NAME_0 = "picture_grabber_testcase.config";

  //title of highgui window
  const std::string highgui_window_name_0 = FILE_NAME_0;

  std::cout << "\n\nInteractive test of the PictureGrabber class (mono)\n";
  std::cout << "-----------------------------------------------------\n\n";

  //------------------------------------------------------------------
  //Create the grabber
  std::cout << "Create a PictureGrabber:\n";
  cedar::dev::sensors::visual::PictureGrabber *picture_grabber = NULL;
  try
  {
    picture_grabber = new cedar::dev::sensors::visual::PictureGrabber( CONFIG_FILE_NAME_0 , FILE_NAME_0 );
  }
  catch (cedar::aux::InitializationException &e)
  {
    //after an InitializationExeception the net_grabber class isn't initialized correctly
    //and can't be used
    std::cout << "Error on creation of the NetGrabber class:\n"
              << e.exceptionInfo() << std::endl;

    if (picture_grabber)
    {
      delete picture_grabber;
    }

    return -1;
  }


  //------------------------------------------------------------------
  /*After initialization of a picture_grabber:
   *
   * ALWAYS:
   *  - the picture is already grabbed, so you can check the file using
   *    getImage(), getSize() or something else
   *
   * EITHER:
   *  No or new configuration file:
   *  - loopedThread isn't running (only needed for recording)
   *  - grabbername is set to default, i.e. picture_grabber
   *
   * OR:
   *  Parameters loaded from configfile
   *  - thread isn't running
   *  - grabbername is restored from configfile
   */

    picture_grabber->setName(GRABBER_NAME_0);
    std::cout << "\nGrab channel 0 from \"" << picture_grabber->getSourceInfo()<<"\"" << std::endl;

    //this raise an error, because it isn't a stereo-grabber
    //std::cout << "Grab from (1):" << picture_grabber->getSourceInfo(1);

    std::cout << "\nSize of loaded picture:\n";
    cv::Size ch0_size = picture_grabber->getSize(0);
    std::cout << "Channel 0: " << ch0_size.width <<" x " << ch0_size.height << std::endl;

    //check framerate of the grabber-thread (thread isn't started yet)
    std::cout << "\nPictureGrabber thread FPS : " << picture_grabber->getFps() << std::endl;

    //grabberthread don't have to be started. There is no new content to grab.

    //Nevertheless you can create an avi file from your picture-grabbing
    //In this case you have to start the thread
    picture_grabber->setRecordName("RecordedPictureGrabber(mono).avi");

    //It is possible to create a snaptshot of the pics.
    //In the case, you set another extenstion as in the source, the image will be converted

    //Set Snapshotnames without channel-number (in mono-case: default value is 0)
    //This is the same behavior as setRecordName()
    //The type of the file (e.g. bitmap or jpg or something else) depend on extension
    //look at the documentation of setSnapshotName for details
    picture_grabber->setSnapshotName("snap.bmp");  // snap[ch0].bmp and snap[ch1].bmp
    picture_grabber->setSnapshotName(0,"snap01.jpg"); // rename channel 0 to snap.jpg. Channel 1 isn't altered

    //Check the constructed filenames
    std::cout << "Check filenames of snapshots and recordings\n" << std::endl;

    std::cout << "SnapshotName:\t" << picture_grabber->getSnapshotName() <<std::endl;
    std::cout << "RecordName:\t" << picture_grabber->getRecordName() <<std::endl;

    //enforcing an error and catch it
    try
    {
      std::cout << "\nTry to enforce an exception:\n";
      std::cout << "SnapshotName_2: " << picture_grabber->getSnapshotName(2) <<std::endl;
    }
    catch (cedar::aux::ExceptionBase& e)
    {
      //std::cout << "Exception: " <<e.what() << std::endl; //until now: buggy cedar implementation
      //all grabber exceptions (look at cedar/devices/sensors/visual/exceptions/namespace.h)
      //can be used without separate include-files
      std::cout <<e.exceptionInfo()<<std::endl;
    }

    // Save a snapshot of the current images
    //picture_grabber->saveSnapshotAllCams();

    // Save a snapshot from channel 0 only
    // Be aware, that existing files will be overwritten without any question
    //picture_grabber->saveSnapshot(0);


  //------------------------------------------------------------------
  //Create an OpenCV highgui window to show grabbed frames
  std::cout << "\nShow pictures\n";
  cv::namedWindow(highgui_window_name_0,CV_WINDOW_KEEPRATIO);

  //the first frame is already grabbed on initialization
  cv::Mat frame0 = picture_grabber->getImage();

  //start recording
  picture_grabber->setFps(15);
  std::cout << "\nStart Recording\n";

  //startRecording starts the grabbing-thread if not running
  //the speed of the grabbingthread is independent of the speed writing to
  //the avi-file. The video will only be shorter or longer.
  picture_grabber->startRecording(15,CV_FOURCC('M','P','4','2'));

  //get frames for a while
  unsigned int counter=0;

  while (!frame0.empty())
  {
    imshow(highgui_window_name_0,frame0);

    //it is not necessary to do this, unless a new picture should be used
    frame0 = picture_grabber->getImage();
    counter++;

    //after one second, set new source-pictures
    if (counter == 10)
    {
      // Grab from another picture
      // Be aware, when recording is on:
      // The size of the video-frames wont't be changed.
      // So, if the size of the new picture is different from the old one
      // the new picture will be resized to the dimensions of the old
      // regardless of the aspect ratio
      picture_grabber->setSourceFile(0,FILE_NAME_1);
    }

    //exit after another second
    if (counter == 20)
    {
      break;
    }

    //wait 100ms (needed for highgui)
    cv::waitKey(100);
  }

  //------------------------------------------------------------------
  //clean up

  cv::destroyWindow(highgui_window_name_0);

  //stop grabbing-thread if running
  //recording will also be stopped
  if (picture_grabber->isRunning())
  {
    picture_grabber->stop();
  }

  if (picture_grabber)
  {
    delete picture_grabber;
  }

  std::cout << "finished\n";

  return 0;
}
