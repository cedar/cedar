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

// SYSTEM INCLUDES
#include <QtGui/QApplication>
#include <opencv2/opencv.hpp>


int main(int argc, char* argv[])
{
  //--------------------------------------------------------------------------------------------------------------------
  //constants
  //--------------------------------------------------------------------------------------------------------------------

  const std::string FILE_NAME_0 = "/usr/share/wallpapers/Vector_Sunset/contents/images/1024x768.jpg";
  const std::string FILE_NAME_1 = "/usr/share/wallpapers/Green_Concentration/contents/screenshot.png";

  const std::string GRABBER_NAME = "picture_grabber_TestCase";
  const std::string CONFIG_FILE_NAME = "picture_grabber_testcase.config";

  //title of highgui window
  const std::string window_title = FILE_NAME_0;

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

  //----------------------------------------------------------------------------------------
  // apply configuration. this step is essential.
  //----------------------------------------------------------------------------------------
  // if you don't apply the default or loaded Parameter, the grabber will not work
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
  //Create a cedar::aux::gui ImagePlot widget to show grabbed frames
  //----------------------------------------------------------------------------------------

  //the first frame is already grabbed on initialization, so get it
  cv::Mat frame0 = p_grabber->getImage();

  QApplication app(argc, argv);
  cedar::aux::gui::ImagePlotPtr p_plot = cedar::aux::gui::ImagePlotPtr(new cedar::aux::gui::ImagePlot());
  cedar::aux::MatDataPtr p_data = cedar::aux::MatDataPtr(new cedar::aux::MatData(frame0));
  p_plot->plot(p_data,window_title);
  p_plot->show();
  p_plot->resize(frame0.cols,frame0.rows);

  while (QApplication::hasPendingEvents())
  {
    QApplication::processEvents();
  }

  unsigned int counter = 0;
  bool file2 = false;
  while (!frame0.empty() && p_plot->isVisible())
  {
    while (QApplication::hasPendingEvents())
    {
      QApplication::processEvents();
    }

    counter++;

    //after one second, set new source-pictures
    if (!(++counter %= 100))
    {
      // Grab from another picture
      if (file2)
      {
        p_grabber->setSourceFile(0,FILE_NAME_0);
        file2=false;
      }
      else
      {
        p_grabber->setSourceFile(0,FILE_NAME_1);
        file2=true;
      }
      frame0 = p_grabber->getImage();
      cedar::aux::MatDataPtr p_data = cedar::aux::MatDataPtr(new cedar::aux::MatData(frame0));
      p_plot->plot(p_data,window_title);
      p_plot->resize(frame0.cols,frame0.rows);
    }

    cedar::aux::sleep(cedar::unit::Milliseconds(100));
  }

  //----------------------------------------------------------------------------------------
  // save configuration. this step is optional.
  //----------------------------------------------------------------------------------------
  //p_grabber->writeJson(CONFIG_FILE_NAME);

  //----------------------------------------------------------------------------------------
  //clean up
  //----------------------------------------------------------------------------------------

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
