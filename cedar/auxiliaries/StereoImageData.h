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

    File:        StereoImageData.h

    Maintainer:  Irina Popova,

    Email:       irina.popova@ini.ruhr-uni-bochum.de,

    Date:        2012 04 02

    Description: Header for the cedar::aux::StereoImageData.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_STEREO_IMAGE_DATA_H
#define CEDAR_AUX_STEREO_IMAGE_DATA_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/Data.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/StereoImageData.fwd.h"

// SYSTEM INCLUDES
//#include <QReadWriteLock>

/*!@brief A class to use stereo images.
 *
 * Contains two matrices, one for the left and one for the right image.
 *
 */

class cedar::aux::StereoImageData : public cedar::aux::Data
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  StereoImageData()
  {
  }

  //!@brief This constructor initializes the internal data to a left und right images.
  StereoImageData(const cv::Mat& image_left, const cv::Mat& image_right)
  {
    this->mLeftImage = image_left;
    this->mRightImage = image_right;
  }

  //!@brief Destructor
  virtual ~StereoImageData()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  //!@brief returns the left image.
  cv::Mat& getLeftImage()
  {
    return this->mLeftImage;
  }

  //!@brief returns the right image.
  cv::Mat& getRightImage()
  {
    return this->mRightImage;
  }

  //!@brief returns the left image as a constant.
  inline const cv::Mat& getLeftImage() const
  {
    return this->mLeftImage;
  }

  //!@brief returns the right image as a constant.
  const cv::Mat& getRightImage() const
  {
    return this->mRightImage;
  }

  //!@brief sets the internal data to the given data.
  void setImages(const cv::Mat& image_left, const cv::Mat& image_right)
  {
    this->mLeftImage = image_left;
    this->mRightImage = image_right;
  }

  //!@brief sets the left image to the given data.
  void setLeftImage(const cv::Mat& image_left)
  {
    this->mLeftImage = image_left;
  }

  //!@brief sets the right image to the given data.
  void setRightImage(const cv::Mat& image_right)
  {
    this->mRightImage = image_right;
  }

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
  //!@brief the left image
  cv::Mat mLeftImage;
  //!@brief the right image
  cv::Mat mRightImage;
private:
  // none yet
}; // class cedar::aux::StereoImageData

#endif // CEDAR_AUX_STEREO_IMAGE_DATA_H

