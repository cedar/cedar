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
  mpRotationalAcceleration(new cedar::aux::MatData(cv::Mat::zeros(3, 1, CV_64F))),
  mpLambda(new cedar::aux::DoubleParameter(this,"rotational acceleration prefactor", 0.2,  0, 1)),
  mpMaxAngle(new cedar::aux::DoubleParameter(this,"maximal angle", 30., 0.0, 90.))
{
  this->declareInput("angle");
  this->declareInput("orthogonal acceleration vector");

  this->declareOutput("rotational acceleration", mpRotationalAcceleration);

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::SinusDynamics::compute(const cedar::proc::Arguments&)
{
  const double &angle =  std::max(mpAngle->getData().at<double>(0, 0),  mpMaxAngle->getValue() * (2 * M_PI / 360)); // we also perform this check in the component
  const double &f_rot = -mpLambda->getValue() * sin(angle); //rotational dynamics
  cv::Mat w_rot = mpOrthogonalAcceleration->getData() * f_rot;

  if(isnan(w_rot.at<double>(0,0)) || isnan(w_rot.at<double>(1,0)) ||isnan(w_rot.at<double>(2,0)))
  {
    std::cout << "SinusDynamics acceleration is NaN, setting it to 0" << std::endl; //Todo: log some error
    w_rot = cv::Mat::zeros(3, 1, CV_64F);
  }

  mpRotationalAcceleration->setData(w_rot);
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
      return cedar::proc::DataSlot::VALIDITY_VALID;
  }
  else if( slot->getName() == "orthogonal acceleration vector" )
  {
    if (_input && _input->getDimensionality() == 1 && cedar::aux::math::get1DMatrixSize(_input->getData()) == 3 && _input->getData().type() == CV_64F)
      return cedar::proc::DataSlot::VALIDITY_VALID;
  }
  
  // else
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::SinusDynamics::inputConnectionChanged(const std::string& inputName){
  if (inputName == "angle")
  {
    mpAngle = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
  else if (inputName == "orthogonal acceleration vector")
  {
    mpOrthogonalAcceleration = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
}
