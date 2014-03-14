/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        ImageData.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
    Date:        2011 07 22

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_IMAGE_DATA_H
#define CEDAR_AUX_IMAGE_DATA_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/annotation/ColorSpace.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/ImageData.fwd.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>

/*!@brief This is a data class representing images.
 */
CEDAR_DECLARE_DEPRECATED(class) cedar::aux::ImageData : public cedar::aux::MatData
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ImageData(const cv::Mat& value = cv::Mat())
  :
  cedar::aux::MatData(value)
  {
    this->setAnnotation
    (
      cedar::aux::annotation::ColorSpacePtr
      (
        new cedar::aux::annotation::ColorSpace
        (
          cedar::aux::annotation::ColorSpace::Blue,
          cedar::aux::annotation::ColorSpace::Green,
          cedar::aux::annotation::ColorSpace::Red
        )
      )
    );
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

}; // class cedar::aux::ImageData

#endif // CEDAR_AUX_IMAGE_DATA_H

