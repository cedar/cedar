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

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        OglGrabberTest.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 04 26

    Description: Simple example on how to grab from a QGLWidget

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/devices/sensors/visual/GLGrabber.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/auxiliaries/gui/ImagePlot.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/gui/SceneWidget.h"
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/gl/Block.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/units/Length.h"
#include "cedar/units/PlaneAngle.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#include <QReadWriteLock>
#ifndef Q_MOC_RUN
  #include <boost/lexical_cast.hpp>
#endif
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
}

// ---------------------------------------------------------------------------------------------------------------------
// Interactive test program
// ---------------------------------------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
  // -------------------------------------------------------------------------------------------------------------------
  // Constants
  // -------------------------------------------------------------------------------------------------------------------

  // the name of the grabber
  // only used in the configuration file
  const std::string GRABBER_NAME = "GL_Grabber_Test";

  // the name for the configuration file (not needed for this test)
  const std::string CONFIG_FILE_NAME = "GL_grabber_testcase.config";

  //title of plot window
  std::string window_title = "Grabber: " + GRABBER_NAME;

  //--------------------------------------------------------------------------------------------------------------------
  //main test
  //--------------------------------------------------------------------------------------------------------------------

  std::cout.setf(std::ios::fixed,std::ios::floatfield);
  std::cout.precision(3);

  std::cout << "\n\nInteractive test of the GLGrabber class\n";
  std::cout << "--------------------------------------------\n\n";

  // -------------------------------------------------------------------------------------------------------------------
  // Create GL scene
  // -------------------------------------------------------------------------------------------------------------------

  QApplication a(argc, argv);

  // create the scene administering the objects to be visualized
  cedar::aux::gl::ScenePtr p_scene(new cedar::aux::gl::Scene);
  p_scene->setSceneLimit(10);
  p_scene->drawFloor(false);

  // create a simple viewer for the scene
  cedar::aux::gui::Viewer viewer(p_scene);
  viewer.show();
  viewer.setSceneRadius(10);
  viewer.startTimer(50);

  // create a rectangular block and add it to the scene
  cedar::aux::LocalCoordinateFramePtr p_block_local_coordinate_frame(new cedar::aux::LocalCoordinateFrame());
  p_block_local_coordinate_frame->setTranslation
  (
    3.0 * cedar::unit::meters,
    -3.0 * cedar::unit::meters,
    3.0 * cedar::unit::meters
  );
  cedar::aux::gl::ObjectVisualizationPtr p_block
  (
    new cedar::aux::gl::Block(p_block_local_coordinate_frame, 1, 2, 3, 0, 1, 0.5)
  );
  p_scene->addObjectVisualization(p_block);
  p_block_local_coordinate_frame->rotate(0, cedar::aux::math::pi/2.0 * cedar::unit::radians);

  processQtEvents();

  // -------------------------------------------------------------------------------------------------------------------
  // Create the GLGrabber
  // -------------------------------------------------------------------------------------------------------------------

  std::cout << "Create a GLGrabber:\n";
  cedar::dev::sensors::visual::GLGrabberPtr p_grabber;
  try
  {
    p_grabber = cedar::dev::sensors::visual::GLGrabberPtr
                (
                  new cedar::dev::sensors::visual::GLGrabber
                      (
                       static_cast<QGLWidget *>(&viewer),
                       GRABBER_NAME // not necessary
                      )
                );
  }
  catch (cedar::aux::InitializationException &e)
  {
    //after an InitializationExeception the video_grabber class isn't initialized correctly
    //and couldn't be used
    std::cout << "Error on creation of the VideoGrabber class:\n"
              << e.exceptionInfo() << std::endl;

    // connected with viewer, process pending events
    processQtEvents();

    // delete p_grabber is done via the shared-pointer class
    return -1;
  }

  // activate crash-handler if there is any hardware-related stuff which has to be cleaned up
  p_grabber->installCrashHandler();


  //----------------------------------------------------------------------------------------
  // load configuration. this step is optional.
  //----------------------------------------------------------------------------------------
  try
  {
    // p_grabber->readJson(CONFIG_FILE_NAME);
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
    // an error occured during initialization.
    return -1;
  }

  // allow to register the grabber in the viewer class and wait.
  for (int i = 0; i < 10; ++i)
  {
    processQtEvents();
    cedar::aux::sleep(cedar::unit::Time(10.0 * cedar::unit::milli * cedar::unit::seconds));
  }

  std::cout << "\nGrab channel 0 from \"" << p_grabber->getSourceInfo()<< "\"" << std::endl;


  //-------------------------------------------------------------------------------------------------------------------
  // grab frames
  //-------------------------------------------------------------------------------------------------------------------

  // wait for a second, to allow the viewer to draw its buffer
  std::cout << "Time to test the viewer" << std::endl;

  // grab a picture
  p_grabber->grab();

  // the first frame is now grabbed and could be read
  // always use the QReadWriteLock for locking the cv::Mat image object on access
  QReadWriteLock* p_lock = p_grabber->getReadWriteLockPointer();

  // the local image buffer
  cv::Mat frame;

  // get the picture from the grabber
  p_lock->lockForRead();
  frame = p_grabber->getImage();
  p_lock->unlock();

  //---------------------------------------------------------------------------------------
  // create a main window (cedar::aux::gui::ImagePlot) do display the grabbed images
  //---------------------------------------------------------------------------------------
  cedar::aux::gui::ImagePlotPtr p_plot = cedar::aux::gui::ImagePlotPtr(new cedar::aux::gui::ImagePlot());
  cedar::aux::MatDataPtr p_data = cedar::aux::MatDataPtr(new cedar::aux::MatData(frame));
  p_plot->plot(p_data,window_title);
  p_plot->setWindowTitle(QString::fromStdString(window_title));
  p_plot->show();
  p_plot->resize(frame.cols,frame.rows);

  // allow the ImagePlot to create
  for (int i=0; i<10; i++)
  {
    processQtEvents();
  }

  // The grabbing have to be done in the GUI-thread!
  // If you start background-grabbing, a segmentation-fault occur!

  // p_grabber->setFramerate(50);
  // p_grabber->start();

  unsigned int counter=0;
  std::cout << "\nDisplay GL frames\n";

  // get frames for a while
  while (!frame.empty() && p_plot->isVisible())
  {
    processQtEvents();

    // grab in the GUI-thread
    p_grabber->grab();

    // read new images, from the grabber-buffer. This is independent from background-thread
    // the background thread gets the images from the gl-widget periodically
    // with the getImage() method, you can get the image from the internal buffer
    // if background-thread and GL-processing is faster than your processing, images will be skipped
    p_lock->lockForRead();
    p_data->lockForWrite();
    p_data->setData(p_grabber->getImage());
    p_data->unlock();
    p_lock->unlock();

    // state messages
    if (! (++counter %= 200))
    {
      // display real reached fps
      std::cout << "Thread FPS: " << p_grabber->getMeasuredFramerate() << std::endl;
    }
    cedar::aux::sleep(cedar::unit::Time(1.0 * cedar::unit::milli * cedar::unit::seconds));
  }

  //----------------------------------------------------------------------------------------
  // save configuration. this step is optional.
  //----------------------------------------------------------------------------------------
  // p_grabber->writeJson(CONFIG_FILE_NAME);

  //----------------------------------------------------------------------------------------
  // clean up
  //----------------------------------------------------------------------------------------

  std::cout << "finished\n";

  // p_grabber is deleted in the shared-pointer class
  return 0;
}
