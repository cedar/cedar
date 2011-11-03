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

// PROJECT INCLUDES
#include <devices/sensors/visual/VideoGrabber.h>

// SYSTEM INCLUDES
#include <iostream>
#include <exception>



/*! \brief This is a short example of how to use the Video-Grabber class.
 *
 *  \remarks
 *      <pre>
 *        Needed libraries:
 *          - libcedardev     (for grabber)
 *          - libcedaraux     (for exceptions)
 *
 *        Needed include-paths:
 *          - opencv2
 *          - cedar
 *      </pre>
 *
 */




using namespace cv;


int main(int , char **)
{

  //------------------------------------------------------------------
  //definition of used files and names
  const char* filename0 = "/opt/matlab/R2010b/toolbox/images/imdemos/rhinos.avi";
  const char* filename1 = "/opt/matlab/R2010b/toolbox/images/imdemos/traffic.avi";
  const char* grabbername = "TestStereoVideoGrabber";
  const char* configfile ="stereovideograbber.configfile";

  std::string window0 = (std::string)grabbername+":"+filename0;
  std::string window1 = (std::string)grabbername+":"+filename1;

  std::cout << "\n\nInteractive test of the VideoGrabber class (stereo)\n";
  std::cout << "-----------------------------------------------------\n\n";



  //------------------------------------------------------------------
  //Create the grabber
  std::cout << "Create a PictureGrabber:\n";
  cedar::dev::sensors::visual::VideoGrabber video_grabber(configfile,filename0,filename1);


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
   *  - grabbername is set to default, i.e. VideoGrabber
   *
   * OR:
   *  Parameters loaded from configfile
   *  - loop and speedfactor loaded
   *  - speed is set on speedfactor*avi_file_speed
   *  - thread isn't running
   *  - grabbername is restored from configfile
   */


    std::cout << "\nGrab from (0) " << video_grabber.getSourceInfo(0)<< std::endl;
    std::cout << "Grab from (1) " << video_grabber.getSourceInfo(1)<< std::endl;

    //error, because it is only a stereo-grabber
    //std::cout << "Grab from (3):" << video_grabber.getPhysicalSourceInformation(2);


    //Get more Informations from loaded avis
    std::cout << "\nSome Informations of loaded video files:\n";
    std::cout << "AVI(ch0) fourcc : " << video_grabber.getSourceEncoding() << std::endl;
    std::cout << "AVI(ch0) frames : " << video_grabber.getFrameCount() << std::endl;
    std::cout << "AVI(ch0) pos_rel: " << video_grabber.getPositionRel() << std::endl;
    std::cout << "AVI(ch0) pos_abs: " << video_grabber.getPositionAbs() << std::endl;
    std::cout << "AVI(ch0) FPS    : " << video_grabber.getSourceFps() << std::endl;

    //check framerate of the grabber-thred (thread isn't started yet)
    std::cout << "\nVideoGrabber thread FPS: " << video_grabber.getFps() << std::endl;

    //disable loop
    //video_grabber.setLoop(false);


    //Set the name for the recording file
    //This function have special cases
    //In the mono case, filename is used without changes
    //In the stereo case, the filenames are constructed like
    //filename[ch0].ext and filename[ch1].ext
    //If you want to set the names independently, you can use it with the camera-parameter
    video_grabber.setRecordName("record.avi");
    //record-filenames now set to record[ch0].avi and record[ch1].avi


    //Set Snapshotnames without channel-number (in mono-case: default value is 0)
    //This is the same behavior as setRecordName()
    //The type of the file (e.g. bitmap or jpg or something else) depend on extension
    //look at the documentation of setSnapshotName for details
    video_grabber.setSnapshotName("snap.bmp");  // snap[ch0].bmp and snap[ch1].bmp
    video_grabber.setSnapshotName(0,"snap01.jpg"); // rename channel 0 to snap.jpg. Channel 1 isn't altered


    //Check the constructed filenames
    std::cout << "Check filenames of snapshots and recordings\n" << std::endl;

    std::cout << "SnapshotName_0:\t" << video_grabber.getSnapshotName(0) <<std::endl;
    std::cout << "SnapshotName_1:\t" << video_grabber.getSnapshotName(1) <<std::endl;
    std::cout << "RecordName_0:\t" << video_grabber.getRecordName(0) <<std::endl;
    std::cout << "RecordName_1:\t" << video_grabber.getRecordName(1) <<std::endl;

    //enforcing an error and catch it
    try
    {
      std::cout << "\nTry to enforce an exception:\n";
      std::cout << "SnapshotName_2: " << video_grabber.getSnapshotName(2) <<std::endl;
    }
    catch (cedar::aux::exc::ExceptionBase& e)
    {
      //std::cout << "Exception: " <<e.what() << std::endl; //until now: buggy cedar implementation
      //all grabber exceptions (look at cedar/devices/sensors/visual/exceptions/namespace.h)
      //can be used without separate include-files
      std::cout <<e.exceptionInfo()<<std::endl;
    }

    // Save a snapshot of the current images
    //video_grabber.saveSnapshotAllCams();

    // Save a snapshot from channel 0 only
    // Be aware, that already existing files will be overwritten without question
    //video_grabber.saveSnapshot(0);


    std::cout << "\nGrabbing and scrolling in the Video-File\n";
    // until now, only the first frame of the AVI-file is grabbed.
    // so lets grab the next frame into the buffer
    //video_grabber.grab();

    // and save channel 0 under a different filename
    //video_grabber.setSnapshotName(0,"snap02.jpg");
    //video_grabber.saveSnapshot(0);

    // Scrolling in the avi-file to particular positions
    //TODO: TEST set/get Position Rel/Abs
    //    Until now:
    //    it depends on the avi-file and/or fileformat
    //    if scrolling works or not
    //    so please use it carefully
    std::cout << "\nGrab the second frame (frame no. 1):"<< std::endl;
    video_grabber.grab();
    std::cout << "AVI(ch0) pos_rel: " << video_grabber.getPositionRel() << std::endl;
    std::cout << "AVI(ch0) pos_abs: " << video_grabber.getPositionAbs() << std::endl;

    //goto the half position
    std::cout << "\nScrolling to frame 50:"<< std::endl;
    video_grabber.setPositionAbs(50);
    std::cout << "AVI(ch0) pos_rel: " << video_grabber.getPositionRel() << std::endl;
    std::cout << "AVI(ch0) pos_abs: " << video_grabber.getPositionAbs() << std::endl;

  //goto the end, i.e. the last frame
    std::cout << "\nScrolling to the end of the file:"<< std::endl;
    video_grabber.setPositionRel(1);
    std::cout << "AVI(ch0) pos_rel: " << video_grabber.getPositionRel() << std::endl;
    std::cout << "AVI(ch0) pos_abs: " << video_grabber.getPositionAbs() << std::endl;


    //switch back to start
    std::cout << "\nScrolling to the beginning of the video-file:"<< std::endl;
    video_grabber.setPositionRel(0);
    std::cout << "AVI(ch0) pos_rel: " << video_grabber.getPositionRel() << std::endl;
    std::cout << "AVI(ch0) pos_abs: " << video_grabber.getPositionAbs() << std::endl;

    std::cout << "AVI-parameter (channel 0) position abs: "<<video_grabber.getSourceProperty(CV_CAP_PROP_POS_FRAMES,0)<< std::endl;
    std::cout << "AVI-parameter (channel 1) position abs: "<<video_grabber.getSourceProperty(CV_CAP_PROP_POS_FRAMES,1)<< std::endl;

    // Relative scrolling in the range 0..1
    //video_grabber.setPositionRel(0.8800);

    // Absolute scrolling in the range from 0..getFrameCount()
    //video_grabber.setPositionAbs(100);

    // Check position again
    //std::cout << "AVI(ch0) pos_rel: " << video_grabber.getPositionRel() << std::endl;
    //std::cout << "AVI(ch0) pos_abs: " << video_grabber.getPositionAbs() << std::endl;


//    return 0;

    //Start recording
    //gi->startRecording(ag->getAviParamFps());




  //some options
  video_grabber.setLoop(true);


  //------------------------------------------------------------------
  //Create an OpenCV highgui window to show grabbed frames
  std::cout << "\nShow videos\n";
  namedWindow(window0,CV_WINDOW_KEEPRATIO);
  namedWindow(window1,CV_WINDOW_KEEPRATIO);

  //the first frame is already grabbed on initialization
  cv::Mat frame0 = video_grabber.getImage();
  cv::Mat frame1 = video_grabber.getImage(1);

  //start the grabbing-thread 2 times faster then normal
  std::cout << "\nSet speed factor of grabbing-thread to 2\n";
  video_grabber.setSpeedFactor(2);
  std::cout << "VideoGrabber thread FPS    : " << video_grabber.getFps() << std::endl;
  video_grabber.start();

  //enable Recording
  //The FPS of Recording is independent from GrabbingThread.

  //recording with actual speed (depends on speed factor)
  video_grabber.startRecording(video_grabber.getFps(),CV_FOURCC('M','P','4','2'));

  //recording with speed of avi-file
  //video_grabber.startRecording(video_grabber.getSourceFps(),CV_FOURCC('M','P','4','2'));

  //recording with fixed framerate
  //video_grabber.startRecording(70,CV_FOURCC('M','P','4','2'));

  std::cout << "\nShow rest of videos\n";
  //get frames until avi is over
  while (!frame0.empty())
  {
    imshow(window0,frame0);
    imshow(window1,frame1);

    //std::cout << "AVI(ch0) pos_rel: " << video_grabber.getPositionRel() << std::endl;
    //std::cout << "AVI(ch0) pos_abs: " << video_grabber.getPositionAbs() << std::endl;

    frame0 = video_grabber.getImage();
    frame1 = video_grabber.getImage(1);

    //wait 10ms (needed for highgui)
    //you can change this to 100 or 500, and see the difference
    waitKey(10);
  }


  //------------------------------------------------------------------
  //clean up highgui
  destroyWindow(window0);
  destroyWindow(window1);


  //for configfile
  video_grabber.setSpeedFactor(1);;

  std::cout << "finished\n";

  return 0;
}
