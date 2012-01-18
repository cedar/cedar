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

    File:        CameraCapabilities.h

    Maintainer:  Georg.Hartinger
    Email:       georg.hartinger@rub.de
    Date:        2011 08 01

    Description: Header for the @em cedar::dev::sensors::visual::CameraCapabilities class.

    Credits:
 
======================================================================================================================*/

#ifndef CEDAR_DEV_SENSORS_VISUAL_CAMERA_CAPABILITIES_H
#define CEDAR_DEV_SENSORS_VISUAL_CAMERA_CAPABILITIES_H

// LOCAL INCLUDES
#include "../CameraGrabber.h"
#include "cedar/auxiliaries/ConfigurationInterface.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


typedef struct CameraPropertyCapabilityStruct
{
   cedar::dev::sensors::visual::CameraProperty::Id propId;
   //int value;
   int max_value;
   int min_value;
   bool is_supported;
   bool is_readable;
   bool is_one_push_capable;
   bool is_on_off_capable;
   bool is_auto_capable;
   bool is_manual_capable;
   bool is_absolute_capable;
} CameraPropertyCapability;


/*!@brief This class manages the capabilities of a camera
 *
 * It implements the configurationInterface private because
 * the configuration should never be written
 */
class cedar::dev::sensors::visual::CameraCapabilities
:
public cedar::aux::ConfigurationInterface
//private cedar::aux::ConfigurationInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  CameraCapabilities(
                      const std::string& configFileName
                    );

  //!@brief Destructor
  virtual ~CameraCapabilities();
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

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

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //returns the capabilities of a given camera property
  CameraPropertyCapability& getCapabilities(CameraProperty::Id propId);

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

  //contains every capability of a property
  std::vector<CameraPropertyCapability> mCamProperties;

private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  bool declareParameters();


}; // class cedar::dev::sensors::visual::CameraCapabilities

#endif // CEDAR_DEV_SENSORS_VISUAL_CAMERA_CAPABILITIES_H


