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

#ifndef CEDAR_DEV_SENSORS_VISUAL_OGL_GRABBER_H
#define CEDAR_DEV_SENSORS_VISUAL_OGL_GRABBER_H

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/GrabberInterface.h"
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
public cedar::dev::sensors::visual::GrabberInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //!@cond SKIPPED_DOCUMENTATION

  /*! @struct OglChannel
   *  @brief Additional data of a grabbing channel to grab from a QGLWidget
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
  GLGrabber(std::string configFileName, QGLWidget *oglWidget);

  /*! @brief The constructor for a stereo grabber.
   *  @param configFileName Filename for a file, where the configuration parameters should be stored
   *  @param channelName0  Channel one to grab from
   *  @param channelName1  Channel two to grab from
   */
  GLGrabber(std::string configFileName, QGLWidget *oglWidget0, QGLWidget *oglWidget1);

  //!@brief Destructor
  ~GLGrabber();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:


  /*! @brief Set a new Widget to grab from
   *
   */
  void setWidget(unsigned int channel, QGLWidget *oglWidget);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //derived from GrabberInterface
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
  // none yet


  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::sensors::visual::GLGrabber

#endif //CEDAR_DEV_SENSORS_VISUAL_OGL_GRABBER_H
