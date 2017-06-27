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

    File:        LibDcBase.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 12 11

    Description: Header for the @em LibDcBase class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_CAMERA_LIBDCCAMERABASE_H
#define CEDAR_DEV_SENSORS_CAMERA_LIBDCCAMERABASE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_LIB_DC1394

// CEDAR INCLUDES
#include "cedar/devices/sensors/camera/namespace.h"

// SYSTEM INCLUDES
#include <inttypes.h>
#include <dc1394/dc1394.h>
#include <iostream>
#include <vector>
#include <string>

/*!@brief This class is a simple interface to the firewire bus.
 *
 *    It is only used for the CameraGrabber
 *
 * @remarks: Opens the bus and handles the firewire device context handle.<br>
 *  USAGE:<br>
 *    1. scan bus<br>
 *    2. select device<br>
 *    3. open it<br>
 *    4. work with opened device<br>
 *    5. close device<br>
 */
class cedar::dev::sensors::camera::LibDcBase
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The standard constructor.
   * @throw cedar::dev::sensors::camera::LibDcInitException Thrown, on initialization errors in libdc
   */
  // opens the firewire bus context and query the number of available cameras
  LibDcBase();
  
  //!@brief Destructor
  // close all firewire related handles
  ~LibDcBase();
  
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Release all Handles and memory
  void cleanUp();
  
  //! After creation of the class, this method gives the camera count
  unsigned int getNumCams();
  
  //! Gives the GUID of given camera. Returns 0 if unsupported camNr
  uint64_t getCamGuid(unsigned int camNr);

  //! Print out all features of the currently opened camera
  void printAllFeatures();
  
 
  // camera methods:

  //! Open a given camera. Methods like getCamFeatures rely on an opend camera.
  //   This is the first command if you want to get informations on a specific device
  bool openCamera(uint64_t camGuid);
  
  //! Close actual opened camera. Call this method at the end of your work
  //   or just before you like to open another camera
  void closeCamera();
  

  /*! Get the names of all features of a camera
   * @return An empty vector, if no camera was opened before
   *
   */
  std::string getAllFeaturesAsString();
  

  /*! Returns a libdc1394 featureset with all features and all informations about that features
   *
   * @return A libdc1394 struct of all supported video modes.
   *  If there wasn't a camera opened,
   *  then all fields of the struct are set to zero
   */
  dc1394featureset_t getCamFeatures();
  

  /*! Returns all supported video modes
   *
   * @return A libdc1394 struct of all supported video modes.
   *  If there wasn't a camera opened,
   *  then all fields of the struct are set to zero
   *
  */
  dc1394video_modes_t getCamVideoModes();
  
  /*! Returns a libdc1394 struct of all supported framerates of the given grabbing mode
   *
   * @param mode The wanted grabbing mode as a constant of the dc1394video_mode_t enum
   *
   * @return A libdc1394 struct of all supported video modes.
   *  If there wasn't a camera opened,
   *  then all fields of the struct are set to zero
   */
  dc1394framerates_t getCamFramerates(dc1394video_mode_t mode);

 
  // utility methods
    
  //! Translate a DC1394-int-framerate to a string
  std::string DC1394FrameRateToString(dc1394framerate_t framerate) const;
  
  //! Translate a DC1394-int-mode to a string
  std::string DC1394VideoModeToString(dc1394video_mode_t mode) const;
  
  //! Translate a DC1394-int-feature to a string
  std::string DC1394FeatureToString(dc1394feature_t feature) const;
  
  //! Translate a DC1394-int-feature to a CameraGrabber property string
  //!   features not supported by the backend will return an empty string
  std::string DC1394FeatureToCameraGrabberPropertyString(dc1394feature_t feature) const;

  //! Returns the last error string
  std::string getErrorString() const;

  //! Reset the the actual opened camera
  //!   A camera have to be opened before you can do this
  bool resetCam();
  
  //! Reset the firewire bus
  //!   a camera have to be opened before you can do this
  bool resetFwBus();


  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------

protected:
  // none yet
private:
  //! Firewire bus context
  dc1394_t* mpFwContext;
  
  //! List of all available cameras
  dc1394camera_list_t* mpCameraList;
  
  //! The actual opened Camera
  dc1394camera_t* mpCamera;
  
  //! Last error
  dc1394error_t mError;
  
  
  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // cedar::dev::sensors::camera::LibDcBase

#endif// CEDAR_USE_LIB_DC1394
#endif // CEDAR_DEV_SENSORS_CAMERA_LIBDCCAMERABASE_H

