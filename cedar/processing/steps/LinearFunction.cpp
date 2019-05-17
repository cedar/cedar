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
      "Implements a linear function f(x)= slope * (x + horizontal shift). "
      "The "
      "'horizontal shift' can be provided via the GUI parameter "
      "'horizontal shift' "
      "or optionally via "
      "the according input (which then overrides the GUI parameter). "
      "Note that the 'horizontal shift' gets added to the input and then "
      "both get multiplied by the factor "
      "'slope', so if you want to set an attractor, the slope has to be "
      "negative and the 'horizontal shift' the negative of the "
      "desired attractor."
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
  mHorizontalShift(new cedar::aux::DoubleParameter(this, "horizontal shift", 0.0))
{
  this->declareInput("input");
  this->declareInput("horizontal shift (optional)", false); // optional

  this->declareOutput("result", mResult);

  QObject::connect(mHorizontalShift.get(), SIGNAL(valueChanged()), this, SLOT(constantChanged()));
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

  auto x= data->getData().clone();

  float shift;
  bool  has_shift_input;

  auto shiftinput = getInput("horizontal shift (optional)");
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
    shift= mHorizontalShift->getValue();
  }

  auto out = mSlope->getValue() * (x + shift); 

  mResult->setData( out );

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
  auto shiftinput = getInput("horizontal shift (optional)");
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
    mHorizontalShift->setConstant(true);
  }
  else
  {
    // Parameter aktivieren:
    mHorizontalShift->setConstant(false);
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
    if (_input 
       // && _input->getDimensionality() == 0 
        && _input->getData().type() == CV_32F)
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }  
  else if ( slot->getName() == "horizontal shift (optional)")
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


