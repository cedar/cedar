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

    File:        Gauss.h

    Maintainer:  Alexander Gluchow
    Email:       
    Date:        2016 05 19

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_KERNEL_ROTATEDGAUSS_H
#define CEDAR_AUX_KERNEL_ROTATEDGAUSS_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/kernel/Kernel.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/kernel/RotatedGauss.fwd.h"

// SYSTEM INCLUDES
#include <vector>


/*!@brief A class for a convolution kernel that has the shape of a Gaussian distribution.
 *
 */
class cedar::aux::kernel::RotatedGauss : public cedar::aux::kernel::Kernel
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

 RotatedGauss();

  //!@brief The standard destructor.
  virtual ~RotatedGauss();
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  	  double getAlpha();

  	  void setAlpha(double alpha);

  	  double getSigma(int dim);

  	  void setSigma(unsigned int dim, int sigma);

  	  double getAmplitude();

  	  void setAmplitude(double amplitude);

  	  unsigned int estimateWidth(unsigned int dim);

  	  bool getNormalize();

  	  void setNormalize(bool normalize);


public slots:

//--------------------------------------------------------------------------------------------------------------------
// protected methods
//--------------------------------------------------------------------------------------------------------------------
protected slots:

//--------------------------------------------------------------------------------------------------------------------
// private methods
//--------------------------------------------------------------------------------------------------------------------
private:
	  /*! virtual function that in the derived classes actually does the job of initializing
	   * the kernel from file
	   */
  	  virtual void onInit();

  	  //calculate the kernel matrix
  	  cv::Mat calculateKernel(double alpha);

	  /*!@brief virtual function to calculate the kernel matrix
	   */
  	  virtual void calculate();


  	  //!@brief A function that heuristically determines width of the kernel in pixels.
  	  unsigned int estimateWidth(unsigned int dim) const;


  	  cv::Mat calculateKernelManually();
//--------------------------------------------------------------------------------------------------------------------
// members
//--------------------------------------------------------------------------------------------------------------------
protected:

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

	  //!@brief angle of the kernel
	  cedar::aux::DoubleParameterPtr _mAlpha;

	  //! Whether or not the kernel is normalized.
	  cedar::aux::BoolParameterPtr _mNormalize;

private:
	  // none yet

}; // class cedar::aux::kernel::RotatedGauss

#endif // CEDAR_AUX_KERNEL_ROTATEDGAUSS_H
