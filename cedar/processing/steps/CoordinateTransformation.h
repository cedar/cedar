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

    File:        CoordinateTransformation.h

    Maintainer:  Ulja van Hengel
    Email:       ulja@ini.ruhr-uni-bochum.de
    Date:        2012 06 21

    Description: This Plugin provides coordinate transformation between Cartesian and polar coordinates and Cartesian
                 and log-polar coordinates.
                 User can set the sampling rate manually.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_STEPS_COORDINATE_TRANSFORMATION_H
#define CEDAR_PROC_STEPS_COORDINATE_TRANSFORMATION_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/EnumBase.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/processing/Step.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/processing/steps/CoordinateTransformation.fwd.h"

// SYSTEM INCLUDES

/*!@brief Enum describing coordinate transformation direction.
 *
 *        This parameter describes the coordinate transformation direction. It has two values:
 *        <ul>
 *          <li>@em Forward: Direction in order of appearance of coordinate system names.</li>
 *          <li>@em Backward: Reversed direction.</li>
 *        </ul>
 *
 * @todo fix error when angle-sampling rate is smaller than 1.
 */
class cedar::proc::steps::TransformationDirection
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! The enum id
  typedef cedar::aux::EnumId Id;

  //! Pointer type to the enum base object of this class.
  typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Initialization of the enum values.
  static void construct();

  //! Returns a reference to the base enum object.
  static const cedar::aux::EnumBase& type();

  //! Returns a pointer to the base enum object.
  static const cedar::proc::steps::TransformationDirection::TypePtr& typePtr();

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
public:
  //! The output image has the same size as the input.
  static const Id Forward = 0;

  //! The output image is large enough so that no values have to be discarded.
  static const Id Backward = 1;

protected:
  // none yet
private:
  //! The type object for this enum class.
  static cedar::aux::EnumType<cedar::proc::steps::TransformationDirection> mType;

}; // class cedar::proc::steps::TransformationDirection


/*!@brief Enum describing between which coordinate systems transformation is done.
 *
 *        This parameter describes between which coordinate systems transformation is done. It has two values:
 *        <ul>
 *          <li>@em CartPolar: Transformation between Cartesian and polar coordinates.</li>
 *          <li>@em CartLogPolar: Transformation between Cartesian and log-polar coordinates.</li>
 *        </ul>
 */
class cedar::proc::steps::TransformationType
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! The enum id
  typedef cedar::aux::EnumId Id;

  //! Pointer type to the enum base object of this class.
  typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Initialization of the enum values.
  static void construct();

  //! Returns a reference to the base enum object.
  static const cedar::aux::EnumBase& type();

  //! Returns a pointer to the base enum object.
  static const cedar::proc::steps::TransformationType::TypePtr& typePtr();

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
public:
  //! The output image has the same size as the input.
  static const Id CartPolar = 0;

  //! The output image is large enough so that no values have to be discarded.
  static const Id CartLogPolar = 1;

protected:
  // none yet
private:
  //! The type object for this enum class.
  static cedar::aux::EnumType<cedar::proc::steps::TransformationType> mType;

}; // class cedar::proc::steps::TransformationType


/*!@brief Transforms the coordinate system of an input image.
 *
 * Depending on the setup, this step performs different transformations.
 *
 * Let \f$I_{\mathrm{in}}\f$ be the input image and \f$I_{\mathrm{out}}\f$ be the output image.
 * When transforming from Cartesian coordinates to a log-polar representation, the output is calculated as
 * \f[
 *     I_{\mathrm{out}}(\varphi, \rho)
 *       = I_{\mathrm{in}}
 *         \left(
 *           c_x + \exp\left(\frac{\rho}{d_{\rho} \cdot m_{\rho}}\right) \cdot \sin\left(\varphi \cdot d_{\varphi}\right),
 *           c_y + \exp\left(\frac{\rho}{d_{\rho} \cdot m_{\rho}}\right) \cdot \cos\left(\varphi \cdot d_{\varphi}\right)
 *         \right),
 * \f]
 * where \f$\rho\f$ is the distance from the center, \f$\varphi\f$ is the angle, \f$c_x\f$ and \f$c_y\f$ specify the center of the image (and the origin of the transformation), \f$d_{\rho}\f$ is the samples per distance parameter, \f$m_{\rho}\f$ is the magnitude (forward) parameter, and \f$d_{\varphi}\f$ is the samples per degree parameter.
 *
 * With the same definitions, the inverse of this transformation is calculated as
 * \f[
 *     I_{\mathrm{out}}(x, y)
 *       = I_{\mathrm{in}}
 *         \left(
 *           m_{\rho} \cdot \log\left( \sqrt{(x - c_x)^2 + (y - c_y)^2} \right) \cdot d_{\rho},
 *           \mathrm{atan2}(y, x)
 *         \right).
 * \f]
 *
 * @todo Write down the equations for the non-logarithmic versions.
 * @todo There really only needs to be one magnitude parameter
 * @todo The output size (and possibly other unused parameters) should be constant when forward-transforming
 */
