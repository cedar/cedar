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

#ifndef CEDAR_DEV_SENSORS_VISUAL_CAMERA_CONFIG_FILE_STORAGE_H
#define CEDAR_DEV_SENSORS_VISUAL_CAMERA_CONFIG_FILE_STORAGE_H

// LOCAL INCLUDES
#include "CameraGrabber.h"
#include "cedar/auxiliaries/ConfigurationInterface.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <set>

//set to store all supported properties in
typedef std::set<cedar::dev::sensors::visual::CameraProperty::Id> SupportedPropertiesSet;

//maps property enum id to value of the property
typedef std::map<unsigned int,int> CameraPropertyValues;
typedef std::pair<unsigned int, int> CameraPropertyValuesPair;



#define PROPERTY_VALUE_PAIR(A,B) std::pair<unsigned int,int>((A),(B))


typedef struct CameraSettingsStruct
{
  std::string fps;
  std::string mode;
  std::string iso_speed;
  std::string capability_config_file_name;
} CameraSettings;


/*!@brief Stores the properties and the settings of one camera in a configuration file
 *
 * More detailed description of the class.
 */
class cedar::dev::sensors::visual::CameraConfigFileStorage
:
public cedar::aux::ConfigurationInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  CameraConfigFileStorage(
                           cv::VideoCapture& videoCapture,
                           const std::string& channelPrefix,
                           SupportedPropertiesSet supportedProperties,
                           const std::string& configFileName
                         );
  //!@brief Destructor
  ~CameraConfigFileStorage();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // Saves the configuration in config-file
   bool writeConfiguration();

   // declares the supported properties


  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /// @cond SKIPPED_DOCUMENTATION
  /* This Method reads all parameters and settings from the camera and stores them inside the buffers
   *  This is needed for the configurationInterface
   *  The parameters and settings will be stored within the mCamProperties and mCamSettings vectors
   */
  void getAllParametersFromCam();

  /*  This Method sets all parameters and settings read from the configfile to the camera
   *  This is needed for the configurationInterface
   *  The parameters and settings will be synchronized from the mCamProperties and mCamSettings vector
   */
  bool setAllParametersToCam();

  // create local buffer for camera properties and settings
  // needed for the ConfigurationInterface class
  void createParameterStorage();

  // Interface to ConfigurationInterface class;
  bool declareParameter();



  /// @endcond

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  /*! \brief stores all camera settings local (needed for configurationInterface) */
  CameraSettings mCamSettings;

  /*! \brief stores supported camera properties local (needed for configurationInterface)
   */
  CameraPropertyValues mCamPropertyValues;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  //the channel prefix used in the configuration file.
  //Essential if more than one camera saved in config-file
  std::string mChannelPrefix;

  cv::VideoCapture& mVideoCapture;

  SupportedPropertiesSet mSupportedProperties;

}; // cedar::dev::sensors::visual::CameraConfigFileStorage

#endif // CEDAR_DEV_SENSORS_VISUAL_CAMERA_CONFIG_FILE_STORAGE_H

