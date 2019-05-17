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

    File:        Boost.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 08 27

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/stepViews/BoostView.h"
#include "cedar/processing/sources/Boost.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/Arguments.h"
#include "cedar/auxiliaries/MatData.h"

// SYSTEM INCLUDES

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
      new ElementDeclarationTemplate<cedar::proc::sources::Boost, cedar::proc::gui::BoostView>
      (
        "Sources",
        "cedar.processing.sources.Boost"
      )
    );
    declaration->setIconPath(":/steps/boost.svg");
    declaration->setDescription("Generates a scalar that can be activated or deactivated. This can be input to, e.g., a field to 'boost' all entries to the field. You can control all your Boosts from a special dialog. You can also choose to deactivate the Boost when hitting the reset button of your architecture.");

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::sources::Boost::Boost()
:
mBoost(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
_mStrength(new cedar::aux::DoubleParameter(this, "strength", 1.0)),
_mActive(new cedar::aux::BoolParameter(this, "active", false)),
_mDeactivateOnReset(new cedar::aux::BoolParameter(this, "deactivate on reset", false))
{
  this->declareOutput("boost", this->mBoost);

  QObject::connect(_mStrength.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(_mActive.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::sources::Boost::reset()
{
  if (this->_mDeactivateOnReset->getValue())
  {
    // as the change in value will cause a recompute call, we have to unlock to prevent a deadlock ...
    this->unlock();

    this->_mActive->setValue(false, true);

    // ... then lock again because the outer loop will want something to unlock
    this->lock(cedar::aux::LOCK_TYPE_READ);
  }
}

void cedar::proc::sources::Boost::compute(const cedar::proc::Arguments&)
{
  if (this->isActive())
  {
    this->mBoost->getData().at<float>(0, 0) = this->getStrength();
  }
  else
  {
    this->mBoost->getData().at<float>(0, 0) = 0.0;
  }
}

void cedar::proc::sources::Boost::recompute()
{
  this->onTrigger();
}
