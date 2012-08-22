/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>
//#include <map>



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

        //-----------------------------------------------------------------------------------------------
        // grabber classes
        //-----------------------------------------------------------------------------------------------

        //common base class for all grabbers
        CEDAR_DECLARE_DEV_CLASS(Grabber);
        
        //grabber
        CEDAR_DECLARE_DEV_CLASS(VideoGrabber);
        CEDAR_DECLARE_DEV_CLASS(PictureGrabber);
        CEDAR_DECLARE_DEV_CLASS(TestGrabber);
        CEDAR_DECLARE_DEV_CLASS(GLGrabber);
        CEDAR_DECLARE_DEV_CLASS(GrabbableGrabber);
        CEDAR_DECLARE_DEV_CLASS(CameraGrabber);

#ifdef CEDAR_USE_YARP
        CEDAR_DECLARE_DEV_CLASS(NetGrabber);
#endif // CEDAR_USE_YARP

        //-----------------------------------------------------------------------------------------------
        // camera grabber helper classes
        //-----------------------------------------------------------------------------------------------

        // backend implementation
        CEDAR_DECLARE_DEV_CLASS(CameraDevice);
        CEDAR_DECLARE_DEV_CLASS(CameraDeviceVfl);
        CEDAR_DECLARE_DEV_CLASS(CameraDeviceCvVideoCapture);

#ifdef CEDAR_USE_LIB_DC1394
        CEDAR_DECLARE_DEV_CLASS(CameraDeviceDc1394);
#endif
        // enum classes
        CEDAR_DECLARE_DEV_CLASS(RecordingFormat);

        // enum classes for camera
        CEDAR_DECLARE_DEV_CLASS(CameraBackendType);
        CEDAR_DECLARE_DEV_CLASS(DeBayerFilter);
        CEDAR_DECLARE_DEV_CLASS(CameraFrameRate);
        CEDAR_DECLARE_DEV_CLASS(CameraProperty);
        CEDAR_DECLARE_DEV_CLASS(CameraVideoMode);
        CEDAR_DECLARE_DEV_CLASS(CameraSetting);
        CEDAR_DECLARE_DEV_CLASS(CameraBackendType);
        CEDAR_DECLARE_DEV_CLASS(CameraIsoSpeed);
        CEDAR_DECLARE_DEV_CLASS(CameraDebayerPattern);

        // properties

        // all properties from one channel
        CEDAR_DECLARE_DEV_CLASS(CameraProperties);

        // one property with name, enum-parameter for mode and double-parameter for value
        CEDAR_DECLARE_DEV_CLASS(CamProperty);
        CEDAR_DECLARE_DEV_CLASS(CameraPropertyMode);

        // settings

        // all settings from one channel
        CEDAR_DECLARE_DEV_CLASS(CameraSettings);
        CEDAR_DECLARE_DEV_CLASS(CameraCapabilities);
        CEDAR_DECLARE_DEV_CLASS(CameraState);


        // camera settings
        CEDAR_DECLARE_DEV_CLASS(CamSetting);

        struct CameraChannel;
        CEDAR_GENERATE_POINTER_TYPES(CameraChannel);

#ifdef CEDAR_USE_LIB_DC1394
        //firewire related base class for the grabbertools
        CEDAR_DECLARE_CLASS(LibDcCameraBase);
#endif // CEDAR_USE_LIB_DC1394

        //-----------------------------------------------------------------------------------------------
        // grabber types
        //-----------------------------------------------------------------------------------------------

        ///! map property enum id to the value of the property
        //typedef std::map<unsigned int, double> CameraPropertyValues;

        //! map of a property-id to his class
        typedef std::map<unsigned int, cedar::dev::sensors::visual::CamProperty> CameraPropertyMap;

        ///! a pair of property enum id and his value
        // typedef std::pair<unsigned int, double> CameraPropertyValuesPair;

        typedef cedar::aux::SetParameter<cedar::dev::sensors::visual::CamPropertyPtr> CameraPropertiesSet;
        CEDAR_GENERATE_POINTER_TYPES(CameraPropertiesSet);

        typedef cedar::aux::SetParameter<cedar::dev::sensors::visual::CamSettingPtr> CameraSettingsSet;
        CEDAR_GENERATE_POINTER_TYPES(CameraSettingsSet);
        //!@endcond

        //-----------------------------------------------------------------------------------------------
        // exceptions
        //-----------------------------------------------------------------------------------------------

        //!@brief An exception for errors on recording
        class GrabberRecordingException;

        //!@brief An exception for errors on saving a snapshot
        class GrabberSnapshotException;

        //!@brief An exception for errors on grabbing
        class GrabberGrabException;

      }
    }
  }
}



#endif /* CEDAR_DEV_SENSORS_VISUAL_NAMESPACE_H */
