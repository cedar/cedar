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

    File:        SpatialTemplate.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2013 10 30

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include <cedar/processing/sources/SpatialTemplate.h>
#include <cedar/processing/ElementDeclaration.h>
#include <cedar/processing/DeclarationRegistry.h>
#include <cedar/auxiliaries/assert.h>
#include <cedar/auxiliaries/exceptions.h>
#include <cedar/auxiliaries/DataTemplate.h>
#include <cedar/auxiliaries/MatData.h>
#include <cedar/auxiliaries/BoolParameter.h>
#include <cedar/auxiliaries/UIntParameter.h>
#include <cedar/auxiliaries/DoubleParameter.h>
#include <cedar/auxiliaries/ObjectParameterTemplate.h>
#include <cedar/auxiliaries/math/TransferFunction.h>
#include <cedar/auxiliaries/math/transferFunctions/AbsSigmoid.h>
#include <cedar/auxiliaries/math/tools.h>
#include <cedar/auxiliaries/math/constants.h>

// SYSTEM INCLUDES
#include <cmath>

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
      new ElementDeclarationTemplate<cedar::proc::sources::SpatialTemplate>
      (
        "Sources",
        "cedar.processing.sources.SpatialTemplate"
      )
    );
    declaration->setIconPath(":/steps/spatial_template.svg");
    declaration->setDescription("Generates a matrix with a "
      "Gaussian-shaped cone region. Indices closer to the center of the cone "
      "will have higher values. "
      "The cone can be rotated and stretched by the parameters."
      "Use this to generated a template for a spatial relation (e.g., to the left of). TODO: The parameters need some more explanation.");

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::sources::SpatialTemplate::SpatialTemplate()
:
mPattern(new cedar::aux::MatData(cv::Mat::zeros(50, 50, CV_32F))),
_mSizeX(new cedar::aux::UIntParameter(this, "size x", 50, cedar::aux::UIntParameter::LimitType::positive(1000))),
_mSizeY(new cedar::aux::UIntParameter(this, "size y", 50, cedar::aux::UIntParameter::LimitType::positive(1000))),
_mMuTh(new cedar::aux::DoubleParameter(this, "mu th", 0.0, -cedar::aux::math::pi, cedar::aux::math::pi)),
_mSigmaTh(new cedar::aux::DoubleParameter(this, "sigma th hor", 0.25, cedar::aux::DoubleParameter::LimitType::positiveZero(2.0))),
_mMuR(new cedar::aux::DoubleParameter(this, "mu r", 15.0, cedar::aux::DoubleParameter::LimitType::positiveZero(1000.0))),
_mSigmaR(new cedar::aux::DoubleParameter(this, "sigma r", 100.0, cedar::aux::DoubleParameter::LimitType::positiveZero(1000.0)))
{
  // output
  this->declareOutput("spatial pattern", mPattern);

  QObject::connect(_mSizeX.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(_mSizeY.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(_mMuTh.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(_mSigmaTh.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(_mMuR.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(_mSigmaR.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));

  this->recompute();
}

cedar::proc::sources::SpatialTemplate::~SpatialTemplate()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::sources::SpatialTemplate::recompute()
{
  unsigned int size_x = _mSizeX->getValue();
  unsigned int size_y = _mSizeY->getValue();
  double mu_th = _mMuTh->getValue();
  double sigma_th = _mSigmaTh->getValue();
  double mu_r = _mMuR->getValue();
  double sigma_r = _mSigmaR->getValue();

  mPattern->getData() = cv::Mat(size_x, size_y, CV_32F);

  // go through all positions of the pattern
  for (unsigned int i = 0; i < size_x; ++i)
  {
    for (unsigned int j = 0; j < size_y; ++j)
    {
      // shift the indices so that the pattern is centered in the output matrix
      double x = i - ((size_x - 1) / 2.0);
      double y = j - ((size_y - 1) / 2.0);

      // log polar transformation
      // http://docs.opencv.org/modules/imgproc/doc/geometric_transformations.html#logpolar
      double th = atan2(y, x);

      // modulo(th - (mu_th + pi), 2*pi)
      double a = th - mu_th - cedar::aux::math::pi;
      double b = 2 * cedar::aux::math::pi;
      double modulo = a-b*floor(a/b);

      th = mu_th - cedar::aux::math::pi + modulo;

      double r = log(sqrt(pow(x, 2.0) + pow(y, 2.0)));

      double gaussian = exp(
                           -0.5 * pow(th - mu_th, 2.0) / pow(sigma_th, 2.0)
                           -0.5 * pow(r - mu_r, 2.0) / pow(sigma_r, 2.0)
                           );

      if (cedar::aux::math::isZero(x) && cedar::aux::math::isZero(y))
      {
        gaussian = 0.0;
      }

      // generate the pattern as a weighted sum of the gaussian and the sigmoid
      mPattern->getData().at<float>(i, j)
        = static_cast<float>(gaussian);
    }
  }

  // this triggers all connected steps.
  this->onTrigger();
}

void cedar::proc::sources::SpatialTemplate::compute(const cedar::proc::Arguments&)
{
}
