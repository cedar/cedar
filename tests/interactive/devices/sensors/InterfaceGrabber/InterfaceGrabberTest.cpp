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
#include "cedar/devices/sensors/visual/InterfaceGrabber.h"

#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gui/SceneWidget.h"
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/gl/Block.h"

// SYSTEM INCLUDES
#include <QReadWriteLock>
#include <boost/lexical_cast.hpp>

int main(int argc, char **argv)
{
  // -------------------------------------------------------------------------------------------------------------------
  // Constants
  // -------------------------------------------------------------------------------------------------------------------

  // the name of the grabber
  // only used in the configuration file
  const std::string GRABBER_NAME = "Interface_Grabber_TestCase";

  // the basename for the configuration file
  // overall there are more config.files, this one for the grabber itself (call it the main configuration),
  // one for each channel for the GL-capabilities (it's name is stored in the main config.)
  // one for the actual setting of the GL (it's name is derived from the main config.filename
  // with an appendix (_ch0, _ch1, ...)
  const std::string CONFIG_FILE_NAME = "interface_grabber_testcase.config";


  //title of highgui window
  std::string highgui_window_name = GRABBER_NAME;

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


  while (QApplication::hasPendingEvents())
  {
    QApplication::processEvents();
  }

  // -------------------------------------------------------------------------------------------------------------------
  // Create grabber
  // -------------------------------------------------------------------------------------------------------------------

  std::cout << "Create an InterfaceGrabber:\n";
  cedar::dev::sensors::visual::InterfaceGrabber *if_grabber=NULL;
  try
  {
    if_grabber = new cedar::dev::sensors::visual::InterfaceGrabber
                         (
                           CONFIG_FILE_NAME,
                           static_cast<cedar::dev::sensors::visual::Grabbable * >(&viewer)
                         );
  }
  catch (cedar::aux::InitializationException &e)
  {
    //after an InitializationExeception the video_grabber class isn't initialized correctly
    //and couldn't be used
    std::cout << "Error on creation of the VideoGrabber class:\n"
              << e.exceptionInfo() << std::endl;

    if (if_grabber)
    {
      delete if_grabber;
    }

    return -1;
  }

  //connected with viewer, process pending events
  while (QApplication::hasPendingEvents())
  {
    QApplication::processEvents();
  }
  std::cout << "events done, second time" << std::endl;

  //first lets set a custom name for our grabber. This name will be used in the configuration file
  if_grabber->setName(GRABBER_NAME);
  std::cout << "\nGrab channel 0 from \"" << if_grabber->getSourceInfo()<< "\"" << std::endl;


  //------------------------------------------------------------------
  //Create an OpenCV highgui window to show grabbed frames
  cv::namedWindow(highgui_window_name,CV_WINDOW_KEEPRATIO);

  //allow GLWidget to paint first pictures
  while (QApplication::hasPendingEvents())
  {
    QApplication::processEvents();
  }

  //manually grab first image, because the GL-related stuff in the viewer has to have enough time to create a scene
  if_grabber->grab();
  //allow GLWidget to paint first pictures
  while (QApplication::hasPendingEvents())
  {
    QApplication::processEvents();
  }

  //the first frame is already grabbed and could be read
  //always use the QReadWriteLock for locking the cv::Mat image object
  //on access
  QReadWriteLock* pReadWriteLock = if_grabber->getReadWriteLockPointer();


  pReadWriteLock->lockForRead();
  cv::Mat frame = if_grabber->getImage();
  pReadWriteLock->unlock();


  if  (frame.empty())
  {
    std::cout << "ERROR, no data" << std::endl;
  }
  else
  {
    //startGrabber the grabber-thread for updating GL images
    if_grabber->setFps(50);

    std::cout << "Start grabbing in the background" << std::endl;
    if_grabber->startGrabber();

    //startGrabber recording
    //std::cout << "\nStart Recording\n";
    //if_grabber->startRecording(50);

    unsigned int counter=0;
    std::cout << "\nDisplay GL frames\n";

    //get frames for a while
    while (!frame.empty())
    {
      cv::imshow(highgui_window_name,frame);

      pReadWriteLock->lockForRead();
      frame = if_grabber->getImage();
      pReadWriteLock->unlock();

      counter++;

      //exit after 5 seconds
      if (counter == 500)
      {
        break;
      }

      //every second
      if (! (counter % 100))
      {
        //display real reached fps
        std::cout << "Thread FPS: " << if_grabber->getFpsMeasured() << std::endl;
      }

      while (QApplication::hasPendingEvents())
      {
        QApplication::processEvents();
      }

      //wait 10ms (needed for highgui) => get images from thread with 100 fps
      cv::waitKey(10);
    }
  }

  //------------------------------------------------------------------
  //clean up

  //stopGrabber grabbing-thread if running
  //recording will also be stopped
  if (if_grabber->isRunning())
  {
    if_grabber->stopGrabber();
  }

  cv::destroyWindow(highgui_window_name);
  //ogl_grabber->writeConfiguration();

  if (if_grabber)
  {
    delete if_grabber;
  }

  std::cout << "finished\n";

  return 0;
}
