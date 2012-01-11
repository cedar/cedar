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

    File:        <filename>

    Maintainer:  <first name> <last name>
    Email:       <email address>
    Date:        <creation date YYYY MM DD>

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_CAMERA_CONFIGURATION_H
#define CEDAR_DEV_SENSORS_VISUAL_CAMERA_CONFIGURATION_H

// LOCAL INCLUDES
#include "namespace.h"
#include "CameraGrabber.h"
//#include "CameraCapabilities.h"
//#include "CameraConfigFileStorage.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

/*typedef struct CameraPropertiesStruct
{
  CameraProperty::Id propId;
  double value;
} CameraPropertyValue;
*/




/*!@brief This class manages the properties and capabilities of a camera.
 *
 * \remarks
 * main use: manage capabilities and store the property values and the settings with the configuration interface.
 * this is done by an extra class, because the configurationInterface is notcopyable. So it is not possible to
 * implement the CameraCapabilities class in the cameragrabber and store them in a std::vector.
 */
class cedar::dev::sensors::visual::CameraConfiguration
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! @brief The standard constructor.
  // \param VideoCaptures Reference to the Vector of all videocaptures.
  //        These will be used, to read and set the values
  // \param configurationFileName The filename of the configuration file of used camera
  // \param guid The guid of the camera. This will be used for the capabilities filename
  /*CameraConfiguration(
                       cv::VideoCapture& videoCapture,
                       const std::string& configurationFileName,
                       unsigned long guid
                     );
  */
  //! @brief The standard constructor.
  // \param VideoCaptures Reference to the Vector of all videocaptures.
  //        These will be used, to read and set the values
  // \param configurationFileName The filename of the configuration file of used camera
  // \param capabilitiesFileName The filename of the capabilities
  CameraConfiguration(
                       cv::VideoCapture videoCapture,
                       const std::string channelPrefix,
                       const std::string configurationFileName,
                       const std::string capabilitiesFileName
                     );


  //!@brief Destructor
  ~CameraConfiguration();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
 /* double getCameraProperty(
                            unsigned int channel,
                            CameraProperty::Id propId
                          );
                          
  bool setCameraProperty(
                          unsigned int channel,
                          CameraProperty::Id propId,
                          double value
                        );
  */
  
   //returns the capabilities of a given property
  int getMinValue(CameraProperty::Id propId);
  int getMaxValue(CameraProperty::Id propId);
  bool isSupported(CameraProperty::Id propId);
  bool isReadable(CameraProperty::Id propId);
  bool isOnePushCapable(CameraProperty::Id propId);
  bool isOnOffCapable(CameraProperty::Id propId);
  bool isAutoCapable(CameraProperty::Id propId);
  bool isManualCapable(CameraProperty::Id propId);
  bool isAbsoluteCapable(CameraProperty::Id propId);
  
  bool writeConfiguration();
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
public:
  // none yet (hopefully never!)
protected:

  bool doInit();



private:

  //manage the capabilities of the camera
  CameraCapabilitiesPtr mpCamCapabilities;

  //manage the properties and settings of the camera
  CameraConfigFileStoragePtr mpCamConfigFileStorage;

  std::string mConfigurationFileName;
  std::string mCapabilitiesFileName;
  std::string mChannelPrefix;
  cv::VideoCapture& mVideoCapture;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet



private:


}; // class cedar::dev::sensors::visual::CameraConfiguration

#endif // CEDAR_DEV_SENSORS_VISUAL_CAMERA_CONFIGURATION_H

