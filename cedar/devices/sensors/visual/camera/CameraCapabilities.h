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

#include "cedar/configuration.h"   // MAKE FIREWIRE OPTIONAL
#ifdef CEDAR_USE_LIB_DC1394

// CEDAR INCLUDES
#include "cedar/devices/sensors/visual/CameraGrabber.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/devices/sensors/visual/namespace.h"

// SYSTEM INCLUDES


/*! \class cedar::dev::sensors::visual::CameraCapabilities
 *  \brief This class manages the capabilities of a camera
 */
class cedar::dev::sensors::visual::CameraCapabilities
:
public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //!@cond SKIPPED_DOCUMENTATION

  /*! \brief Stores the capability of a property
   *
   *    Every property have several capabilities and a defined range
   *    where the values can be set.
   *    This struct stores flags for all capabilities and the range of the values
   */
  struct CameraPropertyCapability
  {
     cedar::dev::sensors::visual::CameraProperty::Id propId;
     int max_value;  // @!todo set max/min in IntParameter for state-properties
     int min_value;
     bool is_supported;
     bool is_readable;
     bool is_one_push_capable;  //@!todo: reduce capabilties to some often used
     bool is_on_off_capable;
     bool is_auto_capable;
     bool is_manual_capable;
     bool is_absolute_capable;  //del
  } ;

  //!@endcond

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  CameraCapabilities();

  //!@brief Destructor
  virtual ~CameraCapabilities();
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*! \brief Get the minimum possible value that can be set of the given property
   *  \param propId The id of the property
   */
  int getMinValue(CameraProperty::Id propId);

  /*! \brief Get the maximum possible value that can be set of the given property
   *  \param propId The id of the property
   */
  int getMaxValue(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property is supported by the used camera
   *  \param propId The id of the  property
   */
  bool isSupported(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property is readable by the used camera
   *  \param propId The id of the  property
   */
  bool isReadable(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property supports the OnePushAuto mode
   *
   *     OnePushAuto is a special mode. <br>
   *     It is used as follows: Set a value to a property and then to OnePushAuto mode.
   *     The camera now will try to hold this value automatically.
   *  \param propId The id of the  property
   */
  bool isOnePushCapable(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property could be turn off and on
   *  \param propId The id of the  property
   */
  bool isOnOffCapable(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property can be set to auto-mode
   *  \param propId The id of the  property
   */
  bool isAutoCapable(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property can be set manually
   *  \param propId The id of the  property
   */
  bool isManualCapable(CameraProperty::Id propId);

  /*! \brief This method tells you, if the given property can be set to an absolute value
   *  \param propId The id of the  property
   */
  bool isAbsoluteCapable(CameraProperty::Id propId);


  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  ///! Returns the capability struct of a given camera property
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

  ///! Contains every property of the camera (the supported as well as the not supported ones)
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
  ///! All properties and settings declared in this method for the ConfigurationInterface class
  bool declareParameters();


}; // class cedar::dev::sensors::visual::CameraCapabilities

#endif // CEDAR_USE_LIB_DC1394

#endif // CEDAR_DEV_SENSORS_VISUAL_CAMERA_CAPABILITIES_H
