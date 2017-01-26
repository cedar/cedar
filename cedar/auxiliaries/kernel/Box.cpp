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

    File:        Box.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 12

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/kernel/Box.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES
#include <limits.h>

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declared
    = cedar::aux::kernel::FactoryManagerSingleton::getInstance()->registerType<cedar::aux::kernel::BoxPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::kernel::Box::Box()
:
cedar::aux::kernel::Separable(),
_mAmplitude(new cedar::aux::DoubleParameter
            (
              this,
              "amplitude",
              1.0,
              -std::numeric_limits<double>::max(),
              std::numeric_limits<double>::max()
            )
           ),
_mWidths
(
  new cedar::aux::UIntVectorParameter
  (
    this,
    "widths",
    getDimensionality(),
    5,
    1,
    std::numeric_limits<unsigned int>::max()
  )
)
/*
_mShifts
(
  new cedar::aux::IntVectorParameter
  (
    this,
    "shifts",
    getDimensionality(),
    0,
    std::numeric_limits<int>::min(),
    std::numeric_limits<int>::max()
  )
)*/
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
  QObject::connect(_mDimensionality.get(), SIGNAL(valueChanged()), this, SLOT(updateDimensionality()));
  QObject::connect(_mWidths.get(), SIGNAL(valueChanged()), this, SLOT(updateKernel()));
  QObject::connect(_mAmplitude.get(), SIGNAL(valueChanged()), this, SLOT(updateKernel()));

  this->calculate();
}


cedar::aux::kernel::Box::~Box()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::kernel::Box::calculateParts()
{
  mpReadWriteLockOutput->lockForWrite();

  CEDAR_DEBUG_ASSERT(this->getDimensionality() == this->_mWidths->size());

  for (unsigned int dim = 0; dim < this->getDimensionality(); ++dim)
  {
    this->setKernelPart(dim, cv::Mat::ones(static_cast<int>(this->_mWidths->at(dim)), 1, CV_32F));
  }

  if (this->getDimensionality() > 0)
  {
    double amplitude = this->getAmplitude();
    this->getKernelPart(0) *= amplitude;
  }

  mpReadWriteLockOutput->unlock();
}

void cedar::aux::kernel::Box::updateDimensionality()
{
  mpReadWriteLockOutput->lockForWrite();
  unsigned int new_dimensionality = this->getDimensionality();
  _mWidths->resize(new_dimensionality, _mWidths->getDefaultValue());
  mpReadWriteLockOutput->unlock();

  this->updateKernel();
}
