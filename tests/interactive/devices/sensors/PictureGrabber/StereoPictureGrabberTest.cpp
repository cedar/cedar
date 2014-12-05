/*========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        StereoPictureGrabberTest.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Simple application to grab from a Picture (stereo-case)

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/PictureGrabber.h"
#include "cedar/auxiliaries/gui/ImagePlot.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#include <QApplication>

// ---------------------------------------------------------------------------------------------------------------------
// Local methods
// ---------------------------------------------------------------------------------------------------------------------
namespace
{
  void processQtEvents()
  {
#ifdef CEDAR_OS_APPLE
    unsigned int event_counter = 0;
    while (QApplication::hasPendingEvents() && (++event_counter < 1000))
#else
    while (QApplication::hasPendingEvents())
#endif
    {
      QApplication::processEvents();
    }
  }

  void showUsage(std::string programName)
  {
    std::cout << "\n\nInteractive test for the PictureGrabber class (stereo).\n\n"
        << "Usage: \t" << programName << " <PictureFile> <PictureFile2>\n"
        << std::endl;
  }
}

// ---------------------------------------------------------------------------------------------------------------------
// Interactive test program
// ---------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  //--------------------------------------------------------------------------------------------------------------------
  //constants
  //--------------------------------------------------------------------------------------------------------------------

  const std::string GRABBER_NAME = "stereo_picture_grabber_testcase";
  const std::string CONFIG_FILE_NAME = "stereo_picture_grabber_testcase.config";

  //--------------------------------------------------------------------------------------------------------------------
  //main test
  //--------------------------------------------------------------------------------------------------------------------

  //get filename of video
  if (argc < 3)
  {
    showUsage(argv[0]);
    return -1;
  }

  const std::string filename_channel0 = std::string(argv[1]);
  const std::string filename_channel1 = std::string(argv[2]);
  std::string window_title = "Stereo_PictureGrabber: \""+ filename_channel0 + "\" \""+ filename_channel1 + "\"";

  std::cout.setf(std::ios::fixed,std::ios::floatfield);
  std::cout.precision(3);

  std::cout << "\n\nInteractive test of the PictureGrabber class (stereo)\n";
  std::cout << "-----------------------------------------------------\n\n";
  std::cout << window_title << std::endl;
  std::cout << "file0: " << filename_channel0 << std::endl;
  std::cout << "file1: " << filename_channel1 << std::endl;

  //----------------------------------------------------------------------------------------
  //Create the grabber
  //----------------------------------------------------------------------------------------
  std::cout << "Create a PictureGrabber:\n";
  cedar::dev::sensors::visual::PictureGrabberPtr p_grabber;
  try
  {
    p_grabber = cedar::dev::sensors::visual::PictureGrabberPtr
                (
                  new cedar::dev::sensors::visual::PictureGrabber(filename_channel0,filename_channel1)
                );
  }
  catch (cedar::aux::InitializationException &e)
  {
    //after an InitializationExeception the p_grabber class isn't initialized correctly
    //and can't be used
    std::cout << "Error on creation of the PictureGrabber class:\n"
              << e.exceptionInfo() << std::endl;

    return -1;
  }

  // install crash-handler not necessary. No hardware and no files for writing needed
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

  //----------------------------------------------------------------------------------------
  // apply configuration. this step is mandatory.
  //----------------------------------------------------------------------------------------
  // if you don't apply the default or loaded Parameter, the grabber will not work
  // check if grabber is created successfully
  if (! p_grabber->applyParameter())
  {
    // an error occured during initialization. Perhaps the file doesn't exist
    return -1;
  }

  //----------------------------------------------------------------------------------------
  // change settings and properties
  //----------------------------------------------------------------------------------------

  /*After initialization of a picture grabber:
   *
   *  The picture is already initialized and the picture is grabbed, so you can check the file using
   *  getImage(), getSize() or something else
   *
  */

  // Set the name of our grabber
  p_grabber->setName(GRABBER_NAME);

  std::cout << "\nGrab Channel 0 from: " << p_grabber->getSourceInfo(0)<< std::endl;
  std::cout << "Grab Channel 1 from: " << p_grabber->getSourceInfo(1)<< std::endl;

  // check filesize
  std::cout << "\nSize of loaded Pictures:\n";
  cv::Size ch0_size = p_grabber->getSize(0);
  cv::Size ch1_size = p_grabber->getSize(1);
  std::cout << "\tChannel 0: " << ch0_size.width <<" x " << ch0_size.height << std::endl;
  std::cout << "\tChannel 1: " << ch1_size.width <<" x " << ch1_size.height << std::endl;


  // It is possible to create a snapshot of the pics.
  // In the case, you set another extension as in the source, the image will be converted
  // look at the documentation of setSnapshotName for details
  p_grabber->setSnapshotName("snap.bmp");  // snap_ch0.bmp and snap_ch1.bmp
  p_grabber->setSnapshotName(0,"snap.jpg"); // rename channel 0 to snap.jpg. Channel 1 isn't altered

  // Check the constructed filenames
  std::cout << "\nCheck filenames of snapshots" << std::endl;
  std::cout << "\tChannel0:" << p_grabber->getSnapshotName() <<std::endl;
  std::cout << "\tChannel1:" << p_grabber->getSnapshotName(1) <<std::endl;

  // Save a snapshot of the current images
  // Be aware, that existing files will be overwritten without any question
  //p_grabber->saveSnapshotAllCams();

  //----------------------------------------------------------------------------------------
  // read pictures from grabber to local image buffer
  //----------------------------------------------------------------------------------------

  //the local image buffer
  cv::Mat frame0;
  cv::Mat frame1;

  //get the picture from the grabber
  frame0 = p_grabber->getImage();
  frame1 = p_grabber->getImage(1);
  std::cout << "frame 0: " << frame0.cols <<" x " << frame0.rows << std::endl;
  std::cout << "frame 1: " << frame1.cols <<" x " << frame1.rows << std::endl;

  //copy both channels to one picture
  unsigned int size_w = frame0.cols + frame1.cols+5;
  unsigned int size_h = frame0.rows > frame1.rows ? frame0.rows : frame1.rows;
  cv::Mat stereo_pic =  cv::Mat::zeros(size_h, size_w, CV_8UC3);
  cv::Rect left_pic_size(0,0,frame0.cols,frame0.rows);
  cv::Mat left_pic = stereo_pic(left_pic_size);
  cv::Rect right_pic_size(frame0.cols+5,0,frame1.cols,frame1.rows);
  cv::Mat right_pic = stereo_pic(right_pic_size);
  frame0.copyTo(left_pic);
  frame1.copyTo(right_pic);


  //----------------------------------------------------------------------------------------
  //Create a cedar::aux::gui ImagePlot widget to show grabbed frames
  //----------------------------------------------------------------------------------------
  std::cout << "\nShow image - close grabber window to exit" << std::endl;

  QApplication app(argc, argv);
  cedar::aux::gui::ImagePlotPtr p_plot = cedar::aux::gui::ImagePlotPtr(new cedar::aux::gui::ImagePlot());
  cedar::aux::MatDataPtr p_data = cedar::aux::MatDataPtr(new cedar::aux::MatData(stereo_pic));
  p_plot->plot(p_data,window_title);
  p_plot->setWindowTitle(QString::fromStdString(window_title));
  p_plot->show();
  p_plot->resize(frame0.cols,frame0.rows);

  //process the events generated inside QT-Framework
  processQtEvents();

  while (p_plot->isVisible())
  {
    //process the events generated inside QT-Framework
    processQtEvents();

    //nothing else to do here. No new frames
    //but it is possible to change the frames.
    //Have a look at the single channel picture grabber for details
    cedar::aux::sleep(cedar::unit::Time(100.0 * cedar::unit::milli * cedar::unit::second));
    std::cout << "." << std::flush;
  }

  std::cout << std::endl;

  //----------------------------------------------------------------------------------------
  //clean up
  //----------------------------------------------------------------------------------------

  //stop grabbing-thread if running
  //recording will also be stopped
  p_grabber->stop();
  std::cout << "finished\n";
  return 0;
}
