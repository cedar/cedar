/*========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        InterfaceGrabberTest.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 04 26

    Description: Simple application to grab from a class which implements the grabbable interface (mono-case)

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/GrabbableGrabber.h"
#include "cedar/auxiliaries/Grabbable.h"
#include "cedar/auxiliaries/gui/ImagePlot.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gui/SceneWidget.h"
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/gl/Block.h"

// SYSTEM INCLUDES
#include <QReadWriteLock>
#include <QtGui/QApplication>
#include <opencv2/opencv.hpp>
#include <boost/lexical_cast.hpp>

int main(int argc, char **argv)
{
  // -------------------------------------------------------------------------------------------------------------------
  // Constants
  // -------------------------------------------------------------------------------------------------------------------

  // the name of the grabber
  // only used in the configuration file
  const std::string GRABBER_NAME = "Interface_Grabber_Test";

  // the name for the configuration file (not needed for this test)
  const std::string CONFIG_FILE_NAME = "interface_grabber_test.config";

  // title of grabber window
  std::string window_title = "Grabber: " + GRABBER_NAME;

  std::cout << "\n\nInteractive test of the InterfaceGrabber class\n";
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
  p_block_local_coordinate_frame->setTranslation(3, -3, 3);
  cedar::aux::gl::ObjectVisualizationPtr p_block
  (
    new cedar::aux::gl::Block(p_block_local_coordinate_frame, 1, 2, 3, 0, 1, 0.5)
  );
  p_scene->addObjectVisualization(p_block);
  p_block_local_coordinate_frame->rotate(0, M_PI/2);

  // wait until viewer is finished with its creation
  while (QApplication::hasPendingEvents())
  {
    QApplication::processEvents();
  }
  usleep(10000);

  // -------------------------------------------------------------------------------------------------------------------
  // Create grabber, with viewer-class as parameter
  // -------------------------------------------------------------------------------------------------------------------
  std::cout << "Create a CameraGrabber:\n";
  cedar::dev::sensors::visual::GrabbableGrabberPtr p_grabber;
  try
  {
    p_grabber = cedar::dev::sensors::visual::GrabbableGrabberPtr
                (
                  new cedar::dev::sensors::visual::GrabbableGrabber
                      (
                       static_cast<cedar::aux::Grabbable * >(&viewer),
                       GRABBER_NAME // not necessary
                      )
                );
  }
  catch (cedar::aux::InitializationException &e)
  {
    // after an InitializationExeception the grabber class isn't initialized correctly
    // and couldn't be used
    std::cout << "Error on creation of the GrabbableGrabber class:\n"
              << e.exceptionInfo() << std::endl;

    // delete p_grabber is done via the shared-pointer class
    return -1;
  }

  // connected with viewer, process pending events
  while (QApplication::hasPendingEvents())
  {
    QApplication::processEvents();
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
    // an error occured during initialization. Perhaps the file doesn't exist
    return -1;
  }

  // allow to register the grabber in the viewer class and wait.
  // at least one redraw is needed to write the GL-Image in the grab-buffer from interface "Grabbable"
  for (int i = 0; i < 100; ++i)
  {
    while (QApplication::hasPendingEvents())
    {
      QApplication::processEvents();
    }
    usleep (10000);
  }


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
  p_plot->show();
  p_plot->resize(frame.cols,frame.rows);

  // allow the ImagePlot to create
  while (QApplication::hasPendingEvents())
  {
    QApplication::processEvents();
  }

  // start the grabber-thread for reading the GL images in the background
  std::cout << "Start grabbing in the background" << std::endl;
  p_grabber->setFps(50);
  p_grabber->startGrabber();

  // start recording (if you like)
  // std::cout << "\nStart Recording\n";
  // p_grabber->startRecording(50);

  unsigned int counter=0;
  std::cout << "\nDisplay GL frames\n";

  // get frames for a while
  while (!frame.empty() && p_plot->isVisible())
  {
    while (QApplication::hasPendingEvents())
    {
      QApplication::processEvents();
    }

    // read new images, from the grabber-buffer. This is independent from background-thread
    // the background thread gets the images from the gl-widget periodically
    // with the getImage() method, you can get the image from the internal buffer
    // if background-thread and GL-processing is faster than your processing, images will be skipped
    p_lock->lockForRead();
    p_data->lockForWrite();
    p_data->setData(p_grabber->getImage());
    p_data->unlock();
    p_lock->unlock();

    // every second
    if (! (++counter %= 1000))
    {
      // display real reached fps
      std::cout << "Thread FPS: " << p_grabber->getFpsMeasured() << std::endl;
    }
    usleep(1000);
  }

  //----------------------------------------------------------------------------------------
  // save configuration. this step is optional.
  //----------------------------------------------------------------------------------------
  // p_grabber->writeJson(CONFIG_FILE_NAME);

  //----------------------------------------------------------------------------------------
  // clean up
  //----------------------------------------------------------------------------------------

  // stopGrabber grabbing-thread if running
  // recording will also be stopped
  if (p_grabber->isRunning())
  {
    p_grabber->stopGrabber();
  }
  std::cout << "finished\n";

  // p_grabber is deleted in the shared-pointer class
  return 0;
}
