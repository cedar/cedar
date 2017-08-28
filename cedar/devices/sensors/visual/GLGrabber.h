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
    along with cedar. If not, see <http:// www.gnu.org/licenses/>.

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


// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_QGLVIEWER

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/Grabber.h"
#include "cedar/devices/sensors/visual/GLChannel.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <QGLWidget>
#include <string>

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


  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! @brief  Constructor for a single channel grabber
   *  @param grabberName  Name of the grabber
   *  @param qglWidget Class derived from QGLWidget to grab from
   */
  GLGrabber
  (
    QGLWidget *qglWidget,
    const std::string& grabberName = "GLGrabber"
  );

  /*! @brief Constructor for a stereo channel grabber
   *  @param grabberName  Name of the grabber
   *  @param qglWidget0 Class derived from QGLWidget to grab from for channel 0
   *  @param qglWidget1 Class derived from QGLWidget to grab from for channel 1
   */
  GLGrabber
  (
    QGLWidget *qglWidget0,
    QGLWidget *qglWidget1,
    const std::string& grabberName = "StereoGLGrabber"
  );

  //!@brief Destructor
  ~GLGrabber();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! @brief Set a new Widget to grab from
   *
   * @param channel The channel to assign the new widget
   * @param qglWidget The new widget
   * @throw cedar::aux::IndexOutOfRangeException When the channel is out of range
   * @throw cedar::dev::sensors::visual::InvalidParameterException When the qglWidget is a NULL-pointer
   */
  void setWidget(unsigned int channel, QGLWidget *qglWidget);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  // inherited from Grabber
  void onCleanUp();
  void onGrab(unsigned int channel);
  void onCreateGrabber();
  void onCloseGrabber();
  std::string onGetSourceInfo(unsigned int channel);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  ///! Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class TestChannelPtr
  inline GLChannelPtr getGLChannel(unsigned int channel)
  {
    return boost::static_pointer_cast<GLChannel>
           (
             cedar::dev::sensors::visual::Grabber::_mChannels->at(channel)
           );
  }

  ///! Cast the storage vector from base channel struct "GrabberChannelPtr" to derived class TestChannelPtr
  inline ConstGLChannelPtr getGLChannel(unsigned int channel) const
  {
    return boost::static_pointer_cast<const GLChannel>
       (
         cedar::dev::sensors::visual::Grabber::_mChannels->at(channel)
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

#endif //CEDAR_USE_QGLVIEWER
#endif // CEDAR_DEV_SENSORS_VISUAL_GL_GRABBER_H
