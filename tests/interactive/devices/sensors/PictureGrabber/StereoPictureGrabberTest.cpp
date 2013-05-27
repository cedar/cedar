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

// SYSTEM INCLUDES
#include <QtGui/QApplication>


int main(int argc, char* argv[])
{
  //--------------------------------------------------------------------------------------------------------------------
  //constants
  //--------------------------------------------------------------------------------------------------------------------

  const std::string FILE_NAME_0 = "/usr/share/wallpapers/Vector_Sunset/contents/images/1024x768.jpg";
  const std::string FILE_NAME_1 = "/usr/share/wallpapers/Vector_Sunset/contents/images/1280x1024.jpg";

  const std::string GRABBER_NAME = "Stereo_Picture_Grabber_TestCase";
  const std::string CONFIG_FILE_NAME = "stereo_picture_grabber_testcase.config";

  const std::string window_title0 = FILE_NAME_0;
  const std::string window_title1 = FILE_NAME_1;

  //--------------------------------------------------------------------------------------------------------------------
  //main test
  //--------------------------------------------------------------------------------------------------------------------

  std::cout << "\n\nInteractive test of the PictureGrabber class (stereo)\n";
  std::cout << "-----------------------------------------------------\n\n";

  //----------------------------------------------------------------------------------------
  //Create the grabber
  //----------------------------------------------------------------------------------------
  std::cout << "Create a PictureGrabber:\n";
  cedar::dev::sensors::visual::PictureGrabber* p_grabber = NULL;
  try
  {
    // create grabber with a filename
    p_grabber = new cedar::dev::sensors::visual::PictureGrabber(FILE_NAME_0,FILE_NAME_1);
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


  // It is possible to create a snaptshot of the pics.
  // In the case, you set another extenstion as in the source, the image will be converted
  // The type of the file (e.g. bitmap or jpg or something else) depend on extension
  // look at the documentation of setSnapshotName for details
  p_grabber->setSnapshotName("snap.bmp");  // snap_ch0.bmp and snap_ch1.bmp
  p_grabber->setSnapshotName(0,"snap01.jpg"); // rename channel 0 to snap.jpg. Channel 1 isn't altered

  // Check the constructed filenames
  std::cout << "\nCheck filenames of snapshots" << std::endl;
  std::cout << "\tChannel0:" << p_grabber->getSnapshotName() <<std::endl;
  std::cout << "\tChannel1:" << p_grabber->getSnapshotName(1) <<std::endl;

  // Save a snapshot of the current images
  // Be aware, that existing files will be overwritten without any question
  //p_grabber->saveSnapshotAllCams();


  //----------------------------------------------------------------------------------------
  //Create a cedar::aux::gui ImagePlot widget to show grabbed frames
  //----------------------------------------------------------------------------------------
  std::cout << "\nShow pictures\n";
  //the first frame is already grabbed on initialization
  cv::Mat frame0 = p_grabber->getImage();
  cv::Mat frame1 = p_grabber->getImage(1);

  QApplication app(argc, argv);
  cedar::aux::gui::ImagePlotPtr p_plot0 = cedar::aux::gui::ImagePlotPtr(new cedar::aux::gui::ImagePlot());
  cedar::aux::MatDataPtr p_data0 = cedar::aux::MatDataPtr(new cedar::aux::MatData(frame0));
  p_plot0->plot(p_data0,window_title0);
  p_plot0->show();
  p_plot0->resize(frame0.cols,frame0.rows);

  cedar::aux::gui::ImagePlotPtr p_plot1 = cedar::aux::gui::ImagePlotPtr(new cedar::aux::gui::ImagePlot());
  cedar::aux::MatDataPtr p_data1 = cedar::aux::MatDataPtr(new cedar::aux::MatData(frame1));
  p_plot1->plot(p_data1,window_title1);
  p_plot1->show();
  p_plot1->resize(frame1.cols,frame1.rows);

  while (QApplication::hasPendingEvents())
  {
    QApplication::processEvents();
  }

  while (!frame0.empty() && !frame1.empty() && p_plot0->isVisible() && p_plot1->isVisible())
  {
    while (QApplication::hasPendingEvents())
    {
      QApplication::processEvents();
    }

    //nothing else to do here. No new frames

    cedar::aux::sleep(cedar::unit::Microseconds(1));
  }

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