class cedar::proc::steps::CoordinateTransformation : public cedar::proc::Step
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  CoordinateTransformation();

  virtual ~CoordinateTransformation();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public slots:
  //!@brief Triggers a computation of a step.
  void recompute();

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
  //!@brief Checks if conected to new input.
  void inputConnectionChanged(const std::string& inputName);

  //!@brief General function that has to be called if transformation maps have to be recomputed (type independent).
  void createMap();

  //!@brief creates transformation map for Cartesian -> polar (and resizes output).
  void createCartPolarMapForward();

  //!@brief creates transformation map for polar -> Cartesian (and resizes output).
  void createCartPolarMapBackward();

  //!@brief creates transformation map for Cartesian -> log-polar (and resizes output).
  void createCartLogPolarMapForward();

  //!@brief creates transformation map for log-polar -> Cartesian (and resizes output).
  void createCartLogPolarMapBackward();

  //!@brief converts transformation maps from CV_32F into (CV_16SC2, CV_16UC1)
  void convertMap();

  //!@brief Annotates the current result matrix.
  void applyAnnotations();

  //!@brief Computes the coordinate transformation.
  void compute(const cedar::proc::Arguments&);

  /*! Allocates an output matrix with the given number of rows and columns (and the right size in the third dimension,
   *  if the input is three-dimensional.
   */
  void allocateOutput(int rows, int cols);

  /*! Returns the number of input rows and cols. In case of three-dimensional matrices, returns the sizes of the first
   *  and second dimension.
   */
  void getRowColSize(const cv::Mat& mat, unsigned int& rows, unsigned int& cols);

  static void getSetup(int& dim0, int& dim1, int sliced_dimension);

private slots:
  //! Reacts to a change in the transform direction.
  void transformDirectionChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
    //! Input Matrix
    cedar::aux::ConstMatDataPtr mInput;

    //! Transformed Matrix
    cedar::aux::MatDataPtr mOutput;

    //! X coordinates for transformation
    cedar::aux::MatDataPtr mMapX;

    //! Y coordinates for transformation
    cedar::aux::MatDataPtr mMapY;

    //! Transformation map X converted to CV_16SC for faster computation
    cedar::aux::MatDataPtr mMapXConverted;

    //! Transformation map Y converted to CV_16SC for faster computation
    cedar::aux::MatDataPtr mMapYConverted;

    //! saves the number of rows of the input to check if input size has changed so maps have to be recomputed
    unsigned int mInputRows;

    //! saves the number of columns of the input to check if input size has changed so maps have to be recomputed
    unsigned int mInputCols;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
    //! Defines between which two coordinate systems transformation is done
    cedar::aux::EnumParameterPtr _mTransformationType;

    //! Defines in which direction transformation is done
    cedar::aux::EnumParameterPtr _mTransformationDirection;

    //! Defines how many samples are done per degree. 0.5 samples per degree means a circle is sampled with 180 degree.
    cedar::aux::DoubleParameterPtr _mSamplesPerDegree;

    //! Defines how many samples are done per distance.
    cedar::aux::DoubleParameterPtr _mSamplesPerDistance;

    //! Defines the number of rows of the output matrix for the backward transformation
    cedar::aux::UIntParameterPtr _mNumberOfRows;

    //! Defines the number of columns of the output matrix for the backward transformation
    cedar::aux::UIntParameterPtr _mNumberOfCols;

    //! Defines the magnitude of the logarithmic scale for the forward log-polar-transformation
    cedar::aux::DoubleParameterPtr  _mMagnitudeForward;

    //! Defines the magnitude of the logarithmic scale for the backward log-polar-transformation
    cedar::aux::DoubleParameterPtr  _mMagnitudeBackward;

    //! Fills the center by offsetting the rho values by one.
    cedar::aux::BoolParameterPtr _mFillCenter;

    //! Dimension along which 2d slices are made when treating 3d inputs
    cedar::aux::UIntParameterPtr _mSlicedDimension;
}; // class cedar::proc::steps::CoordinateTransformation

#endif // CEDAR_PROC_STEPS_COORDINATE_TRANSFORMATION_H

