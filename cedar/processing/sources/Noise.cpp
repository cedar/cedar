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

    File:        Noise.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 11 10

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/sources/Noise.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <vector>


//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration
    (
      new cedar::proc::ElementDeclarationTemplate<cedar::proc::sources::Noise>
          (
            "Sources", "cedar.processing.sources.Noise"
          )
    );
    declaration->setIconPath(":/steps/noise.svg");
    declaration->setDescription("A step that generates normally distributed random noise.");
    declaration->deprecatedName("cedar.dynamics.Noise");
    declaration->deprecate("This source offers little options for noise generation. In addition, it is misleading "
        "when integrating via Euler.");

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::sources::Noise::Noise()
:
cedar::proc::Step(true),
mRandomMatrix(new cedar::aux::MatData(cv::Mat::zeros(50,50,CV_32F))),
_mDimensionality(new cedar::aux::UIntParameter(this, "dimensionality", 2, 0, 4)),
_mSizes(new cedar::aux::UIntVectorParameter(this, "sizes", 2, 50, 1, 1000)),
_mMean(new cedar::aux::DoubleParameter(this, "mean", 0.0, -1000, 1000)),
_mStandardDeviation(new cedar::aux::DoubleParameter(this, "standard deviation", 1.0, 0.0, 1000.0))
{
  _mDimensionality->setValue(2);
  _mSizes->makeDefault();
  QObject::connect(_mSizes.get(), SIGNAL(valueChanged()), this, SLOT(dimensionSizeChanged()));
  QObject::connect(_mDimensionality.get(), SIGNAL(valueChanged()), this, SLOT(dimensionalityChanged()));
  this->declareOutput("random", mRandomMatrix);

  // now check the dimensionality and sizes of all matrices
  this->updateMatrices();
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::sources::Noise::compute(const cedar::proc::Arguments&)
{
  cv::Mat& random = this->mRandomMatrix->getData();
  cv::randn(random, cv::Scalar(_mMean->getValue()), cv::Scalar(_mStandardDeviation->getValue()));
}

void cedar::proc::sources::Noise::dimensionalityChanged()
{
  this->_mSizes->resize(_mDimensionality->getValue(), _mSizes->getDefaultValue());
  this->updateMatrices();
}

void cedar::proc::sources::Noise::dimensionSizeChanged()
{
  this->updateMatrices();
}

void cedar::proc::sources::Noise::updateMatrices()
{
  int dimensionality = static_cast<int>(_mDimensionality->getValue());

  std::vector<int> sizes(dimensionality);
  for (int dim = 0; dim < dimensionality; ++dim)
  {
    sizes[dim] = _mSizes->at(dim);
  }
  this->lockAll();
  if (dimensionality == 0)
  {
    this->mRandomMatrix->getData() = cv::Mat(1, 1, CV_32F, cv::Scalar(0));
  }
  else if (dimensionality == 1)
  {
    this->mRandomMatrix->getData() = cv::Mat(sizes[0], 1, CV_32F, cv::Scalar(0));
  }
  else
  {
    this->mRandomMatrix->getData() = cv::Mat(dimensionality,&sizes.at(0), CV_32F, cv::Scalar(0));
  }
  this->unlockAll();
  this->emitOutputPropertiesChangedSignal("random");
}
