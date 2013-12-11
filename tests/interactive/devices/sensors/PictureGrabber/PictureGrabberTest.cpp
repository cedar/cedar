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
#include "cedar/auxiliaries/gui/ImagePlot.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <opencv2/opencv.hpp>
#include <ios>

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
    std::cout << "\n\nInteractive test for the PictureGrabber class.\n\n"
        << "Usage: \t" << programName << " <PictureFile> [<PictureFile2>]\n"
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

  const std::string GRABBER_NAME = "picture_grabber_testcase";
  const std::string CONFIG_FILE_NAME = "picture_grabber_testcase.config";

  //--------------------------------------------------------------------------------------------------------------------
  //main test
  //--------------------------------------------------------------------------------------------------------------------

  //get filename of video
  if (argc < 2)
  {
    showUsage(argv[0]);
    return -1;
  }

  const std::string filename_channel0 = std::string(argv[1]);
  std::string filename_channel1 = "";
  std::string window_title = "PictureGrabber: \""+ filename_channel0 + "\"";

  if (argc > 2)
  {
    filename_channel1 = std::string(argv[2]);
    window_title = window_title + ", \""+ filename_channel1 + "\"";
  }

  std::cout.setf(std::ios::fixed,std::ios::floatfield);
  std::cout.precision(3);

  std::cout << "\n\nInteractive test of the PictureGrabber class (mono)\n";
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
                  new cedar::dev::sensors::visual::PictureGrabber(filename_channel0)
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
  //Create a cedar::aux::gui ImagePlot widget to show grabbed frames
  //----------------------------------------------------------------------------------------
  std::cout << "\nShow image - close grabber window to exit" << std::endl;


  //the first frame is already grabbed on initialization, so get it
  cv::Mat frame0 = p_grabber->getImage();

  QApplication app(argc, argv);
  cedar::aux::gui::ImagePlotPtr p_plot = cedar::aux::gui::ImagePlotPtr(new cedar::aux::gui::ImagePlot());
  cedar::aux::MatDataPtr p_data = cedar::aux::MatDataPtr(new cedar::aux::MatData(frame0));
  p_plot->plot(p_data, window_title);
  p_plot->setWindowTitle(QString::fromStdString(window_title));
  p_plot->show();
  p_plot->resize(frame0.cols,frame0.rows);

  //process the events generated inside QT-Framework
  processQtEvents();

  unsigned int counter = 0;
  bool file2 = false;
  while (!frame0.empty() && p_plot->isVisible())
  {
    //process the events generated inside QT-Framework
    processQtEvents();

    //after two seconds, set new source-pictures
    if (!(++counter %= 20))
    {
      if (filename_channel1 != "")
      {
        // Grab from another picture
        if (file2)
        {
          std::cout << "\nSet file 0" << std::endl;
          p_grabber->setSourceFile(0,filename_channel0);
          file2=false;
        }
        else
        {
          std::cout << "\nSet file 1" << std::endl;
          p_grabber->setSourceFile(0,filename_channel1);
          file2=true;
        }

        frame0 = p_grabber->getImage();
        cedar::aux::MatDataPtr p_data = cedar::aux::MatDataPtr(new cedar::aux::MatData(frame0));
        p_plot->plot(p_data, window_title);
        p_plot->resize(frame0.cols,frame0.rows);
        std::cout << "Grab from \"" << p_grabber->getSourceInfo()<<"\"" << std::endl;
        cv::Size ch0_size = p_grabber->getSize(0);
        std::cout << "Size of loaded picture: " << ch0_size.width <<" x " << ch0_size.height << std::endl;
      }
      else
      {
        std::cout << "." << std::flush;
      }
    }
    cedar::aux::sleep(cedar::unit::Time(100.0 * cedar::unit::milli * cedar::unit::second));
  }

  std::cout << std::endl;

  //----------------------------------------------------------------------------------------
  // save configuration. this step is optional.
  //----------------------------------------------------------------------------------------
  //p_grabber->writeJson(CONFIG_FILE_NAME);

  //----------------------------------------------------------------------------------------
  //clean up
  //----------------------------------------------------------------------------------------

  //stop grabbing-thread if running
  //recording will also be stopped
  p_grabber->stop();

  std::cout << "finished\n";

  return 0;
}
