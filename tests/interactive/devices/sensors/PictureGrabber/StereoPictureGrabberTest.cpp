/*========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        Stereopicture_grabberTest.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Simple application to grab from a Picture (stereo-case)

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
//#include "../../../../../../cedar/auxiliaries/LogFile.h"
//#include "../../../../../../cedar/devices/sensors/visual/PictureGrabber.h"
#include <devices/sensors/visual/PictureGrabber.h>
#include <auxiliaries/LogFile.h>

// PROJECT INCLUDES


// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>



/*! \brief This is a short example of how to use the PictureGrabber class as a stereo grabber.
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
  //const char* filename0 = "/usr/local/src/OpenCV-2.2.0/samples/c/cat.jpg"; //error in picture
  const char* filename0 = "/usr/local/src/OpenCV-2.2.0/samples/c/puzzle.png";
  const char* filename1 = "/usr/local/src/OpenCV-2.2.0/samples/c/fruits.jpg";
  const char* grabbername = "Stereopicture_grabber";
  const char* configfile ="stereopicture_grabber.configfile";

  std::string window0 = (std::string)grabbername+":"+filename0;
  std::string window1 = (std::string)grabbername+":"+filename1;


  std::cout << "\n\nInteractive test of the PictureGrabber class (stereo)\n";
  std::cout << "-----------------------------------------------------\n\n";


  //------------------------------------------------------------------
  //Create the grabber
  std::cout << "Create a PictureGrabber:\n";
  cedar::dev::sensors::visual::PictureGrabber picture_grabber(configfile,filename0,filename1);


  //------------------------------------------------------------------
  /*After initialization of a picture_grabber:
   *
   * ALWAYS:
   *  - the first frame is already grabbed, so you can check the file using
   *    getImage(), getSize(), getAviParam() or something else
   *
   * EITHER:
   *  No or new configuration file:
   *  - loopedThread isn't running (start auto-grabbing with start() )
   *  - grabbername is set to default, i.e. picture_grabber
   *
   * OR:
   *  Parameters loaded from configfile
   *  - thread isn't running
   *  - grabbername is restored from configfile
   */

    picture_grabber.setName(grabbername);

    std::cout << "\nGrab from (0) " << picture_grabber.getSourceInfo(0)<< std::endl;
    std::cout << "Grab from (1) " << picture_grabber.getSourceInfo(1)<< std::endl;

    //error, because it is only a stereo-grabber
    //std::cout << "Grab from (3):" << picture_grabber.getSourceInfo(2);


    std::cout << "\nSize of loaded Pictures:\n";
    cv::Size ch0_size = picture_grabber.getSize(0);
    cv::Size ch1_size = picture_grabber.getSize(1);
    std::cout << "Channel 0: " << ch0_size.width <<" x " << ch0_size.height << std::endl;
    std::cout << "Channel 1: " << ch1_size.width <<" x " << ch1_size.height << std::endl;


    //check framerate of the grabber-thred (thread isn't started yet)
    std::cout << "\nPictureGrabber thread FPS : " << picture_grabber.getFps() << std::endl;

    //grabberthread don't have to be started. There is no new content to grab.

    //Nevertheless you can create an avi file from your picture-grabbing
    //In this case you have to start the thread
    picture_grabber.setRecordName("RecordedPictureGrabber.avi");

    //It is possible to create a snaptshot of the pics.
    //In the case, you set another extenstion as in the source, the image will be converted

    //Set Snapshotnames without channel-number (in mono-case: default value is 0)
    //This is the same behavior as setRecordName()
    //The type of the file (e.g. bitmap or jpg or something else) depend on extension
    //look at the documentation of setSnapshotName for details
    picture_grabber.setSnapshotName("snap.bmp");  // snap[ch0].bmp and snap[ch1].bmp
    picture_grabber.setSnapshotName(0,"snap01.jpg"); // rename channel 0 to snap.jpg. Channel 1 isn't altered

    //Check the constructed filenames
    std::cout << "Check filenames of snapshots and recordings\n" << std::endl;

    std::cout << "SnapshotName_0:\t" << picture_grabber.getSnapshotName(0) <<std::endl;
    std::cout << "SnapshotName_1:\t" << picture_grabber.getSnapshotName(1) <<std::endl;
    std::cout << "RecordName_0:\t" << picture_grabber.getRecordName(0) <<std::endl;
    std::cout << "RecordName_1:\t" << picture_grabber.getRecordName(1) <<std::endl;

    //enforcing an error and catch it
    try
    {
      std::cout << "\nTry to enforce an exception:\n";
      std::cout << "SnapshotName_2: " << picture_grabber.getSnapshotName(2) <<std::endl;
    }
    catch (cedar::aux::exc::ExceptionBase& e)
    {
      //std::cout << "Exception: " <<e.what() << std::endl; //until now: buggy cedar implementation
      //all grabber exceptions (look at cedar/devices/sensors/visual/exceptions/namespace.h)
      //can be used without separate include-files
      std::cout <<e.exceptionInfo()<<std::endl;
    }

    // Save a snapshot of the current images
    //picture_grabber.saveSnapshotAllCams();

    // Save a snapshot from channel 0 only
    // Be aware, that existing files will be overwritten without any question
    //picture_grabber.saveSnapshot(0);


  //------------------------------------------------------------------
  //Create an OpenCV highgui window to show grabbed frames
  std::cout << "\nShow pictures\n";
  namedWindow(window0,CV_WINDOW_KEEPRATIO);
  namedWindow(window1,CV_WINDOW_KEEPRATIO);

  //the first frame is already grabbed on initialization
  cv::Mat frame0 = picture_grabber.getImage();
  cv::Mat frame1 = picture_grabber.getImage(1);

  //start recording
  std::cout << "\nStart Recording\n";
  picture_grabber.setFps(15);
  picture_grabber.start();
  picture_grabber.startRecording(15);


  //get frames for a while
  unsigned int counter=0;

  while (!frame0.empty())
  {
    imshow(window0,frame0);
    imshow(window1,frame1);

    //it is not necessary to do this, unless a new picture should be used
    frame0 = picture_grabber.getImage();
    frame1 = picture_grabber.getImage(1);
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
      picture_grabber.setSourceFile(0,filename1);
      picture_grabber.setSourceFile(1,filename0);
    }

    //exit after another second
    if (counter == 20)
    {
      break;
    }

    //wait 100ms (needed for highgui)
    waitKey(100);
  }

  //stop grabbing-thread if running
  if (picture_grabber.isRunning())
  {
    picture_grabber.stop();
  }

  //------------------------------------------------------------------
  //clean up highgui
  destroyWindow(window0);
  destroyWindow(window1);
  std::cout << "finished\n";

  return 0;
}
