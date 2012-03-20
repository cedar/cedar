/*========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        StereoNetGrabberTest.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Simple application to grab from a Yarp-Server (stereo-case)

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/devices/sensors/visual/NetGrabber.h"
//#include <devices/sensors/visual/NetGrabber.h>

// PROJECT INCLUDES


// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>



//--------------------------------------------------------------------------------------------------------------------
//constants
//--------------------------------------------------------------------------------------------------------------------
#define YARP_CHANNEL_0 "Net_Grabber_TestCase_Channel_0"
#define YARP_CHANNEL_1 "Net_Grabber_TestCase_Channel_1"

#define GRABBER_NAME_0 "Stereo_Net_Grabber_TestCase"
#define CONFIG_FILE_NAME_0 "stereo_net_grabber_testcase.config"


//--------------------------------------------------------------------------------------------------------------------
// main test program
//--------------------------------------------------------------------------------------------------------------------


using namespace cv;


int main(int , char **)
{

  //title of highgui window
  std::string highgui_window_name_0 = (std::string) "0 " + GRABBER_NAME_0 + ": " + YARP_CHANNEL_0;
  std::string highgui_window_name_1 = (std::string) "1 " + GRABBER_NAME_0 + ": " + YARP_CHANNEL_1;


  std::cout << "\n\nInteractive test of the NetGrabber class (stereo)\n";
  std::cout << "-----------------------------------------------------\n\n";

  //------------------------------------------------------------------
  //Create the grabber
  std::cout << "Create a NetGrabber:\n";
  cedar::dev::sensors::visual::NetGrabber *net_grabber=NULL;
  try
  {
    net_grabber = new cedar::dev::sensors::visual::NetGrabber( CONFIG_FILE_NAME_0 , YARP_CHANNEL_0, YARP_CHANNEL_1 );
  }
  catch (cedar::aux::exc::InitializationException &e)
  {
    //after an InitializationExeception the net_grabber class isn't initialized correctly
    //and can't be used
    std::cout << "Error on creation of the NetGrabber class:\n"
              << e.exceptionInfo() << std::endl;

    if (net_grabber)
    {
      delete net_grabber;
    }

    return -1;
  }
  //------------------------------------------------------------------
  /*After initialization of a net_grabber:
   *
   * ALWAYS:
   *  - the first frame is already grabbed, so you can check the file using
   *    getImage(), getSize(), getAviParam() or something else
   *
   * EITHER:
   *  No or new configuration file:
   *  - loopedThread isn't running (startGrabber auto-grabbing with startGrabber() )
   *  - grabbername is set to default, i.e. NetGrabber
   *
   * OR:
   *  Parameters loaded from configfile
   *  - thread isn't running
   *  - grabbername is restored from configfile
   */

    net_grabber->setName(GRABBER_NAME_0);

    std::cout << "\nGrab from (0) " << net_grabber->getSourceInfo(0)<< std::endl;
    std::cout << "Grab from (1) " << net_grabber->getSourceInfo(1)<< std::endl;

    //error, because it is only a stereo-grabber
    //std::cout << "Grab from (3):" << net_grabber->getSourceInfo(2);


    std::cout << "\nSize of received Frames:\n";
    cv::Size ch0_size = net_grabber->getSize(0);
    cv::Size ch1_size = net_grabber->getSize(1);
    std::cout << "Channel 0: " << ch0_size.width <<" x " << ch0_size.height << std::endl;
    std::cout << "Channel 1: " << ch1_size.width <<" x " << ch1_size.height << std::endl;


    //check framerate of the grabber-thred (thread isn't started yet)
    std::cout << "\nNetGrabber thread FPS : " << net_grabber->getFps() << std::endl;

    //grabberthread don't have to be started. There is no new content to grab.

    //Nevertheless you can create an avi file from your picture-grabbing
    //In this case you have to startGrabber the thread
    net_grabber->setRecordName("RecordedStereoNetGrabber.avi");

    //It is possible to create a snaptshot of the pics.
    //In the case, you set another extenstion as in the source, the image will be converted

    //Set Snapshotnames without channel-number (in mono-case: default value is 0)
    //This is the same behavior as setRecordName()
    //The type of the file (e.g. bitmap or jpg or something else) depend on extension
    //look at the documentation of setSnapshotName for details
    // StereoNetGrabber_Snapshot[ch0].bmp and StereoNetGrabber_Snapshot[ch1].bmp
    net_grabber->setSnapshotName("StereoNetGrabber_Snapshot.bmp");

    // rename channel 0 to StereoNetGrabber_Snapshot_0.jpg. Channel 1 isn't altered
    net_grabber->setSnapshotName(0,"StereoNetGrabber_Snapshot_0.jpg");

    //Check the constructed filenames
    std::cout << "Check filenames of snapshots and recordings\n" << std::endl;

    std::cout << "SnapshotName_0:\t" << net_grabber->getSnapshotName(0) <<std::endl;
    std::cout << "SnapshotName_1:\t" << net_grabber->getSnapshotName(1) <<std::endl;
    std::cout << "RecordName_0:\t" << net_grabber->getRecordName(0) <<std::endl;
    std::cout << "RecordName_1:\t" << net_grabber->getRecordName(1) <<std::endl;

    //enforcing an error and catch it
    try
    {
      std::cout << "\nTry to enforce an exception:\n";
      std::cout << "SnapshotName_2: " << net_grabber->getSnapshotName(2) <<std::endl;
    }
    catch (cedar::aux::exc::ExceptionBase& e)
    {
      //std::cout << "Exception: " <<e.what() << std::endl; //until now: buggy cedar implementation
      //all grabber exceptions (look at cedar/devices/sensors/visual/exceptions/namespace.h)
      //can be used without separate include-files
      std::cout <<e.exceptionInfo()<<std::endl;
    }

    // Save a snapshot of the current images
    //net_grabber->saveSnapshotAllCams();

    // Save a snapshot from channel 0 only
    // Be aware, that existing files will be overwritten without any question
    //net_grabber->saveSnapshot(0);


  //------------------------------------------------------------------
  //Create an OpenCV highgui window to show grabbed frames
  std::cout << "\nDisplay received pictures\n";
  namedWindow(highgui_window_name_0,CV_WINDOW_KEEPRATIO);
  namedWindow(highgui_window_name_1,CV_WINDOW_KEEPRATIO);

  //the first frame is already grabbed on initialization
  cv::Mat frame0 = net_grabber->getImage();
  cv::Mat frame1 = net_grabber->getImage(1);

  //startGrabber recording
  std::cout << "\nStart Recording\n";
  net_grabber->setFps(15);
  net_grabber->startGrabber();
  net_grabber->startRecording(15);


  //get frames for a while
  unsigned int counter=0;

  while (!frame0.empty())
  {
    imshow(highgui_window_name_0,frame0);
    imshow(highgui_window_name_1,frame1);

    //it is not necessary to do this, unless a new picture should be used
    frame0 = net_grabber->getImage();
    frame1 = net_grabber->getImage(1);
    counter++;

    //after one second, do something
    if (counter == 100)
    {
      std::cout << "Actual framerate of grabbing: " << net_grabber->getFpsMeasured() << std::endl;

    }

    //exit after another second
    if (counter == 200)
    {
      break;
    }

    //wait 10ms (needed for highgui)
    waitKey(10);
  }

  //stopGrabber grabbing-thread if running
  if (net_grabber->isRunning())
  {
    net_grabber->stopGrabber();
  }

  //------------------------------------------------------------------
  //clean up

  destroyWindow(highgui_window_name_0);
  destroyWindow(highgui_window_name_1);

  //stopGrabber grabbing-thread if running
  //recording will also be stopped
  if (net_grabber->isRunning())
  {
    net_grabber->stopGrabber();
  }

  if (net_grabber)
  {
   delete net_grabber;
  }

  std::cout << "finished\n";

  return 0;
}
