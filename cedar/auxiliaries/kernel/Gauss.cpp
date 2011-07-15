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

    File:        Gauss.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 07 07

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "auxiliaries/kernel/Gauss.h"
#include "auxiliaries/NumericParameter.h"
#include "processing/DataT.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>
#include <math.h>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::kernel::Gauss::Gauss()
:
cedar::aux::kernel::Separable(),
_mAmplitude(new cedar::aux::DoubleParameter("amplitude", -10000.0, 10000.0)),
_mLimit(new cedar::aux::DoubleParameter("limit", 0.0, 1.0))
{
  this->registerParameter(_mAmplitude);
  this->registerParameter(_mLimit);
}

cedar::aux::kernel::Gauss::Gauss(
                                  double amplitude,
                                  std::vector<double> sigmas,
                                  std::vector<double> shifts,
                                  double limit,
                                  unsigned int dimensionality,
                                  const std::string& kernelFile
                                )
:
cedar::aux::kernel::Separable(dimensionality, kernelFile),
_mAmplitude(new cedar::aux::DoubleParameter("amplitude", amplitude, -10000.0, 10000.0)),
_mLimit(new cedar::aux::DoubleParameter("limit", limit, 0.0, 1.0))
{
  this->registerParameter(_mAmplitude);
  this->registerParameter(_mLimit);
  this->mCenters.resize(dimensionality);
  this->mKernelParts.resize(dimensionality);
  this->mSizes.resize(dimensionality);
  this->_mSigmas = sigmas;
  this->_mShifts = shifts;
  this->calculate();
}

cedar::aux::kernel::Gauss::~Gauss()
{
#ifdef DEBUG
  std::cout << "> freeing data (Gauss)" << std::endl;
#endif
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::kernel::Gauss::onInit()
{
  calculate();
}

void cedar::aux::kernel::Gauss::calculate()
{
  mpReadWriteLockOutput->lockForWrite();
  const unsigned int& dimensionality = _mDimensionality->get();
  const double& amplitude = _mAmplitude->get();

  try
  {
    mKernelParts.resize(dimensionality);
    mCenters.resize(dimensionality);

    // calculate the kernel parts for every dimension
    for (unsigned int dim = 0; dim < dimensionality; dim++)
    {
      // estimate width
      if (_mSigmas.at(dim) != 0)
      {
        mSizes.at(dim) = getWidth(dim);
      }
      else
      {
        mSizes.at(dim) = 1;
      }
      mCenters.at(dim) = mSizes.at(dim) / 2 + _mShifts.at(dim);
      mKernelParts.at(dim) = cv::Mat::zeros(mSizes.at(dim), 1, CV_32FC1);

      // calculate kernel part
      if (_mSigmas.at(dim) != 0)
      {
        for (unsigned int j = 0; j < mSizes.at(dim); j++)
        {
          //!\todo move Gauss function and filling up of matrix to some tool function
          mKernelParts.at(dim).at<float>(j, 0)
              = exp(-std::pow(static_cast<int>(j) - mCenters.at(dim), 2) / (2 * std::pow(_mSigmas.at(dim), 2)));
        }
      }
      else // discrete case
      {
        mKernelParts.at(dim).at<float>(0, 0) = 1;
      }
      // normalize
      mKernelParts.at(dim) = mKernelParts.at(dim) * (1. / cv::sum(mKernelParts.at(dim)).val[0]);
      if (dim == 0)
      {
        mKernelParts.at(dim) = amplitude * mKernelParts.at(dim);
      }
    }
    // assemble the kernel
    int sizes[dimensionality];
    for (unsigned int i = 0; i < mSizes.size(); i++)
    {
      sizes[i] = mSizes.at(i);
    }
    mKernel->getData<cv::Mat>() = cv::Mat(static_cast<int>(dimensionality), sizes, CV_32F);
    cv::Mat& kernel = mKernel->getData<cv::Mat>();
    // now fill up the big kernel matrix
    int position[dimensionality];
    unsigned int max_index = 1;
    unsigned int current_index;
    for (unsigned int dim = 0; dim < dimensionality; dim++)
    {
      position[dim] = 0;
      max_index *= mSizes.at(dim);
    }
    std::cout << "max_index " << max_index << std::endl;
    for (unsigned int i = 0; i < max_index; i++)
    {
      current_index = i;
      float value = 1.0;
      for (unsigned int dim = 0; dim < dimensionality; dim++)
      {
        value *= mKernelParts.at(dim).at<float>(position[dim], 0);
      }
      kernel.at<float>(position) = value;
      // increment index
      position[0]++;
      for (unsigned int dim = 0; dim < dimensionality; dim++)
      {
        if (position[dim] >= static_cast<int>(mSizes.at(dim)))
        {
          position[dim] = 0;
          if (dim+1 < dimensionality)
          {
            position[dim+1]++;
          }
        }
      }
    }
    std::cout << "last i " << current_index << std::endl;
  }
  catch(std::out_of_range& error)
  {
    std::cerr << "> Error (kernel::Gauss) :" << error.what() << " in calculate().\n"
        << "  Check your configuration files." << std::endl;
    // set matrices to semi-meaningful sizes to prevent further crashes
    // todo
  }
  mpReadWriteLockOutput->unlock();
}

void cedar::aux::kernel::Gauss::setSigma(unsigned int dimension, double sigma)
{
  try
  {
    _mSigmas.at(dimension) = sigma;
  }
  catch(std::exception& e)
  {
    std::cout << "Error in " << this->_mName->get()
              << " (cedar::aux::kernel::Gauss::setSigma): vector out of bounds " << std::endl;
  }
  calculate();
}

double cedar::aux::kernel::Gauss::getSigma(unsigned int dimension) const
{
  return _mSigmas.at(dimension);
}

void cedar::aux::kernel::Gauss::setShift(unsigned int dimension, double shift)
{
  try
  {
    _mShifts.at(dimension) = shift;
  }
  catch(std::exception& e)
  {
    std::cout << "Error in " << this->_mName->get()
              << " (cedar::aux::kernel::Gauss::setShift): vector out of bounds " << std::endl;
  }
  calculate();
}

double cedar::aux::kernel::Gauss::getShift(unsigned int dimension) const
{
  return _mShifts.at(dimension);
}

void cedar::aux::kernel::Gauss::setAmplitude(double amplitude)
{
  _mAmplitude->set(amplitude);
  calculate();
}

double cedar::aux::kernel::Gauss::getAmplitude() const
{
  return _mAmplitude->get();
}

unsigned int cedar::aux::kernel::Gauss::getWidth(unsigned int dim) const
{
  unsigned int tmp;
  /* analytical solution for calculating the width of the sampled Gausssian, given a precision _mLimit
   * simply solve the inverted Gaussian equation
   */
  if ((_mSigmas.at(dim) < 10000) && (_mSigmas.at(dim) > 0))
  {
    tmp = 2 * round(sqrt(2 * powf(_mSigmas.at(dim), 2.0) * log(fabs(_mAmplitude->get()) / _mLimit->get()))) + 1;
  }
  else
  {
    tmp = 1;
  }
  return tmp;
}

