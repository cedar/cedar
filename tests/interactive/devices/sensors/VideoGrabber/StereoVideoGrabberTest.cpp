/*========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        StereoVideoGrabberTest.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Simple application to grab from an AVI-file (stereo-case)

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/devices/sensors/visual/VideoGrabber.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
//#include <iostream>
//#include <exception>


//--------------------------------------------------------------------------------------------------------------------
//constants
//--------------------------------------------------------------------------------------------------------------------
#define FILE_NAME_0 "/opt/matlab/R2010b/toolbox/images/imdemos/rhinos.avi"
#define FILE_NAME_1 "/opt/matlab/R2010b/toolbox/images/imdemos/traffic.avi"

#define GRABBER_NAME_0 "Stereo_Video_Grabber_TestCase"
#define CONFIG_FILE_NAME_0 "stereo_video_grabber_testcase.config"


//--------------------------------------------------------------------------------------------------------------------
// main test program
//--------------------------------------------------------------------------------------------------------------------

using namespace cv;


int main(int , char **)
{
  //title of highgui window
  std::string highgui_window_name_0 = (std::string) GRABBER_NAME_0 + ":" + FILE_NAME_0;
  std::string highgui_window_name_1 = (std::string) GRABBER_NAME_0 + ":" + FILE_NAME_1;



  std::cout << "\n\nInteractive test of the VideoGrabber class (stereo case)\n";
  std::cout << "--------------------------------------------\n\n";


  //------------------------------------------------------------------
  //Create the grabber
  std::cout << "Create a VideoGrabber:\n";
  cedar::dev::sensors::visual::VideoGrabber *video_grabber=NULL;
  try
  {
     video_grabber = new cedar::dev::sensors::visual::VideoGrabber( CONFIG_FILE_NAME_0 , FILE_NAME_0, FILE_NAME_1 );
  }
  catch (cedar::aux::exc::InitializationException &e)
  {
    //after an InitializationExeception the video_grabber class isn't initialized correctly
    //and can't be used
    std::cout << "Error on creation of the VideoGrabber class:\n"
              << e.exceptionInfo() << std::endl;

    if (video_grabber)
    {
      delete video_grabber;
    }

    return -1;
  }

  //------------------------------------------------------------------
  /*After initialization of a VideoGrabber:
   *
   * ALWAYS:
   *  - the first frame is already grabbed, so you can check the file using
   *    getImage(), getSize(), getAviParam() or something else
   *
   * EITHER:
   *  No or new configuration file:
   *  - the framerate is set to the speed stored inside the video-file
   *  - loop is on
   *  - speedFactor is 1
   *  - loopedThread isn't running (start auto-grabbing with start() )
   *  - grabber name is set to default, i.e. VideoGrabber
   *
   * OR:
   *  Parameters loaded from configfile
   *  - loop and speedfactor loaded
   *  - speed is set on speedfactor*avi_file_speed
   *  - thread isn't running
   *  - grabber name is restored from configfile
   */

  //set the name to our custom name
  video_grabber->setName(GRABBER_NAME_0);

  //check the source
  //in the case of a videograbber it is the filename
  std::cout << "\nGrab channel 0 from \"" << video_grabber->getSourceInfo(0)<< std::endl;

  //Get more Informations from loaded avis

  std::cout << "\nSome informations of the video files:\n";
  std::cout << "\tframes : " << video_grabber->getFrameCount() << std::endl;
  std::cout << "\tpos_rel: " << video_grabber->getPositionRel() << std::endl;
  std::cout << "\tpos_abs: " << video_grabber->getPositionAbs() << std::endl;

  std::cout << "\nSome informations on video file (ch0):\n";
  std::cout << "\tfourcc : " << video_grabber->getSourceEncoding() << std::endl;
  std::cout << "\tFPS    : " << video_grabber->getSourceFps() << std::endl;

  std::cout << "\nSome informations on video file (ch1):\n";
  std::cout << "\tfourcc : " << video_grabber->getSourceEncoding(1) << std::endl;
  std::cout << "\tFPS    : " << video_grabber->getSourceFps(1) << std::endl;



  //check framerate of the grabber-thred (thread isn't started yet)
  std::cout << "\nVideoGrabber thread FPS: " << video_grabber->getFps() << std::endl;

  //Set the name for the recording file
  //This function have special cases
  //In the mono case, filename is used without changes
  //In the stereo case, the filenames are constructed like
  //filename[ch0].ext and filename[ch1].ext
  //If you want to set the names independently, you can use it with the camera-parameter
  //Start recording with startRecording() (thread will be started on startRecording!)
  video_grabber->setRecordName("record.avi");
  //record-filenames now set to record[ch0].avi and record[ch1].avi


  //Set Snapshotnames without channel-number (in mono-case: default value is 0)
  //This is the same behavior as setRecordName()
  //The type of the file (e.g. bitmap or jpg or something else) depends on extension
  //look at the documentation of setSnapshotName for details
  video_grabber->setSnapshotName("snap.bmp");  // snap[ch0].bmp and snap[ch1].bmp
  video_grabber->setSnapshotName(0,"snap01.jpg"); // rename channel 0 to snap.jpg. Channel 1 isn't altered

  //Check the constructed filenames
  std::cout << "\nCheck filenames of snapshots and recordings:" << std::endl;
  std::cout << "\tSnapshotName:\t" << video_grabber->getSnapshotName(0) <<std::endl;
  std::cout << "\tRecordName:\t" << video_grabber->getRecordName(0) <<std::endl;

  //enforcing an error and catch it
  try
  {
    std::cout << "\nTry to enforce an exception:\n";
    std::cout << "SnapshotName_2: " << video_grabber->getSnapshotName(2) <<std::endl;
  }
  catch (cedar::aux::exc::ExceptionBase& e)
  {
    //std::cout << "Exception: " <<e.what() << std::endl; //until now: buggy cedar implementation
    std::cout <<e.exceptionInfo()<<std::endl;
  }

  //Save a snapshot of the current image on channel 0
  //video_grabber->saveSnapshot();
  //Or on all channels
  //video_grabber->saveSnapshotAllCams();

  std::cout << "\nGrabbing and scrolling in the Video-File\n";

  // until now, only the first frame of the AVI-file is grabbed.
  // so lets grab the next frame into the buffer

  std::cout << "\nGrab the second frame (frame no. 1):"<< std::endl;
  video_grabber->grab();
  std::cout << "\tPos_Rel: " << video_grabber->getPositionRel() << std::endl;
  std::cout << "\tPos_Abs: " << video_grabber->getPositionAbs() << std::endl;

  //goto frame 50
  std::cout << "\nScrolling to frame 50:"<< std::endl;
  video_grabber->setPositionAbs(50);
  video_grabber->grab();
  std::cout << "\tPos_Rel: " << video_grabber->getPositionRel() << std::endl;
  std::cout << "\tPos_Abs: " << video_grabber->getPositionAbs() << std::endl;

