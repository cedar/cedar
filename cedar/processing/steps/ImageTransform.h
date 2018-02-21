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

    File:        ImageTransform.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2016 03 09

    Description: Header file for the class cedar::proc::steps::ImageTransform.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_IMAGE_TRANSFORM_H
#define CEDAR_PROC_STEPS_IMAGE_TRANSFORM_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include <cedar/processing/Step.h>
#include <cedar/processing/InputSlotHelper.h>
#include <cedar/auxiliaries/MatData.h>
#include <cedar/auxiliaries/IntVectorParameter.h>
#include <cedar/auxiliaries/DoubleParameter.h>
#include <cedar/auxiliaries/EnumParameter.h>

// FORWARD DECLARATIONS
#include "cedar/processing/steps/ImageTransform.fwd.h"

// SYSTEM INCLUDES


/*!
 */
class cedar::proc::steps::ImageTransform : public cedar::proc::Step
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //!@todo This is an exact copy from cedar's Resize step -- make the resize step's version available as a separate class and use it here.
  class Interpolation
  {
    public:
      //! Typedef for the interpolation id.
      typedef cedar::aux::EnumId Id;

      //! Shared-pointer typedef for the base type pointer.
      typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

      //! Static constructor function.
      static void construct()
      {
        mType.type()->def(cedar::aux::Enum(LINEAR, "LINEAR", "Linear"));
        mType.type()->def(cedar::aux::Enum(NEAREST, "NEAREST", "Nearest"));
        mType.type()->def(cedar::aux::Enum(AREA, "AREA", "Area"));
        mType.type()->def(cedar::aux::Enum(CUBIC, "CUBIC", "Cubic"));
        mType.type()->def(cedar::aux::Enum(LANCZOS4, "LANCZOS4", "Lanczos 4 (8 x 8)"));
      }

      //! Returns a const reference to the enum's type object.
      static const cedar::aux::EnumBase& type()
      {
        return *mType.type();
      }

      //! Returns a const reference to the pointer of the enum's type object.
      static const TypePtr& typePtr()
      {
        return mType.type();
      }

      //! Linear interpolation.
      static const Id LINEAR = cv::INTER_LINEAR;

      //! Nearest neighbor interpolation.
      static const Id NEAREST = cv::INTER_NEAREST;

      //! Area interpolation.
      static const Id AREA = cv::INTER_AREA;

      //! Cubic interpolation.
      static const Id CUBIC = cv::INTER_CUBIC;

      //! Lanczos 4 interpolation.
      static const Id LANCZOS4 = cv::INTER_LANCZOS4;

    private:
      //! Static pointer to the cedar::aux::EnumType object that manages the enum values.
      static cedar::aux::EnumType<Interpolation> mType;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ImageTransform();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void setRotationAmount(double angleInDegrees);

  double getRotationAmount() const;

  void setScaleFactor(double scaleFactor);

  double getScaleFactor() const;

  void setTranslation(int offsetX, int offsetY);

  void getTranslation(int& offsetX, int& offsetY) const;

  //! Resets the transformation parameters to identity transformations.
  void resetTransformation();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void compute(const cedar::proc::Arguments& arguments);
  void recompute(const cv::Mat& input);

  void updateTransformationMatrix();

  void inputConnectionChanged(const std::string& inputSlotName);

private slots:
  void transformationParameterChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief The image to be transformed.
  cedar::proc::InputSlotHelper<cedar::aux::MatData> mImage;

  //!@brief The output image.
  cedar::aux::MatDataPtr mOutput;

  cv::Mat mTransformationMatrix;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::aux::IntVectorParameterPtr _mTranslation;

  cedar::aux::DoubleParameterPtr _mRotation;

  cedar::aux::DoubleParameterPtr _mScaling;

  cedar::aux::EnumParameterPtr _mBorderType;

  cedar::aux::EnumParameterPtr _mInterpolationType;

}; // class cedar::proc::steps::ImageTransform

#endif // CEDAR_PROC_STEPS_IMAGE_TRANSFORM_H

