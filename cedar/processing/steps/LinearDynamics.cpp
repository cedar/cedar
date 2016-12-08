// LOCAL INCLUDES
#include "LinearDynamics.h"
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
      new ElementDeclarationTemplate<cedar::proc::steps::LinearDynamics>
      (
        "Robotics",
        "cedar.processing.steps.LinearDynamics"
      )
    );
    declaration->setIconPath(":/steps/linear_dynamics.svg");
    declaration->setDescription
    (
      "Linear dynamics"
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::LinearDynamics::LinearDynamics()
  :
  cedar::proc::Step(true),
  mpAcceleration(new cedar::aux::MatData(cv::Mat::zeros(3, 1, CV_64F))),
  mpLambda(new cedar::aux::DoubleParameter(this,"dynamics prefactor", 0.2,  0, 1)),
  mpSDes(new cedar::aux::DoubleParameter(this,"desired acceleration", 0.1, 0.0, 10))
{
  this->declareInput("velocity vector");
  this->declareOutput("acceleration vector", mpAcceleration);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::LinearDynamics::compute(const cedar::proc::Arguments&)
{
  const cv::Mat &velocity = mpVelocity->getData();
  const double &s_des = mpSDes->getValue();
  const double &lambda = mpLambda->getValue();

  const double s = cv::norm(velocity);

  cv::Mat acceleration;

  if(s <= std::numeric_limits<double>::epsilon())
  {
    acceleration = velocity + 2*std::numeric_limits<double>::epsilon(); // make a tiny step into target direction
  }
  else
  {
    const double f_fwd = -lambda * (s - s_des); //dynamics
    acceleration = (velocity / s) * f_fwd;
  }

  if (isnan(acceleration.at<double>(0,0)) || isnan(acceleration.at<double>(1,0)) ||isnan(acceleration.at<double>(2,0)))
  {
    std::cout << "LinearDynamics acceleration is NaN, setting it to 0" << std::endl;
    acceleration = cv::Mat::zeros(3, 1, CV_64F);
  }

  mpAcceleration->setData(acceleration);
}

//// validity check
cedar::proc::DataSlot::VALIDITY cedar::proc::steps::LinearDynamics::determineInputValidity
  (
   cedar::proc::ConstDataSlotPtr slot,
   cedar::aux::ConstDataPtr data
  ) const
{
  //all inputs have same type
  cedar::aux::ConstMatDataPtr _input = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
  if( slot->getName() == "velocity vector")
  {
    if (_input && _input->getDimensionality() == 1 && cedar::aux::math::get1DMatrixSize(_input->getData()) >= 3 && _input->getData().type() == CV_64F)
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }
  
  // else
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::LinearDynamics::inputConnectionChanged(const std::string& inputName)
{
  if (inputName == "velocity vector")
  {
    mpVelocity = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
}
