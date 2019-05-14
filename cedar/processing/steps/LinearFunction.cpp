// LOCAL INCLUDES
#include "LinearFunction.h"
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
      new ElementDeclarationTemplate<cedar::proc::steps::LinearFunction>
      (
        "Algebra",
        "cedar.processing.steps.LinearFunction"
      )
    );
    declaration->setIconPath(":/steps/linear_dynamics.svg");
    declaration->setDescription
    (
      "Implements a linear function f(x)= slope * x + intercept. The "
      "intercept can be provided via the GUI parameter 'intercept' "
      "or optionally via "
      "the according input (which then overrides the GUI parameter). "
      "TODO: only acts on scalars at the moment, change to act element-wise."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::LinearFunction::LinearFunction()
:
  cedar::proc::Step(false),
  mResult(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
  mSlope(new cedar::aux::DoubleParameter(this,"slope", 1.0)),
  mIntercept(new cedar::aux::DoubleParameter(this, "intercept", 0.0))
{
  this->declareInput("input");
  this->declareInput("intercept (optional)", false); // optional

  this->declareOutput("result", mResult);

  QObject::connect(mIntercept.get(), SIGNAL(valueChanged()), this, SLOT(constantChanged()));
  QObject::connect(mSlope.get(), SIGNAL(valueChanged()), this, SLOT(constantChanged()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::LinearFunction::recompute()
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

  auto shiftinput = getInput("intercept (optional)");
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
    shift= mIntercept->getValue();
  }

  const float out = mSlope->getValue() * (x + shift); 

  mResult->getData().at<float>(0, 0) = out;

  checkOptionalInputs();
    // need to call this here, because determineinputValidity does not
    // get called for optional inputs that get removed by the user.
}

void cedar::proc::steps::LinearFunction::compute(const cedar::proc::Arguments&)
{
  recompute();
}

void cedar::proc::steps::LinearFunction::checkOptionalInputs()
{
  auto shiftinput = getInput("intercept (optional)");
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
    mIntercept->setConstant(true);
  }
  else
  {
    // Parameter aktivieren:
    mIntercept->setConstant(false);
  }
}

//// validity check
cedar::proc::DataSlot::VALIDITY cedar::proc::steps::LinearFunction::determineInputValidity
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
  else if ( slot->getName() == "intercept (optional)")
  {
    if (_input && _input->getDimensionality() == 0 && _input->getData().type() == CV_32F)
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }

  // else
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::LinearFunction::inputConnectionChanged(const std::string&)
{
  recompute();
}

void cedar::proc::steps::LinearFunction::constantChanged()
{
  recompute();
}


