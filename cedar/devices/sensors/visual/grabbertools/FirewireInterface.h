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

    File:        FirewireInterface.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 12 11

    Description: Header for the @em FirewireInterface class.

    Credits:

======================================================================================================================*/

#ifndef FIREWIRE_INTERFACE_H
#define FIREWIRE_INTERFACE_H


// MAKE FIREWIRE OPTIONAL
#include "cedar/devices/robot/CMakeDefines.h"
#ifdef CEDAR_USE_LIB_DC1394
//#pragma message "compiling with DC1394 support"

// LOCAL INCLUDES
#include "cedar/devices/sensors/visual/grabbertools/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <inttypes.h>
#include <dc1394/dc1394.h>
#include <iostream>
#include <vector>

/*!@brief This class is a simple interface to the firewire bus
 *
 * \remarks: Opens the bus and handles the firewire device context handle.<br>
 *  USAGE:
 *    1. scan bus
 *    2. select device
 *    3. open it
 *    4. work with opened device
 *    5. close device
 */
class cedar::dev::sensors::visual::grabbertools::FirewireInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  #define EXCEPTION_THROW_ON_ERR throw (getErrorString());
  #define EXCEPTION_THROW_ON_NO_CAM cleanUp(); throw ("No camera handle");

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  //opens the firewire bus context and query the number of available cameras
  FirewireInterface();
  
  //!@brief Destructor
  //close all firewire related handles
  ~FirewireInterface();
  
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void cleanUp();
  
  //after creation of the class, this method gives the camera count
  unsigned int getNumCams();
  
  //return 0 if unsupported cam nr
  uint64_t getCamGuid(unsigned int camNr);

  //print out all features of the currently opened camera
  void printAllFeatures();
  
 
  //camera methods:

  //opens a given camera
  //methods like getCamFeatures rely on an opend camera
  bool openCamera(uint64_t camGuid);
  
  //close actual opened camera, call this method at the end of your work
  //or just before you like to open another camera
  void closeCamera();
  

  //get the name of all features of a camera
  std::vector<std::string> getAllFeaturesAsString();
  
  //returns a featureset with all features and all informations about a feature
  dc1394featureset_t getCamFeatures();
  

  //get all supported video modes
  dc1394video_modes_t getCamVideoModes();
  
  //get all supported framerates 
  //this depend on the video mode
  dc1394framerates_t getCamFramerates(dc1394video_mode_t mode);

 
  //utility methods
    
  //translate a DC1394-int-framerate to a string
  std::string DC1394FrameRateToString(dc1394framerate_t framerate);
  
  //translate a DC1394-int-mode to a string
  std::string DC1394VideoModeToString(dc1394video_mode_t mode) ;
  
  //translate a DC1394-int-feature to a string
  std::string DC1394FeatureToString(dc1394feature_t feature);
  
  //translate a DC1394-int-feature to a CameraGrabber property string
  //features not supported by the backend will return an empty string
  std::string DC1394FeatureToCameraGrabberPropertyString(dc1394feature_t feature);  

  //read the last error string
  std::string getErrorString();

  //reset the the actual opened camera
  //a camera have to be opened before you can do this
  bool resetCam();
  
  //reset the firewire bus
  //a camera have to be opened before you can do this
  bool resetFwBus();


  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:



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
  // none yet
private:
  //firewire bus context
  dc1394_t* mpFwContext;
  
  //list of all available cameras
  dc1394camera_list_t* mpCameraList;
  
  //the actual opened Camera
  dc1394camera_t* mpCamera;
  
  //last error
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

}; // cedar::dev::sensors::visual::grabbertools::FirewireInterface

// MAKE FIREWIRE OPTIONAL
#endif// CEDAR_USE_LIB_DC1394
#endif // FIREWIRE_INTERFACE_H

