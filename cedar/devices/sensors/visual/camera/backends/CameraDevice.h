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

    File:        CameraDevice.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Header for the cedar::dev::sensors::visual::CameraDevice class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_CAMERA_DEVICE_H
#define CEDAR_DEV_SENSORS_VISUAL_CAMERA_DEVICE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/namespace.h"
#include "cedar/auxiliaries/SetParameter.h"
#include "cedar/devices/sensors/visual/camera/CameraProperties.h"
#include "cedar/devices/sensors/visual/camera/CameraSettings.h"
#include "cedar/devices/sensors/visual/camera/CameraChannel.h"  //circular includes !!



// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <QReadWriteLock>


/*!@brief Base class of the misc camera grabber backends.
 *
 * Implements the common features of a camera device
 */
class cedar::dev::sensors::visual::CameraDevice
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------


  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief The standard constructor.
  CameraDevice
  (
//    cedar::dev::sensors::visual::CameraSettingsPtr pSettings,
//    cedar::dev::sensors::visual::CameraPropertiesPtr pProperties,
//    cv::VideoCapture videoCapture,
//    QReadWriteLock* p_videoCaptureLock
//   cedar::dev::sensors::visual::CameraGrabber::CameraChannelPtr pCameraChannel
   cedar::dev::sensors::visual::CameraChannelPtr pCameraChannel
  );

public:
  //!@brief Destructor
  virtual ~CameraDevice();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  bool init();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //cv::VideoCapture mVideoCapture;
  //QReadWriteLock* mpVideoCaptureLock;

  //cedar::dev::sensors::visual::CameraSettingsPtr mpCamSettings;
  //cedar::dev::sensors::visual::CameraPropertiesPtr mpCamProperties;

//  cedar::dev::sensors::visual::CameraGrabber::CameraChannelPtr mpCameraChannel;
  cedar::dev::sensors::visual::CameraChannelPtr mpCameraChannel;

  virtual void fillCapabilities() = 0;
  virtual bool createCaptureDevice() = 0;
  virtual void applySettingsToCamera() = 0;
  virtual void applyStateToCamera() = 0;



  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

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

}; // class cedar::dev::sensors::visual::CameraDevice

/*
// The typedefs for the manager of the CameraDevices
#include "cedar/auxiliaries/FactoryManager.h"

namespace cedar
{
  namespace dev
  {
    namespace sensors
    {
      namespace visual
      {
        //!@brief The manager of all sigmoind instances
        typedef cedar::aux::FactoryManager<CameraDevicePtr> CameraDeviceManager;

#ifdef MSVC
#ifdef CEDAR_LIB_EXPORTS_DEV
        // dllexport
        template class __declspec(dllexport) cedar::aux::Singleton<CameraDeviceManager>;
#else // CEDAR_LIB_EXPORTS_DEV
      // dllimport
        extern template class __declspec(dllimport) cedar::aux::Singleton<CameraDeviceManager>;
#endif // CEDAR_LIB_EXPORTS_DEV
#endif // MSVC

        //!@brief The singleton object of the TransferFunctionFactory.
        typedef cedar::aux::Singleton<CameraDeviceManager> CameraDeviceManagerSingleton;
      }
    }
  }
}
*/
#endif // CEDAR_DEV_SENSORS_VISUAL_CAMERA_DEVICE_H

