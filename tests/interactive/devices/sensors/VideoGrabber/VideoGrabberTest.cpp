/*========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        VideoGrabberTest.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Simple application to grab from an AVI-file (mono-case)

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/VideoGrabber.h"

// SYSTEM INCLUDES


int main(int , char **)
{

  //--------------------------------------------------------------------------------------------------------------------
  //constants
  //--------------------------------------------------------------------------------------------------------------------

  const std::string FILE_NAME_0 = "/opt/matlab/R2011b/toolbox/images/imdemos/rhinos.avi";

  const std::string GRABBER_NAME = "video_grabber_testcase";
  const std::string CONFIG_FILE_NAME = "video_grabber_testcase.config";

  //title of highgui window
  const std::string highgui_window_name_0 = FILE_NAME_0;

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
     p_grabber = new cedar::dev::sensors::visual::VideoGrabber(FILE_NAME_0);
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
  // change settings and properties
  //----------------------------------------------------------------------------------------

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
   *  - loopedThread isn't running (startGrabber auto-grabbing with startGrabber() )
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
  p_grabber->setName(GRABBER_NAME);

  //check the source
  //in the case of a videograbber it is the filename
  std::cout << "\nGrab channel 0 from \"" << p_grabber->getSourceInfo(0)<< std::endl;

  //Get more Informations from loaded avis
  std::cout << "\nSome informations of the video file:\n";
  std::cout << "\tfourcc : " << p_grabber->getSourceEncoding() << std::endl;
  std::cout << "\tframes : " << p_grabber->getFrameCount() << std::endl;
  std::cout << "\tpos_rel: " << p_grabber->getPositionRelative() << std::endl;
  std::cout << "\tpos_abs: " << p_grabber->getPositionAbsolute() << std::endl;
  std::cout << "\tFPS    : " << p_grabber->getSourceFps() << std::endl;

  //check framerate of the grabber-thred (thread isn't started yet)
  std::cout << "\nVideoGrabber thread FPS: " << p_grabber->getFps() << std::endl;

  //Set the name for the recording file
  //Start recording with startRecording() (thread will be started on startRecording!)
  p_grabber->setRecordName("record.avi");

  //Set Snapshotnames without channel-number (in mono-case: default value is 0)
  //Filetype depends on extension
  p_grabber->setSnapshotName("snap.bmp");
  p_grabber->setSnapshotName(0,"snap01.jpg");

  //Check the constructed filenames
  std::cout << "\nCheck filenames of snapshots and recordings:" << std::endl;
  std::cout << "\tSnapshotName:\t" << p_grabber->getSnapshotName(0) <<std::endl;
  std::cout << "\tRecordName:\t" << p_grabber->getRecordName(0) <<std::endl;

  //enforcing an error and catch it
  try
  {
    std::cout << "\nTry to enforce an exception:\n";
    std::cout << "SnapshotName_2: " << p_grabber->getSnapshotName(2) <<std::endl;
  }
  catch (cedar::aux::ExceptionBase& e)
  {
    //std::cout << "Exception: " <<e.what() << std::endl; //until now: buggy cedar implementation
    std::cout <<e.exceptionInfo()<<std::endl;
  }

  //Save a snapshot of the current images
  p_grabber->saveSnapshot();

  std::cout << "\nGrabbing and scrolling in the Video-File\n";

  // until now, only the first frame of the AVI-file is grabbed.
  // so lets grab the next frame into the buffer

  std::cout << "\nGrab the second frame (frame no. 1):"<< std::endl;
  p_grabber->grab();
  std::cout << "\tPos_Rel: " << p_grabber->getPositionRelative() << std::endl;
  std::cout << "\tPos_Abs: " << p_grabber->getPositionAbsolute() << std::endl;

  //goto frame 50
  std::cout << "\nScrolling to frame 50:"<< std::endl;
  p_grabber->setPositionAbsolute(50);
  p_grabber->grab();
  std::cout << "\tPos_Rel: " << p_grabber->getPositionRelative() << std::endl;
  std::cout << "\tPos_Abs: " << p_grabber->getPositionAbsolute() << std::endl;

//goto the end, i.e. the last frame
  std::cout << "\nScrolling to the end of the file:"<< std::endl;
  p_grabber->setPositionRelative(1);
  p_grabber->grab();
  std::cout << "\tPos_Rel: " << p_grabber->getPositionRelative() << std::endl;
  std::cout << "\tPos_Abs: " << p_grabber->getPositionAbsolute() << std::endl;


  //switch back to startGrabber
  std::cout << "\nScrolling to the beginning of the video-file:"<< std::endl;
  p_grabber->setPositionRelative(0);
  p_grabber->grab();
  std::cout << "\tPos_Rel: " << p_grabber->getPositionRelative() << std::endl;
  std::cout << "\tPos_Abs: " << p_grabber->getPositionAbsolute() << std::endl;

  //with the getSourceProperty member function, it is possible to get other informations
  //supported by OpenCV's VideoCapture.get() function
  std::cout << "AVI-parameter (channel 0) position abs: "<<p_grabber->getSourceProperty(0,CV_CAP_PROP_POS_FRAMES)<< std::endl;

  //some options
  //loops through the avi
  p_grabber->setLooped(true);

  //----------------------------------------------------------------------------------------
  //Create an OpenCV highgui window to show grabbed frames
  //----------------------------------------------------------------------------------------

  std::cout << "\nDisplay video in highgui window\n";
  cv::namedWindow(highgui_window_name_0,CV_WINDOW_KEEPRATIO);

  cv::Mat frame0 = p_grabber->getImage();

  //startGrabber the grabbing-thread 2 times faster then normal
  std::cout << "\nSet speed factor of grabbing-thread to 2\n";
  p_grabber->setSpeedFactor(2);
  std::cout << "VideoGrabber thread FPS    : " << p_grabber->getFps() << std::endl;

  p_grabber->startGrabber();


  //from now on, you have to be aware, that the grabbing thread writes on the image
  //so use the ReadWriteLock
  QReadWriteLock * pReadWriteLock;
  pReadWriteLock = p_grabber->getReadWriteLockPointer();

  //enable Recording
  //The FPS of Recording is independent from GrabbingThread.

  //recording with actual speed (depends on speed factor)
  //the grabbing thread will be started on startRecorded if isn't running
  p_grabber->startRecording(p_grabber->getFps(),cedar::dev::sensors::visual::RecordingFormat::RECORD_MP42);

  //recording with speed of avi-file
  //p_grabber->startRecording(p_grabber->getSourceFps(),CV_FOURCC('M','P','4','2'));

  //recording with fixed framerate
  //p_grabber->startRecording(50,CV_FOURCC('M','P','4','2'));

  unsigned int counter_stat = 0;
  unsigned int counter_end = 500;

  //get frames until avi is over
  //here: never because we have set loop to true
  while (!frame0.empty())
  {
    cv::imshow(highgui_window_name_0,frame0);

    //get images with respect to grabbing-thread
    pReadWriteLock->lockForRead();
    cv::Mat frame0 = p_grabber->getImage();
    pReadWriteLock->unlock();

    //status
    if (++counter_stat %= 3 )
    {
      std::cout << "Measured FPS: " << p_grabber->getFpsMeasured()
                << "\tPos_Rel: "<< p_grabber->getPositionRelative()
                << "\tPos_Abs: "<<p_grabber->getPositionAbsolute()
                << std::endl;
    }

    //stopGrabber after 500 frames
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

  //stopGrabber grabbing-thread if running
  //recording will also be stopped
  if (p_grabber->isRunning())
  {
    p_grabber->stopGrabber();
  }

  if (p_grabber)
  {
   delete p_grabber;
  }

  std::cout << "finished\n";

  return 0;
}
