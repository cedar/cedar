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

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/VideoGrabber.h"
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

  const std::string FILE_NAME_0 = "/home/ghartinger/Videos/rhinos.avi";
  const std::string FILE_NAME_1 = "/home/ghartinger/Videos/traffic.avi";

  const std::string GRABBER_NAME = "stereo_p_grabber_testcase";
  const std::string CONFIG_FILE_NAME = "stereo_p_grabber_testcase.config";

  //title of highgui window
  const std::string window_title0 = FILE_NAME_0;
  const std::string window_title1 = FILE_NAME_1;

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
     p_grabber = new cedar::dev::sensors::visual::VideoGrabber(FILE_NAME_0, FILE_NAME_1);
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
  //Create an OpenCV highgui window to show grabbed frames
  //----------------------------------------------------------------------------------------

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

  //start the grabbing-thread. It is possible to set a speedfactor
  p_grabber->setSpeedFactor(1);


  std::cout << "VideoGrabber thread FPS    : " << p_grabber->getFps() << std::endl;
  p_grabber->start();

  unsigned int counter_stat = 0;

  //get frames until avi is over
  //here: never because we have set loop to true
  while (!frame0.empty() && !frame1.empty() && p_plot0->isVisible() && p_plot1->isVisible())
  {
    while (QApplication::hasPendingEvents())
    {
      QApplication::processEvents();
    }

    frame0 = p_grabber->getImage(0);
    frame1 = p_grabber->getImage(1);

    //status
    if (++counter_stat %= 3 )
    {
      std::cout << "Measured FPS: " << p_grabber->getFpsMeasured()
                << "\tPos_Rel: "<< p_grabber->getPositionRelative()
                << "\tPos_Abs: "<< p_grabber->getPositionAbsolute()
                << std::endl;
    }

    cedar::aux::sleep(cedar::unit::Microseconds(1));
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
  if (p_grabber->isRunning())
  {
    p_grabber->stop();
  }

  if (p_grabber)
  {
   delete p_grabber;
  }

  std::cout << "finished\n";

  return 0;
}
