/*======================================================================================================================

 Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

 File:        Camera.cpp

 Maintainer:  Jan Tekülve
 Email:       jan.tekuelve@ini.rub.de
 Date:        2016 07 05

 Description: Source file for the class cedar::dev::Camera.

 Credits:

 ======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/devices/Camera.h"

// CEDAR INCLUDES

// SYSTEM INCLUDES

const cedar::dev::Component::ComponentDataType cedar::dev::Camera::CAMERA_PICTURE = 301;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::Camera::Camera()
:
    xDim(new cedar::aux::UIntParameter(this, "pictureSizeX", 2)),
    yDim(new cedar::aux::UIntParameter(this, "pictureSizeY", 2))
{
  init();
}

cedar::dev::Camera::~Camera()
{
  prepareComponentDestructAbsolutelyRequired();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
cv::Mat cedar::dev::Camera::getData()
{
  return getUserSideMeasurementBuffer(CAMERA_PICTURE);
}

std::vector<unsigned long> cedar::dev::Camera::getPictureSizes()
{
  std::vector<unsigned long> returnVector = {xDim->getValue(),yDim->getValue()};
  return returnVector;
}

unsigned int cedar::dev::Camera::getPictureSizeX()
{
  return xDim->getValue();
}

unsigned int cedar::dev::Camera::getPictureSizeY()
{
  return yDim->getValue();
}

void cedar::dev::Camera::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  cedar::aux::NamedConfigurable::readConfiguration(node);
  initializeFromConfiguration();
}

void cedar::dev::Camera::init()
{
  installMeasurementType(cedar::dev::Camera::CAMERA_PICTURE, "Camera Picture");
  initializeFromConfiguration();
}

void cedar::dev::Camera::initializeFromConfiguration()
{
  setMeasurementDimensionality( cedar::dev::Camera::CAMERA_PICTURE, this->getPictureSizes() );
}
