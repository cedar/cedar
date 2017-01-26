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

    File:        MatrixSlice.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 09 25

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROCESSING_STEPS_MATRIX_SLICE_H
#define CEDAR_PROCESSING_STEPS_MATRIX_SLICE_H

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/steps/MatrixSlice.fwd.h"

// SYSTEM INCLUDES
#include <vector>


/*!@brief Processing step that extracts a slice of a matrix.
 */
class cedar::proc::steps::MatrixSlice : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief An enum class defining types of anchors for a matrix slice.
   */
  class AnchorType
  {
  public:
    //! Type of the enum.
    typedef cedar::aux::EnumId Id;

    //! Pointer to the enumeration type.
    typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

    //! Constructs the enumeration values.
    static void construct()
    {
      mType.type()->def(cedar::aux::Enum(Absolute, "Absolute", "Absolute"));
      mType.type()->def(cedar::aux::Enum(Center, "Center", "Center"));
    }

    //! Returns the enum base class.
    static const cedar::aux::EnumBase& type()
    {
      return *mType.type();
    }

    //! Returns a pointer to the enum base class.
    static const cedar::proc::steps::MatrixSlice::AnchorType::TypePtr& typePtr()
    {
      return mType.type();
    }

    //! Anchor is specified relative to (0, 0) in the matrix.
    static const Id Absolute = 0;

    //! Anchor is specified relative to the center of the matrix.
    static const Id Center = 1;

  private:
    static cedar::aux::EnumType<cedar::proc::steps::MatrixSlice::AnchorType> mType;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  MatrixSlice();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void readConfiguration(const cedar::aux::ConfigurationNode& node);

  /*! Sets the range cut out by this step.
   */
  void setRanges(const std::vector<cv::Range>& ranges);

  //! Returns the range for the given dimension.
  cv::Range getRange(unsigned int dimension) const;

  //! Sets the anchor for the slice.
  void setAnchor(cedar::aux::EnumId anchor);

public slots:
  //! Called when the selected range of the slice changes.
  void rangeChanged();

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

  void allocateOutputMatrix();

  void updateDimensionality();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! Input matrix.
  cedar::aux::ConstMatDataPtr mInput;

  //! Output matrix.
  cedar::aux::MatDataPtr mOutput;

  //! Vector containing the ranges used for slicing the matrix.
  std::vector<cv::Range> mRanges;

  //! Used to remember limits from the configuration file.
  std::vector<cedar::aux::math::Limits<unsigned int> > mStoredLimits;


  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! How the position of the slice is determined.
  cedar::aux::EnumParameterPtr _mAnchorType;

  //! Parameter used for determining the lower range of the matrix slice.
  cedar::aux::UIntVectorParameterPtr _mRangeLower;

  //! Parameter used for determining the upper range of the matrix slice.
  cedar::aux::UIntVectorParameterPtr _mRangeUpper;

}; // class cedar::proc::steps::MatrixSlice

#endif // CEDAR_PROCESSING_STEPS_MATRIX_SLICE_H
