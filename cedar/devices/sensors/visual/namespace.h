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

    File:        namespace.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@rub.de
    Date:        2011 08 01

    Description: Namespace for @em cedar::devices::sensors::visual.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_SENSORS_VISUAL_NAMESPACE_H
#define CEDAR_DEV_SENSORS_VISUAL_NAMESPACE_H

// CEDAR INCLUDES
#include "cedar/devices/lib.h"
#include "cedar/configuration.h"   // MAKE FIREWIRE and YARP OPTIONAL

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>
#include <map>



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

        //-----------------------------------------------------------------------------------------------
        // grabber classes
        //-----------------------------------------------------------------------------------------------

        //!@cond SKIPPED_DOCUMENTATION

        //common interface class
        CEDAR_DECLARE_DEV_CLASS(Grabber);
        
        //grabber
        CEDAR_DECLARE_DEV_CLASS(VideoGrabber);
        CEDAR_DECLARE_DEV_CLASS(PictureGrabber);
        CEDAR_DECLARE_DEV_CLASS(TestGrabber);
        CEDAR_DECLARE_DEV_CLASS(GLGrabber);
        CEDAR_DECLARE_DEV_CLASS(GrabbableInterface);
        CEDAR_DECLARE_DEV_CLASS(InterfaceGrabber);

#ifdef CEDAR_USE_LIB_DC1394
        CEDAR_DECLARE_DEV_CLASS(CameraGrabber);
#endif // CEDAR_USE_LIB_DC1394

#ifdef CEDAR_USE_YARP
        CEDAR_DECLARE_DEV_CLASS(NetGrabber);
#endif // CEDAR_USE_YARP

        //-----------------------------------------------------------------------------------------------
        // helper classes
        //-----------------------------------------------------------------------------------------------
        
        //enum classes for camera
#ifdef CEDAR_USE_LIB_DC1394
        CEDAR_DECLARE_DEV_CLASS(CameraFrameRate);
        CEDAR_DECLARE_DEV_CLASS(CameraProperty);
        CEDAR_DECLARE_DEV_CLASS(CameraVideoMode);
        CEDAR_DECLARE_DEV_CLASS(CameraSetting);
        CEDAR_DECLARE_DEV_CLASS(CameraIsoSpeed);
#endif //CEDAR_USE_LIB_DC1394

#ifdef CEDAR_USE_LIB_DC1394
        //misc helper classes for firewire
        CEDAR_DECLARE_DEV_CLASS(CameraStateAndConfig);
        CEDAR_DECLARE_DEV_CLASS(CameraCapabilities);
        CEDAR_DECLARE_DEV_CLASS(CameraConfig);

        //firewire related base class for the grabbertools
        CEDAR_DECLARE_CLASS(LibDcCameraBase);
#endif // CEDAR_USE_LIB_DC1394

        //-----------------------------------------------------------------------------------------------
        // grabber types
        //-----------------------------------------------------------------------------------------------

        ///! map property enum id to the value of the property
        typedef std::map<unsigned int, double> CameraPropertyValues;

        ///! a pair of property enum id and his value
        typedef std::pair<unsigned int, double> CameraPropertyValuesPair;

        //!@endcond

        //-----------------------------------------------------------------------------------------------
        // exceptions
        //-----------------------------------------------------------------------------------------------

        //! @brief An exception for errors on recording
        class GrabberRecordingException;

        //! @brief An exception for errors on saving a snapshot
        class GrabberSnapshotException;

        //! @brief An exception for errors on grabbing
        class GrabberGrabException;

      }
    }
  }
}



#endif /* CEDAR_DEV_SENSORS_VISUAL_NAMESPACE_H */
