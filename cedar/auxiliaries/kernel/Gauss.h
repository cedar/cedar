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

    File:        Gauss.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 07 07

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_KERNEL_GAUSS_H
#define CEDAR_AUX_KERNEL_GAUSS_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/kernel/Separable.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/kernel/Gauss.fwd.h"

// SYSTEM INCLUDES
#include <vector>


/*!@brief A class for a convolution kernel that has the shape of a Gaussian distribution.
 *
 */
class cedar::aux::kernel::Gauss : public cedar::aux::kernel::Separable
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
  Gauss
  (
    unsigned int dimensionality = 2,
    double amplitude = 1.0,
    double sigmas = 3.0,
    double shifts = 0.0,
    double limit = 5.0
  );

  //!@brief Constructor to create an instance of Gauss directly from a set of parameters (without configuration file).
  Gauss(
         double amplitude,
         std::vector<double> sigmas,
         std::vector<double> shifts,
         double limit,
         unsigned int dimensionality
       );
  //!@brief Destructor
  virtual ~Gauss();
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!\brief get the sigma of a given dimension
  double getSigma(unsigned int dimension) const;

  /*!\brief set the sigma of a chosen dimension and Gaussian
   * \param dimension the dimension
   * \param sigma the new \f$\sigma\f$
   */
  void setSigma(unsigned int dimension, double sigma);

  //!@brief get the shift of the kernel for a given dimension
  double getShift(unsigned int dimension) const;

  //!@brief set the shift for a given dimension
  void setShift(unsigned int dimension, double shift);

  //!@brief get the amplitude of the kernel
  double getAmplitude() const;

  //!@brief set the amplitude of the kernel
  void setAmplitude(double amplitude);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected slots:
  //!@brief update the dimensionality of the kernel matrices, triggered by a signal (e.g. a changed parameter value)
  void updateDimensionality();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*! virtual function that in the derived classes actually does the job of initializing
   * the kernel from file
   */
  virtual void onInit();

  /*!@brief virtual function to calculate the kernel matrix
   */
  void calculateParts();

  //!@brief A function that heuristically determines width of the kernel in pixels.
  unsigned int estimateWidth(unsigned int dim) const;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief vector of pixel sizes of the kernel matrices
  std::vector<unsigned int> mSizes;
  //!@brief matrix indices for kernel centers
  std::vector<int> mCenters;
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief amplitude of the kernel - after normalization of each dimension, this is applied to the first one
  cedar::aux::DoubleParameterPtr _mAmplitude;

  //!@brief sigmas of the Gauss function for each dimension
  cedar::aux::DoubleVectorParameterPtr _mSigmas;

  //! Whether or not the kernel is normalized.
  cedar::aux::BoolParameterPtr _mNormalize;

  //!@brief shift of the Gauss function from the center for each dimension
  cedar::aux::DoubleVectorParameterPtr _mShifts;

  //!@brief scalar value, which is multiplied by the dimensions' sigmas to determine the pixel size
  cedar::aux::DoubleParameterPtr _mLimit;
private:
  // none yet

}; // class cedar::aux::kernel::Gauss

#endif // CEDAR_AUX_KERNEL_GAUSS_H
