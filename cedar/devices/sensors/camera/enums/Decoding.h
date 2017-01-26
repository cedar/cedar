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

    File:        DeBayerFilter.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Header for DeBayerFilter enum-type class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_CEDAR_DEV_SENSORS_CAMERA_DECODING_H
#define CEDAR_CEDAR_DEV_SENSORS_CAMERA_DECODING_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/devices/sensors/camera/namespace.h"

// SYSTEM INCLUDES
//#include <opencv2/opencv.hpp>

/*!@brief Enum class to determine the applied bayer pattern (if needed)
 *
 *  For all available filters have a look at:
 *      &lt;OpenCV_dir&gt;/modules/imgproc/include/opencv2/imgproc/types_c.h
 *
 *  cv::Mat stores the images in BGR format, so only that filters mapped here
 *
 */
class cedar::dev::sensors::camera::Decoding
{
  //--------------------------------------------------------------------------------------------------------------------
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Typedef for the enum values
  typedef cedar::aux::EnumId Id;
public:
  //! Typedef for this enum as shared pointer
  typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Construct
  static void construct();

  //! Type
  static const cedar::aux::EnumBase& type();

  //! Type Pointer
  static const cedar::dev::sensors::camera::Decoding::TypePtr& typePtr();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  static cedar::aux::EnumType<cedar::dev::sensors::camera::Decoding> mType;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:

  /// No bayer filter applied
  static const Id NONE = 0;

  /// Apply  bayer filter Blue-Green to BGR
  static const Id FROM_BG = cv::COLOR_BayerBG2BGR;

  /// Apply  bayer filter Green-Blue to BGR
  static const Id FROM_GB = cv::COLOR_BayerGB2BGR;

  /// Apply  bayer filter Red-Green to BGR
  static const Id FROM_RG = cv::COLOR_BayerRG2BGR;

  /// Apply  bayer filter Green-Red to BGR
  static const Id FROM_GR = cv::COLOR_BayerGR2BGR;

protected:
  // none yet
private:
  // none yet

}; // cedar::dev::sensors::camera::Decoding


#endif // CEDAR_CEDAR_DEV_SENSORS_CAMERA_DECODING_H

