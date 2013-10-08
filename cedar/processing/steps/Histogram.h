/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/processing/steps/namespace.h"
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/auxiliaries/EnumParameter.h"

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
  class Normalization
  {
    public:
      typedef cedar::aux::EnumId Id;
      typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

    public:
      static void construct()
      {
        mType.type()->def(cedar::aux::Enum(None, "None"));
        mType.type()->def(cedar::aux::Enum(ImageSize, "ImageSize", "Image Size"));
        mType.type()->def(cedar::aux::Enum(Maximum, "Maximum"));
      }

      static const cedar::aux::EnumBase& type()
      {
        return *mType.type();
      }

      static const TypePtr& typePtr()
      {
        return mType.type();
      }

    public:
      static const Id None = 0;
      static const Id ImageSize = 1;
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

  cedar::proc::DataSlot::VALIDITY determineInputValidity
  (
    cedar::proc::ConstDataSlotPtr,
    cedar::aux::ConstDataPtr data
  ) const;

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

