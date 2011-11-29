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

    File:        CameraIsoSpeed.h

    Maintainer:  Georg Hartinger
    Email:       georg.hartinger@ini.rub.de
    Date:        2011 08 01

    Description:  Header for CameraIsoSpeed enum-type class

    Credits:

======================================================================================================================*/

#ifndef CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_ISO_SPEED_H
#define CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_ISO_SPEED_H

// LOCAL INCLUDES
//#include "cedar/auxiliaries/EnumType.h"
#include "../../../auxiliaries/EnumType.h"
#include "namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*!@brief Enum class for firewire ISO-speed
 *
 * Use this type for the CameraGrabber::setCameraInitIsoSpeed() and getCameraInitIsoSpeed() method
 */
class cedar::dev::sensors::visual::CameraIsoSpeed
{
  //--------------------------------------------------------------------------------------------------------------------
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------
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
  static const cedar::dev::sensors::visual::CameraIsoSpeed::TypePtr& typePtr(); 
  
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  static cedar::aux::EnumType<cedar::dev::sensors::visual::CameraIsoSpeed> mType;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  static const Id ISO_100 = 100;
  static const Id ISO_200 = 200;
  static const Id ISO_400 = 400;
  static const Id ISO_800 = 800;
  static const Id ISO_1600 = 1600;
  static const Id ISO_3200 = 3200;
    
protected:
  // none yet
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
  // none yet

}; // class cedar::xxx

#endif // CEDAR_CEDAR_DEV_SENSORS_VISUAL_CAMERA_ISO_SPEED_H

