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

 ----- File:        NetTransmitter.cpp

 ----- Author:      Georg Hartinger
 ----- Email:       georg.hartinger@ini.rub.de
 ----- Date:        2013 01 17

 ----- Description: Application to grab from an AVI-file and transmit the frames through a YARP-channel

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/


// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/VideoGrabber.h"
#include "cedar/auxiliaries/gui/ImagePlot.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/auxiliaries/net/Writer.h"
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
    std::cout << "\n\nSimple program to transmit the frames of a video file over the network through a YARP-channel\n\n"
        << "Usage: \t" << programName << " <ChannelName> <VideoFile>\n"
        << std::endl;
  }
}

// ---------------------------------------------------------------------------------------------------------------------
// Main program
// ---------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{

  //--------------------------------------------------------------------------------------------------------------------
  //constants
  //--------------------------------------------------------------------------------------------------------------------

  const std::string GRABBER_NAME = "YarpVideoGrabber";
  const std::string CONFIG_FILE_NAME = "YarpVideoGrabber.config";

  //--------------------------------------------------------------------------------------------------------------------
  //main test
  //--------------------------------------------------------------------------------------------------------------------
  //get filename of video
  if (argc < 3)
  {
    showUsage(argv[0]);
    return -1;
  }

  const std::string channel_name = std::string(argv[1]);
  const std::string filename_channel0 = std::string(argv[2]);
  const std::string window_title = "YARP-channel: \""+ channel_name + "\"";

  std::cout.setf(std::ios::fixed,std::ios::floatfield);
  std::cout.precision(3);

  std::cout << "\n\nSimple program to transmit the frames of a video file over the network through a YARP-channel\n";

  //----------------------------------------------------------------------------------------
  //Create the grabber and grab first frame
  //----------------------------------------------------------------------------------------

  std::cout << "Create the VideoGrabber:\n";
  cedar::dev::sensors::visual::VideoGrabberPtr p_grabber;
  try
  {
    p_grabber = cedar::dev::sensors::visual::VideoGrabberPtr
                (
                  new cedar::dev::sensors::visual::VideoGrabber(filename_channel0)
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

  if (! p_grabber->applyParameter())
  {
    // an error occured during initialization. Perhaps the file doesn't exist
    return -1;
  }

  QReadWriteLock* p_lock = p_grabber->getReadWriteLockPointer();
  cv::Mat frame0;
  p_lock->lockForRead();
  frame0 = p_grabber->getImage();
  p_lock->unlock();

  //start the grabbing-thread.
  p_grabber->setSpeedFactor(1);
  p_grabber->setLooped(true);
  p_grabber->start();

  std::cout << "VideoGrabber started" << std::endl;

  //----------------------------------------------------------------------------------------
  // create a yarp-transmitter
  //----------------------------------------------------------------------------------------

  std::cout << "Create the NetWriter" << std::endl;

  cedar::aux::net::Writer<cv::Mat>* p_net_writer = NULL;
  try
  {
    p_net_writer = new cedar::aux::net::Writer<cv::Mat>(channel_name);
  }
  catch(...)
  {
    std::cout << "ERROR: Can't create Netwriter with channelname : "<<channel_name<<"\n\n";
    if (p_net_writer)
    {
      delete p_net_writer;
      p_net_writer = NULL;
    }
    return -1;
  }

  //send first frame
  p_net_writer->write(frame0);
  std::cout << "First frame sent" << std::endl;

  //----------------------------------------------------------------------------------------
  // Create a cedar::aux::gui ImagePlot widget to show grabbed frames
  //----------------------------------------------------------------------------------------

  QApplication app(argc, argv);
  cedar::aux::gui::ImagePlotPtr p_plot = cedar::aux::gui::ImagePlotPtr(new cedar::aux::gui::ImagePlot());
  cedar::aux::MatDataPtr p_data = cedar::aux::MatDataPtr(new cedar::aux::MatData(frame0));
  p_plot->plot(p_data,window_title);
  p_plot->setWindowTitle(QString::fromStdString(window_title));
  p_plot->show();
  p_plot->resize(frame0.cols,frame0.rows);

  //process the events generated inside QT-Framework
  processQtEvents();

  //----------------------------------------------------------------------------------------
  // Send frames until the window will be closed by the user
  //----------------------------------------------------------------------------------------

  //get the timediff between two frames
  std::cout << "Videoframe: " << p_grabber->getSourceFramerate() << std::endl;
  double frame_time_ms = 1000.f/p_grabber->getSourceFramerate();
  boost::posix_time::ptime time_start;

  unsigned int counter = 0;
  //get frames until avi is over, but here: never because we have set loop to true
  //close window to finish test
  while (!frame0.empty() && p_plot->isVisible())
  {
    time_start = boost::posix_time::microsec_clock::local_time();

    p_lock->lockForRead();
    p_data->lockForWrite();
    frame0 = p_grabber->getImage();
    p_data->unlock();
    p_lock->unlock();

    //process the events generated inside QT-Framework
    processQtEvents();

    //send
    p_net_writer->write(frame0);

    //every 20 frames
    if (!(++counter %= 20))
    {
      std::cout << "Grabbing framerate: " << p_grabber->getMeasuredFramerate() << " fps" << std::endl;
    }

    //send frames with equal framerate like the video, do busy-waiting instead of an extra thread
    while
    (
      boost::posix_time::time_duration
      (
        boost::posix_time::microsec_clock::local_time() - time_start
      ).total_milliseconds() < (frame_time_ms)
    )
    {
      cedar::aux::sleep(cedar::unit::Time(10.0 * cedar::unit::milli * cedar::unit::seconds));
    }
  }


  //----------------------------------------------------------------------------------------
  //clean up
  //----------------------------------------------------------------------------------------

  //stop grabbing-thread if running
  //recording will also be stopped
  p_grabber->stop();

  if (p_net_writer)
  {
    delete p_net_writer;
    p_net_writer = NULL;
  }

  //delete p_grabber is done via the shared-pointer class
  std::cout << "finished\n";

  return 0;
}
