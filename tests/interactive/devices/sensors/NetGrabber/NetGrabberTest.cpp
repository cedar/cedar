/*========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        NetGrabberTest.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Simple application to grab from a Yarp-server (mono-case)

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
#define YARP_CHANNEL_0 "Net_Grabber_TestCase_Channel"

#define GRABBER_NAME_0 "Net_Grabber_TestCase"
#define CONFIG_FILE_NAME_0 "net_grabber_testcase.config"


//--------------------------------------------------------------------------------------------------------------------
// main test program
//--------------------------------------------------------------------------------------------------------------------

using namespace cv;

int main(int , char **)
{
  //title of highgui window
  std::string highgui_window_name_0 = (std::string) GRABBER_NAME_0 + ": " + YARP_CHANNEL_0;

  std::cout << "\n\nInteractive test of the NetGrabber class (mono)\n";
  std::cout << "-----------------------------------------------------\n\n";

  //------------------------------------------------------------------
  //Create the grabber
  std::cout << "Create a NetGrabber:\n";
  cedar::dev::sensors::visual::NetGrabber *net_grabber=NULL;
  try
  {
    net_grabber = new cedar::dev::sensors::visual::NetGrabber( CONFIG_FILE_NAME_0 , YARP_CHANNEL_0 );
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
  /*After initialization of a NetGrabber:
   *
   * ALWAYS:
   *  - the Net is already grabbed, so you can check the file using
   *    getImage(), getSize() or something else
   *
   * EITHER:
   *  No or new configuration file:
   *  - loopedThread isn't running (only needed for recording)
   *  - grabbername is set to default, i.e. NetGrabber
   *
   * OR:
   *  Parameters loaded from configfile
   *  - thread isn't running
   *  - grabbername is restored from configfile
   */

    net_grabber->setName(GRABBER_NAME_0);
    std::cout << "\nGrab channel 0 from \"" << net_grabber->getSourceInfo()<<"\"" << std::endl;

    //this raise an error, because it isn't a stereo-grabber
    //std::cout << "Grab from (1):" << net_grabber->getSourceInfo(1);

    std::cout << "\nSize of loaded Net:\n";
    cv::Size ch0_size = net_grabber->getSize(0);
    std::cout << "Channel 0: " << ch0_size.width <<" x " << ch0_size.height << std::endl;

    //check framerate of the grabber-thread (thread isn't started yet)
    std::cout << "\nNetGrabber thread FPS : " << net_grabber->getFps() << std::endl;


    //Nevertheless you can create an avi file from your Net-grabbing
    //In this case you have to start the thread
    net_grabber->setRecordName("RecordedNetGrabber.avi");

    //Set Snapshotnames without channel-number (in stereo-case: default value is 0)
    //This is the same behavior as setRecordName()
    //The type of the file (e.g. bitmap or jpg or something else) depend on extension
    //look at the documentation of setSnapshotName for details
    net_grabber->setSnapshotName("NetGrabber_Snapshot.jpg"); 
    
    //Check the constructed filenames
    std::cout << "Check filenames of snapshots and recordings\n" << std::endl;

    std::cout << "SnapshotName:\t" << net_grabber->getSnapshotName() <<std::endl;
    std::cout << "RecordName:\t" << net_grabber->getRecordName() <<std::endl;

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

  //------------------------------------------------------------------
  //Create an OpenCV highgui window to show grabbed frames
  std::cout << "\nDisplay the grabbed pictures\n";
  namedWindow(highgui_window_name_0,CV_WINDOW_KEEPRATIO);

  //the first frame is already grabbed on initialization
  //it is in the internal buffer and can be used
  cv::Mat frame0 = net_grabber->getImage();

  //start recording
  net_grabber->setFps(15);
  std::cout << "\nStart Recording\n";

  //startRecording starts the grabbing-thread if not running
  //the speed of the grabbingthread is independent of the speed writing to
  //the avi-file. The video will only be shorter or longer.
  net_grabber->startRecording(15);

  //get frames for a while
  unsigned int counter=0;

  while (!frame0.empty())
  {
    imshow(highgui_window_name_0,frame0);
    frame0 = net_grabber->getImage();

    counter++;

    //after one second, show some stats
    if (counter == 100)
    {
      std::cout << "Actual framerate of grabbing: " << net_grabber->getFpsMeasured() << std::endl;
      // Save a snapshot from channel 0 only
      // Be aware, that existing files will be overwritten without any question
      //net_grabber->saveSnapshot(0);
    }

    //exit after another second
    if (counter == 200)
    {
      break;
    }

    //wait 10ms (needed for highgui)
    waitKey(10);
  }

  //------------------------------------------------------------------
  //clean up

  destroyWindow(highgui_window_name_0);

  //stop grabbing-thread if running
  //recording will also be stopped
  if (net_grabber->isRunning())
  {
    net_grabber->stop();
  }

  if (net_grabber)
  {
    delete net_grabber;
  }

  std::cout << "finished\n";

  return 0;
}
