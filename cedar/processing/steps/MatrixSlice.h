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

  void reset();

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
  //! Parameter used for determining the lower range of the matrix slice.
  cedar::aux::UIntVectorParameterPtr _mRangeLower;

  //! Parameter used for determining the upper range of the matrix slice.
  cedar::aux::UIntVectorParameterPtr _mRangeUpper;

}; // class cedar::proc::steps::MatrixSlice

#endif // CEDAR_PROCESSING_STEPS_MATRIX_SLICE_H
