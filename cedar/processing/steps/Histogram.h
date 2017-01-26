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

    File:        Histogram.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 08 13

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_HISTOGRAM_H
#define CEDAR_PROC_STEPS_HISTOGRAM_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/auxiliaries/EnumParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/steps/Histogram.fwd.h"

// SYSTEM INCLUDES


/*!@brief Calculates a histogram out of its input image.
 */
class cedar::proc::steps::Histogram : public cedar::proc::Step
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! Enumeration class for the different types of normalizations.
   */
  class Normalization
  {
    public:
      //! Type id of the enumeration.
      typedef cedar::aux::EnumId Id;

      //! Pointer type of the base class.
      typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

    public:
      //! Registers the enum values.
      static void construct()
      {
        mType.type()->def(cedar::aux::Enum(None, "None"));
        mType.type()->def(cedar::aux::Enum(ImageSize, "ImageSize", "Image Size"));
        mType.type()->def(cedar::aux::Enum(Maximum, "Maximum"));
      }

      //! Returns the enumeration type.
      static const cedar::aux::EnumBase& type()
      {
        return *mType.type();
      }

      //! Returns a pointer to an enumeration type.
      static const TypePtr& typePtr()
      {
        return mType.type();
      }

    public:
      //! No normalization.
      static const Id None = 0;

      //! Normalization by image size.
      static const Id ImageSize = 1;

      //! Normalization by the maximum value of the histogram.
      static const Id Maximum = 2;

    private:
      static cedar::aux::EnumType<Normalization> mType;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Histogram();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Returns the number of bins of the histogram.
  inline unsigned int getNumBins() const
  {
    return this->_mBins->getValue();
  }

  //! Sets the number of bins in the histogram.
  void setNumberOfBins(unsigned int numberOfBins);

  //!@brief Returns the lower range of the histogram.
  inline double getRangeLower() const
  {
    return this->_mRangeLower->getValue();
  }

  //!@brief Returns the upper range of the histogram.
  inline double getRangeUpper() const
  {
    return this->_mRangeUpper->getValue();
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
  void compute(const cedar::proc::Arguments&);

  void inputConnectionChanged(const std::string& inputName);

private slots:
  void recompute();

  void numberOfBinsChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // inputs
  //! Input image from which to take the histogram.
  cedar::aux::ConstMatDataPtr mInputImage;

  //! Optional mask. If set, only pixels where the mask is non-zero are counted for the histogram.
  cedar::aux::ConstMatDataPtr mMask;

  // buffers
  //! Raw (unnormalized) histogram.
  cedar::aux::MatDataPtr mRawHistogram;

  // outputs
  //! Histogram of values in the input.
  cedar::aux::MatDataPtr mHistogram;

  double mMaxValue;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! The number of bins in the histogram.
  cedar::aux::UIntParameterPtr _mBins;

  //! The lower limit of the first bin of the histogram.
  cedar::aux::DoubleParameterPtr _mRangeLower;

  //! The upper limit of the last bin of the histogram.
  cedar::aux::DoubleParameterPtr _mRangeUpper;

  //! The normalization type
  cedar::aux::EnumParameterPtr _mNormalizationType;

}; // class cedar::proc::steps::Histogram

#endif // CEDAR_PROC_STEPS_HISTOGRAM_H

