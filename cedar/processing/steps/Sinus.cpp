/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        Sinus.h

    Maintainer:  Nico Kuerschner
    Email:       nico.kuerschner@ini.rub.de
    Date:        2016 08 12

    Description: Implements a sinus function

    Credits:

======================================================================================================================*/
// LOCAL INCLUDES
#include "Sinus.h"
#include "cedar/processing/ElementDeclaration.h"
#include <math.h>
#include <iostream>

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
      new ElementDeclarationTemplate<cedar::proc::steps::Sinus>
      (
        "Algebra",
        "cedar.processing.steps.Sinus"
      )
    );
    declaration->setIconPath(":/steps/sinus_dynamics.svg");
    declaration->setDescription
    (
      "Implements the sine function f(x)= amplitude * sin( x + shift ). The Phase shift can either be set via the GUI parameter 'phase shift' or optionally via the according input (which then overrides the GUI parameter).\n"
      "You can switch the sine to a cosine by a GUI parameter.\n"
      "Note that if you want to use the Sinus step to set an attractor you "
      "will need to give the negative of your desired attractor value "
      "as the 'phase shift'. "
      "TODO: sin is only for one scalar, make element-wise for whole tensor."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::Sinus::Sinus()
  :
  cedar::proc::Step(false),
  mResult(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
  mAmplitude(new cedar::aux::DoubleParameter(this,"amplitude", 1.0)),
  mShift(new cedar::aux::DoubleParameter(this, "phase shift", 0.0)),
  mMakeCos(new cedar::aux::BoolParameter(this, "make cos", false))
{
  this->declareInput("input");
  this->declareInput("phase shift (optional)", false); // optional

  this->declareOutput("result", mResult);

  QObject::connect(mShift.get(), SIGNAL(valueChanged()), this, SLOT(constantChanged()));
  QObject::connect(mAmplitude.get(), SIGNAL(valueChanged()), this, SLOT(constantChanged()));
  QObject::connect(mMakeCos.get(), SIGNAL(valueChanged()), this, SLOT(constantChanged()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Sinus::recompute()
{
  auto input = getInput("input");

  if (!input)
    return;

  auto data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(input);

  if (!data)
    return;

  const float &x= data->getData().at<float>(0, 0);

  float shift;
  bool  has_shift_input;

  auto shiftinput = getInput("phase shift (optional)");
  if (shiftinput)
  {
    auto shiftdata = boost::dynamic_pointer_cast<const cedar::aux::MatData>(shiftinput);
    if (shiftdata
        && !shiftdata->getData().empty())
    {
      has_shift_input= true;
      shift= shiftdata->getData().at<float>(0, 0);
    }
  }

  if (!has_shift_input)
  {
    shift= mShift->getValue();
  }

  float out;

  if (mMakeCos->getValue())
  {
    out = mAmplitude->getValue() * cos(x + shift); 
  }
  else
  {
    out = mAmplitude->getValue() * sin(x + shift); 
  }

  mResult->getData().at<float>(0, 0) = out;

  checkOptionalInputs();
    // need to call this here, because determineinputValidity does not
    // get called for optional inputs that get removed by the user.
}

void cedar::proc::steps::Sinus::compute(const cedar::proc::Arguments&)
{
  recompute();
}

void cedar::proc::steps::Sinus::checkOptionalInputs()
{
  auto shiftinput = getInput("phase shift (optional)");
  bool has_shift_input= false;

  if (shiftinput)
  {
    auto shiftdata = boost::dynamic_pointer_cast<const cedar::aux::MatData>(shiftinput);
    if (shiftdata
        && !shiftdata->getData().empty())
    {
      has_shift_input= true;
    }
  }

  if (has_shift_input)
  {
    // ausgrauen des Parameters:
    mShift->setConstant(true);
  }
  else
  {
    // Parameter aktivieren:
    mShift->setConstant(false);
  }
}
//// validity check
cedar::proc::DataSlot::VALIDITY cedar::proc::steps::Sinus::determineInputValidity
  (
   cedar::proc::ConstDataSlotPtr slot,
   cedar::aux::ConstDataPtr data
  ) const
{
  //all inputs have same type
  cedar::aux::ConstMatDataPtr _input = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
  if( slot->getName() == "input" )
  {
    if (_input && _input->getDimensionality() == 0 && _input->getData().type() == CV_32F)
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }  
  else if ( slot->getName() == "phase shift (optional)")
  {
    if (_input && _input->getDimensionality() == 0 && _input->getData().type() == CV_32F)
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }

  // else
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::Sinus::inputConnectionChanged(const std::string&)
{
  recompute();
}

void cedar::proc::steps::Sinus::constantChanged()
{
  recompute();
}

