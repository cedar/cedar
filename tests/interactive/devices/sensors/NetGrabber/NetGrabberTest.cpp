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

 ----- File:        NetGrabberTest.cpp

 ----- Author:      Georg Hartinger
 ----- Email:       georg.hartinger@ini.rub.de
 ----- Date:        2011 08 01

 ----- Description: Simple application to grab from a Yarp-server (mono-case)

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "cedar/devices/sensors/visual/NetGrabber.h"
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
    std::cout << "\n\nInteractive test for the NetGrabber class.\n"
        << "------------------------------------------\n\n"
        << "This grabber get his frames from a yarp-server with the given channelname\n\n"
        << "Usage: \t" << programName << " <YarpChannelName>\n"
        << "\nRemarks: To create a yarp-stream, you could use the program \"interactiveTest_NetTransmitter\"\n"
        << "         In order to use this grabber, a YARP-server have to be up and running.\n"
        << std::endl;
  }

}

// ---------------------------------------------------------------------------------------------------------------------
// Interactive test program
// ---------------------------------------------------------------------------------------------------------------------

/*! @file Interactive test for the NetGrabber class
 *
 *    This grabber get his frames from a yarp-server with the given channelname
 *  @remarks
 *    To create a yarp-stream, you could use the program interactiveTest_NetTransmitter
 *    In order to use this grabber, a YARP-server have to be up and running.
 */

int main(int argc, char* argv[])
{
  //--------------------------------------------------------------------------------------------------------------------
  //constants
  //--------------------------------------------------------------------------------------------------------------------

  const std::string GRABBER_NAME = "NetGrabber_testcase";
  const std::string CONFIG_FILE_NAME = "NetGrabber_testcase.config";

  //--------------------------------------------------------------------------------------------------------------------
  //main test
  //--------------------------------------------------------------------------------------------------------------------

  //get name of yarp-channel
  if (argc < 2)
  {
    showUsage(argv[0]);
    return -1;
  }

  const std::string channel_name = std::string(argv[1]);
  const std::string window_title = "NetGrabber: \""+ channel_name + "\"";

  std::cout.setf(std::ios::fixed,std::ios::floatfield);
  std::cout.precision(3);

  std::cout << "\n\nInteractive test of the NetGrabber class\n";
  std::cout << "-----------------------------------------------------\n\n";
  std::cout << "Channel 0: " << channel_name;

  //----------------------------------------------------------------------------------------
  //Create the grabber
  //----------------------------------------------------------------------------------------

  std::cout << "Create a NetGrabber:\n";
  cedar::dev::sensors::visual::NetGrabberPtr p_grabber;
  try
  {
    p_grabber = cedar::dev::sensors::visual::NetGrabberPtr
                (
                  new cedar::dev::sensors::visual::NetGrabber(channel_name)
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

  // install crash-handler to close the yarp-channel in case of program termination
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
  //p_grabber->grab();

  //the first frame is now grabbed and could be read
  //always use the QReadWriteLock for locking the cv::Mat image object on access
  QReadWriteLock* p_lock = p_grabber->getReadWriteLockPointer();

  //the local image buffer
  cv::Mat frame0;

  //get the picture from the grabber
  p_lock->lockForRead();
  frame0 = p_grabber->getImage();
  p_lock->unlock();

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
  //start the grabber-thread for updating camera images
  //----------------------------------------------------------------------------------------

  p_grabber->start();

  //----------------------------------------------------------------------------------------
  // send frames and show them on screen until window will be closed
  //----------------------------------------------------------------------------------------

  unsigned int counter_stat = 0;

  //get frames until avi is over, but here: never because we have set loop to true
  //close window to finish test
  while (!frame0.empty() && p_plot->isVisible())
  {
    //process the events generated inside QT-Framework
    processQtEvents();

    p_lock->lockForRead();
    p_data->lockForWrite();
    frame0 = p_grabber->getImage();
    p_data->unlock();
    p_lock->unlock();

    //status
    if (!(++counter_stat %= 100))
    {
      std::cout << "Measured FPS: " << p_grabber->getMeasuredFramerate() << std::endl;
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
  p_grabber->stop();

  //delete p_grabber is done via the shared-pointer class
  std::cout << "finished\n";

  return 0;
}

