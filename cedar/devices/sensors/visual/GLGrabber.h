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

    File:        GLGrabber.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description: Header for the @em @em cedar::dev::sensors::visual::GLGrabber class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_GL_GRABBER_H
#define CEDAR_DEV_SENSORS_VISUAL_GL_GRABBER_H

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/Grabber.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <QGLWidget>

/*! @brief A grabber to grab from a QGLWidget
 *
 *  Be aware, that the grabbing have to be done in the gui-thread.
 *  The grabbing will fail, if you start the grabberthread to grab in the background.
 *  You have to grab in your main gui-thread with the grab() memberfunction of the class.
 *  The getImage() member could be also invoked in threads running in the background
 */
class cedar::dev::sensors::visual::GLGrabber
:
public cedar::dev::sensors::visual::Grabber
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //!@cond SKIPPED_DOCUMENTATION

  /*! @struct GLChannel
   *  @brief Additional data of a grabbing channel to grab from a QGLWidget
   */
  struct GLChannel
  :
  cedar::dev::sensors::visual::Grabber::GrabberChannel
  {
    //! @brief The QT OpenGL widget
    QGLWidget* mpQGLWidget ;
  };

  CEDAR_GENERATE_POINTER_TYPES(GLChannel);
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
   *  @param qglWidget A pointer to a QGLWidget to grab from
   */
  GLGrabber(std::string configFileName, QGLWidget *qglWidget);

  /*! @brief The constructor for a stereo grabber.
   *  @param configFileName Filename for a file, where the configuration parameters should be stored
   *  @param qglWidget0 A pointer to a QGLWidget to grab from for channel 0
   *  @param qglWidget1 A pointer to a QGLWidget to grab from for channel 1
   */
  GLGrabber(std::string configFileName, QGLWidget *qglWidget0, QGLWidget *qglWidget1);

  //!@brief Destructor
  ~GLGrabber();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:


  /*! @brief Set a new Widget to grab from
   *
   */
  void setWidget(unsigned int channel, QGLWidget *qglWidget);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //derived from Grabber
  bool onInit();
  bool onDeclareParameters();
  void onCleanUp();
  void onUpdateSourceInfo(unsigned int channel);
  void onAddChannel();
  bool onGrab();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  ///! Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class TestChannelPtr
  inline GLChannelPtr getChannel(unsigned int channel)
  {
    return boost::static_pointer_cast<GLChannel>
           (
             cedar::dev::sensors::visual::Grabber::mChannels.at(channel)
           );
  }

  ///! Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class TestChannelPtr
  inline ConstGLChannelPtr getChannel(unsigned int channel) const
  {
    return boost::static_pointer_cast<const GLChannel>
       (
         cedar::dev::sensors::visual::Grabber::mChannels.at(channel)
       );
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet


  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::sensors::visual::GLGrabber

#endif //CEDAR_DEV_SENSORS_VISUAL_GL_GRABBER_H
