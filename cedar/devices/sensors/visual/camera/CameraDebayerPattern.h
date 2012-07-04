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

    File:        CameraDebayerPattern.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Header for CameraDebayerPattern enum-type class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_CAMERADEBAYERPATTERN_H
#define CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_CAMERADEBAYERPATTERN_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/devices/sensors/visual/namespace.h"

// SYSTEM INCLUDES

/*!@brief Enum class to determine if a camera needs to be converted from a bayer-pattern to
 *  the internal used BGR-format of cv::Mat
 */
class cedar::dev::sensors::visual::CameraDebayerPattern
{
  //--------------------------------------------------------------------------------------------------------------------
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------
//!@cond SKIPPED_DOCUMENTATION
public:
  typedef cedar::aux::EnumId Id;
public:
  typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.

  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  static void construct();

  static const cedar::aux::EnumBase& type();
  static const cedar::dev::sensors::visual::CameraDebayerPattern::TypePtr& typePtr();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  static cedar::aux::EnumType<cedar::dev::sensors::visual::CameraDebayerPattern> mType;
  //!@endcond

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:


  /// No conversion needed
  static const Id NONE = 0;

  /// Convert from blue-red to BGR
  static const Id BG_TO_BGR = CV_BayerBG2BGR; // = 46

  /// Convert from green-blue to BGR
  static const Id GB_TO_BGR = CV_BayerGB2BGR; // = 47

  /// Convert from red-green to BGR
  static const Id RG_TO_BGR = CV_BayerRG2BGR; // = 48

  /// Convert from green-red to BGR
  static const Id GR_TO_BGR = CV_BayerGR2BGR; // = 49


protected:
  // none yet
private:
  // none yet

}; // cedar::dev::sensors::visual::CameraDebayerPattern


#endif // CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_CAMERADEBAYERPATTERN_H

