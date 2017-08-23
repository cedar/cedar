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

    File:        namespace.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@rub.de
    Date:        2011 08 01

    Description: Namespace for @em cedar::devices::sensors::visual.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_SENSORS_VISUAL_NAMESPACE_H
#define CEDAR_DEV_SENSORS_VISUAL_NAMESPACE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/lib.h"
#include "cedar/auxiliaries/SetParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/ObjectListParameterTemplate.fwd.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/smart_ptr.hpp>
#endif

//#define DEBUG_GRABBER

//------------------------------------------------------------------------
// namespace for classes
//------------------------------------------------------------------------

namespace cedar
{
  namespace dev
  {
    namespace sensors
    {
      //! \brief Namespace for all visual sensors like cameras or camera-dummies (i.e. avi-file, picture)
      namespace visual
      {
        //!@cond SKIPPED_DOCUMENTATION

        // common base classes for all grabbers
        CEDAR_DECLARE_DEV_CLASS(Grabber);
        CEDAR_DECLARE_DEV_CLASS(GrabberChannel);
        
        // grabber
        CEDAR_DECLARE_DEV_CLASS(VideoGrabber);
        CEDAR_DECLARE_DEV_CLASS(VideoChannel);

        CEDAR_DECLARE_DEV_CLASS(PictureGrabber);
        CEDAR_DECLARE_DEV_CLASS(PictureChannel);

#ifdef CEDAR_USE_QGLVIEWER
        CEDAR_DECLARE_DEV_CLASS(GLGrabber);
        CEDAR_DECLARE_DEV_CLASS(GLChannel);
#endif //CEDAR_USE_QGLVIEWER

        CEDAR_DECLARE_DEV_CLASS(GrabbableChannel);
        CEDAR_DECLARE_DEV_CLASS(GrabbableGrabber);

#ifdef CEDAR_USE_YARP
        CEDAR_DECLARE_DEV_CLASS(NetGrabber);
        CEDAR_DECLARE_DEV_CLASS(NetChannel);
#endif // CEDAR_USE_YARP

        // the channels as ObjectList for the processingGUI
        typedef cedar::aux::ObjectListParameterTemplate<GrabberChannel> ChannelParameter;
        CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(ChannelParameter);

        // enum class
        CEDAR_DECLARE_DEV_CLASS(RecordingFormat);

        //-----------------------------------------------------------------------------------------------
        // exceptions
        //-----------------------------------------------------------------------------------------------
        class GrabberRecordingException;
        class GrabberSnapshotException;
        class GrabberGrabException;
        class InvalidParameterException;
        class CreateGrabberException;
        //!@endcond
      }
    }
  }
}



#endif /* CEDAR_DEV_SENSORS_VISUAL_NAMESPACE_H */
