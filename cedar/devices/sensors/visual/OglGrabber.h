/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        OglGrabber.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Header for the @em @em cedar::dev::sensors::visual::OglGrabber class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_OGL_GRABBER_H
#define CEDAR_DEV_SENSORS_VISUAL_OGL_GRABBER_H

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/GrabberInterface.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <QGLWidget>

/*! @brief A simple Grabber class for testing the Grabber interface
 *
 *  This grabber class is used to test the grabber interface. It
 *  creates a Grabber with a TestParam (default-value 123) and FPS set to 15
 *
 *  @remarks For grabber developers<br>
 *    This class can also be used as a template to create other classes derived from GrabberInstance
 *
 */
class cedar::dev::sensors::visual::OglGrabber
:
public cedar::dev::sensors::visual::GrabberInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //!@cond SKIPPED_DOCUMENTATION

  /*! @struct PictureChannel
   *  @brief Additional data of a grabbing channel
   *  @remarks For grabber developers<br>
   *    You don't have to create an extended channel structure, until you need more channel data.
   *    But when, then you have to implement the onAddChannel() member function as well
   */
  struct OglChannel
  :
  cedar::dev::sensors::visual::GrabberInterface::GrabberChannel
  {
    //! @brief The QT OpenGL widget
    QGLWidget* mpOglWidget ;
  };

  typedef boost::shared_ptr<OglChannel> OglChannelPtr;
  typedef boost::shared_ptr<const OglChannel> ConstOglChannelPtr;

  //!@endcond

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! @brief The constructor for a single channel grabber.
   *  @param configFileName Filename for a file, where the configuration parameters should be stored
   *  @param channelName  Channel to grab from
   */
  OglGrabber(std::string configFileName, QGLWidget *oglWidget);

  /*! @brief The constructor for a stereo grabber.
   *  @param configFileName Filename for a file, where the configuration parameters should be stored
   *  @param channelName0  Channel one to grab from
   *  @param channelName1  Channel two to grab from
   */
  OglGrabber(std::string configFileName, QGLWidget *oglWidget0, QGLWidget *oglWidget1);

  //!@brief Destructor
  ~OglGrabber();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! @brief Read the counter and reset it to zero
   *  @remarks The counter counts the framerate (i.e. the call of the grab() method) of LoopedThread.
   */
  unsigned int getCounter();

  /*! @brief Simple get-function for the test parameter
   *
   */
  int getTestParam();

  /*! @brief Simple set-function for the test parameter
   *
   */
  void setTestParam(int mTest);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //derived from GrabberInterface
  bool onInit();
  void onCleanUp();
  bool onDeclareParameters();
  const std::string& onGetSourceInfo(unsigned int channel) const;
  bool onGrab();
  void onAddChannel();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  ///! Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class TestChannelPtr
  inline OglChannelPtr getChannel(unsigned int channel)
  {
    return boost::static_pointer_cast<OglChannel>
           (
             cedar::dev::sensors::visual::GrabberInterface::mChannels.at(channel)
           );
  }

  ///! Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class TestChannelPtr
  inline ConstOglChannelPtr getChannel(unsigned int channel) const
  {
    return boost::static_pointer_cast<const OglChannel>
       (
         cedar::dev::sensors::visual::GrabberInterface::mChannels.at(channel)
       );
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:

  //!@brief The counter
  unsigned int mCounter;

  //!@brief The test parameter
  int _mTest;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::sensors::visual::OglGrabber

#endif //CEDAR_DEV_SENSORS_VISUAL_OGL_GRABBER_H