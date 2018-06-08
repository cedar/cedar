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

    File:        RotatedGauss.cpp

    Maintainer:  Alexander Gluchow
    Email:       
    Date:        2016 05 19

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/kernel/RotatedGauss.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/Log.h"

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declared
    = cedar::aux::kernel::FactoryManagerSingleton::getInstance()->registerType<cedar::aux::kernel::RotatedGaussPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::kernel::RotatedGauss::RotatedGauss():
_mAmplitude(new cedar::aux::DoubleParameter(this, "amplitude", 1.0, -10000.0, 10000.0)),
_mSigmas(new cedar::aux::DoubleVectorParameter(this, "sigmas", {5,1}, 0.01, 10000)),
_mAlpha(new cedar::aux::DoubleParameter(this, "alpha", 0.0, -90.0, 90.0)),
_mNormalize(new cedar::aux::BoolParameter(this, "normalize", true))
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
  this->onInit();
}


cedar::aux::kernel::RotatedGauss::~RotatedGauss()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::kernel::RotatedGauss::onInit()
{
	QObject::connect(_mAmplitude.get(), SIGNAL(valueChanged()), this, SLOT(updateKernel()), Qt::DirectConnection);
	QObject::connect(_mAlpha.get(), SIGNAL(valueChanged()), this, SLOT(updateKernel()), Qt::DirectConnection);
	QObject::connect(_mSigmas.get(), SIGNAL(valueChanged()), this, SLOT(updateKernel()), Qt::DirectConnection);
	QObject::connect(_mNormalize.get(), SIGNAL(valueChanged()), this, SLOT(updateKernel()), Qt::DirectConnection);
    this->calculate();
}



cv::Mat cedar::aux::kernel::RotatedGauss::calculateKernel(double alpha)
{
	double amplitude = _mAmplitude->getValue();

	double sigmaY = _mSigmas->at(0); // y
	double sigmaX = _mSigmas->at(1); // x

	double alphaX = alpha / 180 * cedar::aux::math::pi;
	double alphaY = (alpha + 90) / 180 * cedar::aux::math::pi;

	double a = cos(alphaY);
	double b = sin(alphaY);
	double c = cos(alphaX);
	double d = sin(alphaX);

	unsigned int row = 101;
	unsigned int col = 101;
	cv::Mat kernel = cv::Mat::zeros(row, col, CV_32F);
	int x, y;

	for (unsigned int i = 0 ; i < row ; i++)
	{
		for (unsigned int j = 0 ; j < col ; j++)
	    	{
	    		x = i - floor(col / 2);
	    		y = j - floor(row / 2);
	    		kernel.at<float>(i,j) = exp( - pow((a * x + b * y), 2.0) / (2.0 * pow(sigmaY, 2.0)) - pow((c * x + d * y), 2.0) / (2.0 * pow(sigmaX, 2.0)));
	    	}
	    }

	if (_mNormalize->getValue())
		kernel /= cv::sum(kernel).val[0];

	kernel *= amplitude;

	return kernel;
}

void cedar::aux::kernel::RotatedGauss::calculate()
{
	this->mKernel->lockForWrite();
	this->mKernel->setData(calculateKernel(_mAlpha->getValue()));
	this->mKernel->unlock();
}




double cedar::aux::kernel::RotatedGauss::getAlpha()
{
	return _mAlpha->getValue();
}


void cedar::aux::kernel::RotatedGauss::setAlpha(double alpha)
{
	_mAlpha->setValue(alpha);
}


double cedar::aux::kernel::RotatedGauss::getSigma(int dim)
{
	return _mSigmas->at(dim);
}


void cedar::aux::kernel::RotatedGauss::setSigma(unsigned int dim, int sigma)
{
	if (dim < _mSigmas->size())
	  {
	    _mSigmas->setValue(dim, sigma);
	  }
	else
	  {
	    CEDAR_THROW(cedar::aux::IndexOutOfRangeException, "Error in RotatedGauss::setSigma: vector out of bounds");
	  }
}


double cedar::aux::kernel::RotatedGauss::getAmplitude()
{
	return _mAmplitude->getValue();
}


void cedar::aux::kernel::RotatedGauss::setAmplitude(double amplitude)
{
	_mAmplitude->setValue(amplitude);
}


bool cedar::aux::kernel::RotatedGauss::getNormalize()
{
	return _mNormalize->getValue();
}


void cedar::aux::kernel::RotatedGauss::setNormalize(bool normalize)
{
	_mNormalize->setValue(normalize);
}





//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//unused functions:
unsigned int cedar::aux::kernel::RotatedGauss::estimateWidth(unsigned int dim)
{
	unsigned int tmp; //limit = 5;
	  /* size of kernel is determined by limit * sigma
	   */
	  tmp = static_cast<unsigned int>(ceil(2 * pow(_mSigmas->at(dim),2)));
	  // check if kernel size is even and if so, make it odd
	  if (tmp % 2 == 0)
	  {
		tmp++;
	  }
	  return tmp;
}


cv::Mat cedar::aux::kernel::RotatedGauss::calculateKernelManually()
{
	cv::Mat kernel = (cv::Mat_<float>(13,13) << 	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
													0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
													0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
													0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
													0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
													0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0,
													1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
													0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0,
													0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
													0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
													0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
													0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
													0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return kernel;
}

