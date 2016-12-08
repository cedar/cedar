// LOCAL INCLUDES
#include "SinusDynamics.h"
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
      new ElementDeclarationTemplate<cedar::proc::steps::SinusDynamics>
      (
        "Robotics",
        "cedar.processing.steps.SinusDynamics"
      )
    );
    declaration->setIconPath(":/steps/sinus_dynamics.svg");
    declaration->setDescription
    (
      "Sinus dynamics"
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::SinusDynamics::SinusDynamics()
  :
  cedar::proc::Step(true),
  mpAngleChange(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_64F))),
  mpLambda(new cedar::aux::DoubleParameter(this,"lambda", 0.2,  0, 1)),
  mpMaxAngle(new cedar::aux::DoubleParameter(this,"maximal angle", 30., 0.0, 90.))
{
  this->declareInput("angle");
  this->declareOutput("angle change", mpAngleChange);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::SinusDynamics::compute(const cedar::proc::Arguments&)
{
  const double &phi =  std::max(mpAngle->getData().at<double>(0, 0),  mpMaxAngle->getValue() * (2 * M_PI / 360)); // we also perform this check in the component
  const double phi_dot = -mpLambda->getValue() * sin(phi); //sinus dynamics

  mpAngleChange->getData().at<double>(0, 0) = phi_dot;
}

//// validity check
cedar::proc::DataSlot::VALIDITY cedar::proc::steps::SinusDynamics::determineInputValidity
  (
   cedar::proc::ConstDataSlotPtr slot,
   cedar::aux::ConstDataPtr data
  ) const
{
  //all inputs have same type
  cedar::aux::ConstMatDataPtr _input = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
  if( slot->getName() == "angle" )
  {
    if (_input && _input->getDimensionality() == 0 && _input->getData().type() == CV_64F)
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }  
  // else
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::SinusDynamics::inputConnectionChanged(const std::string& inputName)
{
  if (inputName == "angle")
  {
    mpAngle = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
}
