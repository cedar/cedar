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

    File:        GrabbableChannel.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 09 28

    Description: Class GrabbableChannel contains additional data of a grabbable-grabbing channel

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_GRABBABLE_CHANNEL_H
#define CEDAR_DEV_SENSORS_VISUAL_GRABBABLE_CHANNEL_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/namespace.h"
#include "cedar/devices/sensors/visual/GrabberChannel.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/Grabbable.h"

// SYSTEM INCLUDES


//!@brief GrabbableChannel contains additional data of a grabbing channel for the Grabbable-Interface
class cedar::dev::sensors::visual::GrabbableChannel
:
public cedar::dev::sensors::visual::GrabberChannel
{
  //!@brief friend class of GrabbableGrabber for direct access to the members
  friend class cedar::dev::sensors::visual::GrabbableGrabber;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  GrabbableChannel(cedar::aux::Grabbable* grabbable = NULL)
  :
  cedar::dev::sensors::visual::GrabberChannel(),
  mpSourceInterfaceClass(grabbable),
  mpGrabberLock(NULL)
  {
  }

  //!@brief Destructor
  virtual ~GrabbableChannel()
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
  //! The object to grab from (have to be a child of the Grabbable-Interface)
  cedar::aux::Grabbable* mpSourceInterfaceClass;

  //! The lock for access to the grabbed image
  QReadWriteLock* mpGrabberLock;

private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::sensors::visual::GrabbableChannel

#endif // CEDAR_DEV_SENSORS_VISUAL_GRABBABLE_CHANNEL_H
