/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

    This file is part of cedar.

    cedar is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    cedar is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with cedar. If not, see <http://www.gnu.org/licenses/>.

========================================================================================================================

 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        StereoVideoGrabberTest.cpp

 ----- Author:      Georg Hartinger
 ----- Email:       georg.hartinger@ini.rub.de
 ----- Date:        2011 08 01

 ----- Description: Simple application to grab from an AVI-file (stereo-case)

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/VideoGrabber.h"
#include "cedar/auxiliaries/gui/ImagePlot.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#include <QApplication>
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
    std::cout << "\n\nInteractive test for the VideoGrabber class (stereo).\n\n"
        << "Usage: \t" << programName << " <VideoFile Ch.0> <VideoFile Ch.1>\n"
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

  const std::string GRABBER_NAME = "StereoVideoGrabber_testcase";
  const std::string CONFIG_FILE_NAME = "StereoVideoGrabber_testcase.config";

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
  const std::string window_title = "StereoVideoGrabber: \""+ filename_channel0 + "\", \"" + filename_channel1+"\"";

  std::cout.setf(std::ios::fixed,std::ios::floatfield);
  std::cout.precision(3);

  std::cout << "\n\nInteractive test of the VideoGrabber class (stereo)\n";
  std::cout << "--------------------------------------------\n\n";


  //----------------------------------------------------------------------------------------
  //Create the grabber
  //----------------------------------------------------------------------------------------

  std::cout << "Create a VideoGrabber:\n";
  cedar::dev::sensors::visual::VideoGrabberPtr p_grabber;
  try
  {
    p_grabber = cedar::dev::sensors::visual::VideoGrabberPtr
                (
                  new cedar::dev::sensors::visual::VideoGrabber(filename_channel0, filename_channel1)
                );
  }
  catch (cedar::aux::InitializationException &e)
  {
    //after an InitializationExeception the p_grabber class isn't initialized correctly
    //and can't be used
    std::cout << "Error on creation of the VideoGrabber class:\n"
              << e.exceptionInfo() << std::endl;

    return -1;
  }

  // install crash-handler not necessary. No hardware and no files for writing needed
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
  // grab first frame
  //----------------------------------------------------------------------------------------

  //the first frame is already grabbed on initialization
  //p_grabber->grab() not needed

  //the first frame is now grabbed and could be read
  //always use the QReadWriteLock for locking the cv::Mat image object on access
  QReadWriteLock* p_lock = p_grabber->getReadWriteLockPointer();

  //the local image buffer
  cv::Mat frame0;
  cv::Mat frame1;

  //get the picture from the grabber
  p_lock->lockForRead();
  frame0 = p_grabber->getImage();
  frame1 = p_grabber->getImage(1);
  p_lock->unlock();

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
  // Create a cedar::aux::gui ImagePlot widget to show grabbed frames
  //----------------------------------------------------------------------------------------
  std::cout << "\nShow frames - close grabber window to exit" << std::endl;

  QApplication app(argc, argv);
  cedar::aux::gui::ImagePlotPtr p_plot = cedar::aux::gui::ImagePlotPtr(new cedar::aux::gui::ImagePlot());
  cedar::aux::MatDataPtr p_data = cedar::aux::MatDataPtr(new cedar::aux::MatData(stereo_pic));
  p_plot->plot(p_data, window_title);
  p_plot->setWindowTitle(QString::fromStdString(window_title));
  p_plot->show();
  p_plot->resize(stereo_pic.cols,stereo_pic.rows);

  //process events of the ImagePlot widget
  processQtEvents();

  //----------------------------------------------------------------------------------------
  //start the grabber-thread for updating camera images with 30 fps
  //----------------------------------------------------------------------------------------

  //start the grabbing-thread. It is possible to set a speedfactor
  p_grabber->setSpeedFactor(1);
  p_grabber->setLooped(true);

  std::cout << "Video FPS              : " << p_grabber->getSourceFramerate() << std::endl;
  std::cout << "VideoGrabber thread FPS: " << p_grabber->getFramerate() << std::endl;
  p_grabber->start();

  unsigned int counter_stat = 0;

  //get frames until avi is over
  //here: never because we have set loop to true
  while (!frame0.empty() && !frame1.empty() && p_plot->isVisible())
  {
    //process the events generated inside QT-Framework
    processQtEvents();

    //get new images, this is independent from grabbing-thread
    p_lock->lockForRead();
    p_data->lockForWrite();
    frame0 = p_grabber->getImage();
    frame1 = p_grabber->getImage(1);

    //copy to stereo picture for plot
    frame0.copyTo(left_pic);
    frame1.copyTo(right_pic);
    p_data->unlock();
    p_lock->unlock();

    //status
    if (!(++counter_stat %= 100) )
    {
      std::cout << "Measured FPS: " << p_grabber->getMeasuredFramerate()
                << "\tPos_Rel: "<< p_grabber->getPositionRelative()
                << "\tPos_Abs: "<< p_grabber->getPositionAbsolute()
                << std::endl;
    }

    // watch the output - everything should be fine.
    //The video is much slower than 100 fps.
    cedar::aux::sleep(cedar::unit::Time(10.0 * cedar::unit::milli * cedar::unit::seconds));
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
  if (p_grabber->isRunningNolocking())
  {
    p_grabber->stop();
  }

  // p_grabber is deleted in the shared-pointer class
  std::cout << "finished\n";

  return 0;
}
