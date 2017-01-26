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

    File:        ColorConversion.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 07 03

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_COLOR_CONVERSION_H
#define CEDAR_PROC_STEPS_COLOR_CONVERSION_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/auxiliaries/EnumParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/auxiliaries/annotation/ColorSpace.fwd.h"
#include "cedar/processing/steps/ColorConversion.fwd.h"

// SYSTEM INCLUDES


/*!@brief A processing step that converts an image from one color space to another.
 */
class cedar::proc::steps::ColorConversion : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Enum class for color spaces.
  class ColorSpace
  {
    public:
      //! the id of an enum entry
      typedef cedar::aux::EnumId Id;

      //! constructs the enum for all ids
      static void construct()
      {
        mType.type()->def(cedar::aux::Enum(AUTO, "AUTO"));
        mType.type()->def(cedar::aux::Enum(BGR, "BGR"));
        mType.type()->def(cedar::aux::Enum(HSV, "HSV"));
        mType.type()->def(cedar::aux::Enum(YUV, "YUV"));
        mType.type()->def(cedar::aux::Enum(YCrCb, "YCrCb"));
        mType.type()->def(cedar::aux::Enum(Lab, "Lab"));
      }

      //! @returns A const reference to the base enum object.
      static const cedar::aux::EnumBase& type()
      {
        return *(mType.type());
      }

      //! @returns A pointer to the base enum object.
      static const cedar::proc::DataRole::TypePtr& typePtr()
      {
        return mType.type();
      }

    public:
      //! flag for automatically determining the color space (using annotations)
      static const Id AUTO = 0;
      //! flag for BGR color space (blue, green, red)
      static const Id BGR = 1;
      //! flag for HSV color space (hue, saturation, value)
      static const Id HSV = 2;
      //! flag for YUV color space (luma, chrominance)
      static const Id YUV = 3;
      //! Alias for YUV
      static const Id YCrCb = 4;
      //! Alias for LAB
      static const Id Lab = 5;

    private:
      static cedar::aux::EnumType<ColorSpace> mType;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ColorConversion();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Returns the color space into which the input is transformed.
   */
  inline ColorSpace::Id getTargetColorSpace() const
  {
    return this->_mTargetType->getValue();
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Determines whether the data item can be connected to the slot.
  cedar::proc::DataSlot::VALIDITY determineInputValidity
                                  (
                                    cedar::proc::ConstDataSlotPtr slot,
                                    cedar::aux::ConstDataPtr data
                                  ) const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Reacts to a change in the input connection.
  void inputConnectionChanged(const std::string& inputName);

  //!@brief Updates the output matrix.
  void compute(const cedar::proc::Arguments& arguments);

private slots:
  //!@brief Updates the constant passed to the cv::convert function.
  void updateCvConvertConstant();

  //!@brief Determines the color space of the source image
  void updateSourceImageColorSpace();

  //!@brief Updates the color space of the target image.
  void updateTargetImageColorSpace();

  //!@brief Computes the output again.
  void recompute();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::aux::ConstMatDataPtr mInput;
  cedar::aux::annotation::ConstColorSpacePtr mInputColorSpaceAnnotation;
  ColorSpace::Id mInputColorSpace;

  cedar::aux::MatDataPtr mOutput;

  int mCvConversionConstant;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! Type of the source image. Usually determined automatically.
  cedar::aux::EnumParameterPtr _mSourceType;

  //! Type of the target image.
  cedar::aux::EnumParameterPtr _mTargetType;

}; // class cedar::proc::steps::ColorConversion

#endif // CEDAR_PROC_STEPS_COLOR_CONVERSION_H

