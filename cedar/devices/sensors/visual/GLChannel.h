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

    File:        GLChannel

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 09 28

    Description: Class GLChannel contains additional data of a QGLWidget-grabbing channel

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_GLGRABBER_CHANNEL_H
#define CEDAR_DEV_SENSORS_VISUAL_GLGRABBER_CHANNEL_H

#ifdef CEDAR_USE_QGLVIEWER

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/namespace.h"
#include "cedar/devices/sensors/visual/GrabberChannel.h"

// SYSTEM INCLUDES
#include <QGLWidget>

//!@brief Class GLChannel contains additional data of an QGLWidget-grabbing channel
class cedar::dev::sensors::visual::GLChannel
:
public cedar::dev::sensors::visual::GrabberChannel
{
  //!@brief friend class of GLGrabber for direct access to the members
  friend class cedar::dev::sensors::visual::GLGrabber;
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  GLChannel(QGLWidget* qglWidget=NULL)
  :
  cedar::dev::sensors::visual::GrabberChannel(),
  mpQGLWidget(qglWidget)
  {
  }
  //!@brief Destructor
  virtual ~GLChannel()
  {
  };

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! @brief The QT OpenGL widget
  QGLWidget* mpQGLWidget ;

private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::sensors::visual::GLChannel

#endif // CEDAR_USE_QGLVIEWER
#endif // CEDAR_DEV_SENSORS_VISUAL_GLGRABBER_CHANNEL_H

