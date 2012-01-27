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

    File:        CameraState.cpp

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 12 01

    Description: Header of the @em cedar::devices::visual::CameraState class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_CAMERA_STATE_H
#define CEDAR_DEV_SENSORS_VISUAL_CAMERA_STATE_H

// LOCAL INCLUDES
#include "cedar/devices/sensors/visual/CameraGrabber.h"
#include "cedar/auxiliaries/ConfigurationInterface.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <set>

//--------------------------------------------------------------------------------------------------------------------
//set to store all supported properties in
typedef std::set<cedar::dev::sensors::visual::CameraProperty::Id> SupportedPropertiesSet;

//--------------------------------------------------------------------------------------------------------------------
//maps property enum id to value of the property
typedef std::map<unsigned int,int> CameraPropertyValues;
typedef std::pair<unsigned int, int> CameraPropertyValuesPair;

//--------------------------------------------------------------------------------------------------------------------
//macro to create a pair of property-enum to property value
#define PROPERTY_VALUE_PAIR(A,B) std::pair<unsigned int,int>((A),(B))


//--------------------------------------------------------------------------------------------------------------------
//store the values of the settings of a camera for the configuration interface
struct CameraSettings
{
  std::string fps;
  std::string mode;
  std::string iso_speed;
  std::string capability_config_file_name;
};
//--------------------------------------------------------------------------------------------------------------------


/*!@brief Stores the properties and the settings of one camera in a configuration file
 *
 * \remarks This class is used by the CameraConfiguration class to store the actual settings of
 *          this camera
 */
class cedar::dev::sensors::visual::CameraState
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
  CameraState(
                           cv::VideoCapture videoCapture,
                           QReadWriteLockPtr videoCaptureLock,
                           const std::string& channelPrefix,
                           SupportedPropertiesSet supportedProperties,
                           const std::string& configFileName
                         );
  //!@brief Destructor
  ~CameraState();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  ///! Save the configuration to the configuration file specified in the constructor
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

  // set a parameter to the cv::VideoCapture
  // implements the cv::VideoCapture.set() method with respect to concurrent access
  bool setProperty(unsigned int prop_id, double value);

  // get a parameter form the cv::VideoCapture
  // implements the cv::VideoCapture.get() method with respect to concurrent access
  double getProperty(unsigned int prop_id);

  /// @endcond

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:

  /*! \brief This is the struct, for the values of the camera settings
   *  \remarks This is the local storage for the camera settings
   *    and is only needed for ConfigurationInterface class
   */
  CameraSettings mCamSettings;

  /*! \brief This is the map, where all supported properties and their actual values stored in
   *  \remarks This is the local storage for the supported camera properties
   *    and is only needed for ConfigurationInterface class
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
  /*! \brief The channel prefix used in the configuration file.
   *  \remarks This is essential if the configuration of more than one camera should be
   *    saved in the configuration file
   */
  std::string mChannelPrefix;

  ///! The already created cv::VideoCapture object from the CameraGrabber class
  cv::VideoCapture mVideoCapture;

  ///! The lock for concurrent access to the VideoCapture
  QReadWriteLockPtr mpVideoCaptureLock;

  ///! This is the set, where only the supported properties stored in
  SupportedPropertiesSet mSupportedProperties;

}; // cedar::dev::sensors::visual::CameraState

#endif // CEDAR_DEV_SENSORS_VISUAL_CAMERA_STATE_H

