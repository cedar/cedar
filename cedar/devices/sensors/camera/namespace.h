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
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 09 28

    Description: Namespace for @em cedar::devices::sensors::camera

    Credits:

======================================================================================================================*/


#ifndef CEDAR_DEV_SENSORS_CAMERA_NAMESPACE_H
#define CEDAR_DEV_SENSORS_CAMERA_NAMESPACE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/lib.h"
#include "cedar/auxiliaries/SetParameter.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/smart_ptr.hpp>
#endif
#include <map>

namespace cedar
{
  namespace dev
  {
    namespace sensors
    {
      //! @brief Namespace for camera devices
      namespace camera
      {
        //!@cond SKIPPED_DOCUMENTATION
        CEDAR_DECLARE_DEV_CLASS(Grabber);
        CEDAR_DECLARE_DEV_CLASS(Channel);

        // backend implementation
        CEDAR_DECLARE_DEV_CLASS(Backend);
        CEDAR_DECLARE_DEV_CLASS(BackendCvVideoCapture);

  #ifdef CEDAR_USE_VIDEO_FOR_LINUX
        CEDAR_DECLARE_DEV_CLASS(BackendV4L);
  #endif // CEDAR_USE_VIDEO_FOR_LINUX

  #ifdef CEDAR_USE_LIB_DC1394
        CEDAR_DECLARE_DEV_CLASS(BackendDc1394);
        CEDAR_DECLARE_DEV_CLASS(LibDcBase);
  #endif

        // enum classes for camera settings and properties
        CEDAR_DECLARE_DEV_CLASS(BackendType);
        CEDAR_DECLARE_DEV_CLASS(Decoding);
        CEDAR_DECLARE_DEV_CLASS(FrameRate);
        CEDAR_DECLARE_DEV_CLASS(Property);
        CEDAR_DECLARE_DEV_CLASS(VideoMode);
        CEDAR_DECLARE_DEV_CLASS(Setting);
        CEDAR_DECLARE_DEV_CLASS(BackendType);
        CEDAR_DECLARE_DEV_CLASS(IsoSpeed);
        CEDAR_DECLARE_DEV_CLASS(DebayerPattern);

        // all properties from one channel
        CEDAR_DECLARE_DEV_CLASS(Properties);

        // one property
        CEDAR_DECLARE_DEV_CLASS(CamProperty);
        CEDAR_DECLARE_DEV_CLASS(PropertyMode);

        // the settings for one channel
        CEDAR_DECLARE_DEV_CLASS(Settings);
        CEDAR_DECLARE_DEV_CLASS(Capabilities);
        CEDAR_DECLARE_DEV_CLASS(State);

        // camera settings (possible settings)
        CEDAR_DECLARE_DEV_CLASS(CamSetting);

        // map from property-id to the corresponding object, which manage the property
        typedef std::map<unsigned int, cedar::dev::sensors::camera::CamPropertyPtr> PropertyMap;

        //-----------------------------------------------------------------------------------------------
        // exceptions
        //-----------------------------------------------------------------------------------------------

        class PropertyNotSupportedException;
        class PropertyNotSetException;
        class VideoCaptureNotOpenedException;
        class CreateBackendException;

#ifdef CEDAR_USE_LIB_DC1394
        class LibDcInitException;
        class LibDcException;
        class LibDcCameraNotFoundException;
#endif //CEDAR_USE_LIB_DC1394

        //!@endcond
      }
    }
  }
}

#endif /* CEDAR_DEV_SENSORS_CAMERA_NAMESPACE_H */
