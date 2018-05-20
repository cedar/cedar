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

    File:        BaseSignal.cpp

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2017 12 04

    Description: Source file for the class cedar::proc::sources::BaseSignal.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/sources/BaseSignal.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/IsMatrix.h"
#include <cedar/processing/ElementDeclaration.h>
#include "cedar/auxiliaries/GlobalClock.h"
#include "cedar/units/Time.h"

// SYSTEM INCLUDES
#include <cmath>

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------

// dont register, this is a base class


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::sources::BaseSignal::BaseSignal()
:
Step(true), // looped
// outputs
mOutput(new cedar::aux::MatData(cv::Mat())),
mPeriod(new cedar::aux::DoubleParameter(this, "period", 1.0)),
mAmplitude(new cedar::aux::DoubleParameter(this, "amplitude", 1.0)),
mPhaseShift(new cedar::aux::DoubleParameter(this, "phase shift", 0.0)),
mVerticalOffset(new cedar::aux::DoubleParameter(this, "vertical offset", 0.0))
{
  this->declareOutput("signal", mOutput);

  mOutput->setData( cv::Mat::zeros(1, 1, CV_32F) );
}

cedar::proc::sources::BaseSignal::~BaseSignal()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::proc::sources::BaseSignal::compute(const cedar::proc::Arguments&)
{
  this->recompute();
}

void cedar::proc::sources::BaseSignal::recompute()
{
  double value;
  double t;

  auto currentTime= cedar::aux::GlobalClockSingleton::getInstance()->getTime()
                    - mStartTime;

  t= currentTime / cedar::unit::seconds; 

  value= calculateSignal( t, 
                          mPeriod->getValue(),
                          mAmplitude->getValue(),
                          mPhaseShift->getValue(),
                          mVerticalOffset->getValue() );

  mOutput->getData().at<float>(0,0)= value;
}

void cedar::proc::sources::BaseSignal::startClock()
{
  mStartTime= cedar::aux::GlobalClockSingleton::getInstance()->getTime();
}

void cedar::proc::sources::BaseSignal::reset()
{
  startClock();
}

void cedar::proc::sources::BaseSignal::onStart()
{
  startClock();
}