//goto the end, i.e. the last frame
  std::cout << "\nScrolling to the end of the file:"<< std::endl;
  video_grabber->setPositionRel(1);
  video_grabber->grab();
  std::cout << "\tPos_Rel: " << video_grabber->getPositionRel() << std::endl;
  std::cout << "\tPos_Abs: " << video_grabber->getPositionAbs() << std::endl;


  //switch back to start
  std::cout << "\nScrolling to the beginning of the video-file:"<< std::endl;
  video_grabber->setPositionRel(0);
  video_grabber->grab();
  std::cout << "\tPos_Rel: " << video_grabber->getPositionRel() << std::endl;
  std::cout << "\tPos_Abs: " << video_grabber->getPositionAbs() << std::endl;

  //with the getSourceProperty member function, it is possible to get other informations
  //supported by OpenCV's VideoCapture.get() function
  std::cout << "AVI-parameter (channel 0) position abs: "<<video_grabber->getSourceProperty(0,CV_CAP_PROP_POS_FRAMES)<< std::endl;

  //some options
  //loops through the avi
  video_grabber->setLoop(true);

  //------------------------------------------------------------------
  //Create an OpenCV highgui window to show grabbed frames
  std::cout << "\nDisplay videos in highgui window\n";
  namedWindow(highgui_window_name_0,CV_WINDOW_KEEPRATIO);
  namedWindow(highgui_window_name_1,CV_WINDOW_KEEPRATIO);

  //the first frame is already grabbed on initialization
  cv::Mat frame0 = video_grabber->getImage();
  cv::Mat frame1 = video_grabber->getImage(1);

  //start the grabbing-thread 2 times faster then normal
  std::cout << "\nSet speed factor of grabbing-thread to 2\n";
  video_grabber->setSpeedFactor(2);
  std::cout << "VideoGrabber thread FPS    : " << video_grabber->getFps() << std::endl;
  video_grabber->start();

  //enable Recording
  //The FPS of Recording is independent from GrabbingThread.

  //recording with actual speed (depends on speed factor)
  //the grabbing thread will be started on startRecorded if isn't running
  //video_grabber->startRecording(video_grabber->getFps(),CV_FOURCC('M','P','4','2'));

  //recording with speed of avi-file
  //video_grabber->startRecording(video_grabber->getSourceFps(),CV_FOURCC('M','P','4','2'));

  //recording with fixed framerate
  //video_grabber->startRecording(50,CV_FOURCC('M','P','4','2'));

  unsigned int counter_stat = 0;
  unsigned int counter_end = 500;

  //get frames until avi is over
  //here: never because we have set loop to true
  while (!frame0.empty())
  {
    imshow(highgui_window_name_0,frame0);
    imshow(highgui_window_name_1,frame1);
    frame0 = video_grabber->getImage(0);
    frame1 = video_grabber->getImage(1);

    //status
    if (++counter_stat %= 3 )
    { std::cout << "Measured FPS: " << video_grabber->getFpsMeasured()
      << "\tPos_Rel: "<< video_grabber->getPositionRel()
      << "\tPos_Abs: "<< video_grabber->getPositionAbs()
      <<std::endl;
    }

    //stop after 500 frames
    if (--counter_end==0)
    {
      break;
    }
    //wait 10ms (needed for highgui)
    //you can change this to 100 or 500, and see the difference
    waitKey(10);
  }


  //------------------------------------------------------------------
  //clean up

  destroyWindow(highgui_window_name_0);
  destroyWindow(highgui_window_name_1);

  //stop grabbing-thread if running
  //recording will also be stopped
  if (video_grabber->isRunning())
  {
    video_grabber->stop();
  }

  if (video_grabber)
  {
   delete video_grabber;
  }

  std::cout << "finished\n";

  return 0;
}
