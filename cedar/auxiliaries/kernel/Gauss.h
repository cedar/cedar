/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

// LOCAL INCLUDES
#include "auxiliaries/namespace.h"
#include "auxiliaries/kernel/namespace.h"
#include "auxiliaries/kernel/Separable.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

/*!@brief Gauss kernel class.
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
  Gauss();
  Gauss(
         double amplitude,
         std::vector<double> sigmas,
         std::vector<double> shifts,
         double limit,
         unsigned int dimensionality,
         const std::string& kernelFile = "dummy_matrix_file.yml"
       );
  //!@brief Destructor
  virtual ~Gauss();
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! virtual function that in the derived classes actually does the job of initializing
   * the kernel from file
   */
  //!@todo deal with boost PropertyTree here
  virtual void onInit();

  double getSigma(unsigned int dimension) const;

  /*!\brief set the sigma of a chosen dimension and Gaussian
   * \param dimension the dimension
   * \param sigma the new \f$\sigma\f$
   */
  void setSigma(unsigned int dimension, double sigma);

  double getShift(unsigned int dimension) const;

  void setShift(unsigned int dimension, double shift);

  double getAmplitude() const;

  void setAmplitude(double amplitude);

  unsigned int getWidth(unsigned int dim) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected slots:
  /*!\brief virtual function to calculate the kernel matrix
   */
  void calculate();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  std::vector<unsigned int> mSizes; //!< sizes for kernel matrix
  std::vector<int> mCenters; //!< matrix indices for kernel centers
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  cedar::aux::DoubleParameterPtr _mAmplitude; //!<
//  std::vector<double> _mSigmas; //!< index resemble dimension
  cedar::aux::DoubleVectorParameterPtr _mSigmas; //!< index resemble dimension
//  std::vector<double> _mShifts; //!< index resembles dimension
  cedar::aux::DoubleVectorParameterPtr _mShifts; //!< index resemble dimension
  cedar::aux::DoubleParameterPtr _mLimit; //!< variable that defines the precision of the numerical approximation at the borders
private:
  // none yet

}; // class cedar::aux::kernel::Gauss

#endif // CEDAR_AUX_KERNEL_GAUSS_H

