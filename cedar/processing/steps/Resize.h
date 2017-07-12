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

    File:        Resize.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 02

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_RESIZE_H
#define CEDAR_PROC_STEPS_RESIZE_H

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/processing/steps/Resize.fwd.h"

// SYSTEM INCLUDES
#include <utility>
#include <vector>


/*!@brief   A processing step that resizes a matrix.
 *
 *          This step can resize an input matrix of any dimensionality to a matrix with the same dimensionality
 *          but a different shape.
 *
 * @remarks Note, that for more that two dimensions, only linear interpolation is implemented. Any other choice will
 *          default to this method.
 */
class cedar::proc::steps::Resize : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

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
  Resize();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Updates the output matrix.
  void compute(const cedar::proc::Arguments& arguments);

  //! Sets the output size for the given dimension.
  void setOutputSize(unsigned int dimension, unsigned int size);

  //! Returns the output size for the given dimension.
  unsigned int getOutputSize(unsigned int dimension) const;

public slots:
  //!@brief Slot that reacts to changes in the output size.
  void outputSizeChanged();

  //!@brief Slot that triggers the computation of the step.
  void recompute();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief reads a configuration from a node
   * @param node the said node
   */
  void readConfiguration(const cedar::aux::ConfigurationNode& node);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Reacts to a change in the input connection.
  void inputConnectionChanged(const std::string& inputName);

  /*!@brief   Returns a cv::Size that contains the correct sizes according to the step's parameters.
   *
   * @remarks Due to the nature of opencv's matrix size handling, this funciton only works for matrices with 2 or less
   *          dimensions.
   */
  cv::Size getOutputSize() const;

  /*!@brief A recursive interpolation function for n-dimensional tensors.
   */
  double linearInterpolationND
  (
    const cv::Mat& source,
    cv::Mat& target,
    const std::vector<int>& targetIndex
  );

  /*!@brief A recursive helper function for n-dimensional linear interpolation of tensors.
   */
  void linearInterpolationNDRecursion
  (
    const cv::Mat& source,
    cv::Mat& target,
    const std::vector<std::pair<int, int> >& bounds,
    std::vector<double>& interpolatedIndices,
    const std::vector<int>& targetIndex,
    int currentDimension,
    std::vector<double>& interpolatedValues
  );

  /*!@brief Adapts the size of the output matrix.
   */
  void updateOutputMatrixSize();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief MatrixData representing the input. Storing it like this saves time during computation.
  cedar::aux::ConstMatDataPtr mInput;

  //!@brief The data containing the output.
  cedar::aux::MatDataPtr mOutput;
private:

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief The factor by which the input is multiplied.
  cedar::aux::UIntVectorParameterPtr _mOutputSize;

  /*!@brief The type of interpolation performed by the step.
   *
   */
  cedar::aux::EnumParameterPtr _mInterpolationType;

private:

}; // class cedar::proc::steps::Resize

#endif // CEDAR_PROC_STEPS_RESIZE_H

