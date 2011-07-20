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

    File:        GaussInput.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 19

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "processing/source/GaussInput.h"
#include "dynamics/Activation.h"
#include "auxiliaries/NumericParameter.h"
#include "auxiliaries/math/functions.h"
#include "processing/Arguments.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>
#include <vector>
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::source::GaussInput::GaussInput(unsigned int sizeX, unsigned int sizeY)
:
cedar::proc::Step(),
mOutput(new cedar::dyn::MatActivation(cv::Mat::zeros(10,10,CV_32F))),
_mAmplitude(new cedar::aux::DoubleParameter("amplitude", 1.0, -10000.0, 10000.0)),
_mDimensionality(new cedar::aux::UIntParameter("dimensionality", 1, 1000)),
_mSigma(new cedar::aux::DoubleParameter("sigma", 3.0, 0.01, 1000.0)),
_mCenterX(new cedar::aux::DoubleParameter("center_x", sizeX/2.0, -10000.0, 10000.0)),
_mCenterY(new cedar::aux::DoubleParameter("center_y", sizeY/2.0, -10000.0, 10000.0))
{
  this->registerParameter(_mAmplitude);
  this->registerParameter(_mDimensionality);
  _mDimensionality->set(2);
  _mDimensionality->setConstant(true);
  this->registerParameter(_mSigma);
  this->registerParameter(_mCenterX);
  this->registerParameter(_mCenterY);
  this->declareOutput("Gauss input");
  this->setOutput("Gauss input", mOutput);
  QObject::connect(_mAmplitude.get(), SIGNAL(parameterChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mSigma.get(), SIGNAL(parameterChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mCenterX.get(), SIGNAL(parameterChanged()), this, SLOT(updateMatrix()));
  QObject::connect(_mCenterY.get(), SIGNAL(parameterChanged()), this, SLOT(updateMatrix()));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::source::GaussInput::compute(const cedar::proc::Arguments&)
{
  std::vector<cv::Mat> kernel_parts;
  kernel_parts.resize(_mDimensionality->get());

  for (unsigned dim = 0; dim < _mDimensionality->get(); dim++)
  {
    kernel_parts.at(dim) = cv::Mat(10, 1, CV_32F);
    for (int row = 0; row < kernel_parts.at(dim).rows; row++)
    {
      if (dim == 0)
      {
        kernel_parts.at(dim).at<float>(row, 1) = cedar::aux::math::gauss(static_cast<int>(row) - _mCenterX->get(), _mSigma->get());
      }
      else
      {
        kernel_parts.at(dim).at<float>(row, 1) = cedar::aux::math::gauss(static_cast<int>(row) - _mCenterY->get(), _mSigma->get());
      }
    }
  }
  kernel_parts.at(0) *= _mAmplitude->get();
  mOutput->lockForWrite();
  mOutput->getData() = kernel_parts.at(0) * kernel_parts.at(1).t();
  mOutput->unlock();
}

void cedar::proc::source::GaussInput::updateMatrix()
{
  this->compute(cedar::proc::Arguments());
}
