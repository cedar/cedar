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

    File:        SinusSignal.cpp

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2017 12 04

    Description: Source file for the class cedar::proc::sources::SinusSignal.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/sources/SinusSignal.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/IsMatrix.h"
#include <cedar/processing/ElementDeclaration.h>
#include "cedar/auxiliaries/GlobalClock.h"
#include "cedar/units/Time.h"

// SYSTEM INCLUDES
#define _USE_MATH_DEFINES
#include <math.h>

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
    new ElementDeclarationTemplate<cedar::proc::sources::SinusSignal>
    (
      "Sources",
      "cedar.processing.sources.SinusSignal"
    )
  );

  declaration->setIconPath(":/steps/sinus_signal.svg");
  declaration->setDescription
  (
    "Generates a sine signal in time. The Global clock serves as timing "
    "signal. You can switch the sine to a cosine by a GUI parameter."
  );

  declaration->declare();

  return true;
}

bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::sources::SinusSignal::SinusSignal()
:
mMakeCos(new cedar::aux::BoolParameter(this, "make cos", false))
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

double cedar::proc::sources::SinusSignal::calculateSignal(double t, double period, double amplitude, double phase, double verticaloffset)
{
  double value;

  // can be done via phase shift, but this is more user friendly:
  if (mMakeCos->getValue())
    value= cos( ( t + phase ) / period * 2.0 * M_PI );
  else
    value= sin( ( t + phase ) / period * 2.0 * M_PI );

  return amplitude * value + verticaloffset;
}

