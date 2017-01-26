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

    File:        RecordingFormat.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2012 07 04

    Description:  Header for RecordingFormat enum-type class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_RECORDINGFORMAT_H
#define CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_RECORDINGFORMAT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/devices/sensors/visual/namespace.h"

// SYSTEM INCLUDES
//#include <opencv2/opencv.hpp>

/*!@brief Enum class to determine the recording format
 *
 *    Maps the FOR-CC to a CEDAR Enum class. <br>
 *
 *     fourcc 4-character code of codec used for encoding of the recordings.<br>
 *     Examples:<br>
 *     CV_FOURCC('P','I','M,'1') is a MPEG-1 codec<br>
 *     CV_FOURCC('M','J','P','G') is a motion-jpeg codec<br>
 *     CV_FOURCC('M','P','4','2') is also a motion-jpeg codec<br>
 *
 *     For supported codecs, have a look at:<br>
 *     /usr/local/src/OpenCV_{YOUR_VERSION}/modules/highgui/src/cap_ffmpeg_impl.hpp<br>
 *     http://www.fourcc.org/codecs.php<br>
 *
 */
class cedar::dev::sensors::visual::RecordingFormat
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

private:
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  // redefiniton of the CV_FOURCC inline function, because we need a static const value
  #define CEDAR_FOURCC(c1,c2,c3,c4) (((c1) & 255) + (((c2) & 255) << 8) + (((c3) & 255) << 16) + (((c4) & 255) << 24))

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
  static const cedar::dev::sensors::visual::RecordingFormat::TypePtr& typePtr();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  static cedar::aux::EnumType<cedar::dev::sensors::visual::RecordingFormat> mType;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:

  /// Record without any compression (i.e. RAW)
  static const Id RECORD_RAW = 0;

  /// Record in MP42 format
  static const Id RECORD_MP42 = CEDAR_FOURCC('M','P','4','2'); // the same as CV_FOURCC('M','P','4','2')

  /// Record in MJPG
  static const Id RECORD_MJPG = CEDAR_FOURCC('M','J','P','G');

  /// Record in MPEG-2
  static const Id RECORD_MPEG2 = CEDAR_FOURCC('m', 'p', 'g', '2');

  /// Record in h264
  static const Id RECORD_H264 = CEDAR_FOURCC('H', '2', '6', '4');

  /// Record in h263
  static const Id RECORD_H263 = CEDAR_FOURCC('H', '2', '6', '3');

  /// Record in vp3
  static const Id RECORD_VP3 = CEDAR_FOURCC('V', 'P', '3', '1');

protected:
  // none yet
private:
  // none yet

}; // cedar::dev::sensors::visual::RecordingFormat


#endif // CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_RECORDINGFORMAT_H

