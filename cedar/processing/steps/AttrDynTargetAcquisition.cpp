// LOCAL INCLUDES
#include "AttrDynTargetAcquisition.h"
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
      new ElementDeclarationTemplate<cedar::proc::steps::AttrDynTargetAcquisition>
      (
        "Robotics",
        "cedar.processing.steps.AttrDynTargetAcquisition"
      )
    );
    declaration->setIconPath(":/steps/attr_dyn_target_acquisition.svg");
    declaration->setDescription
    (
      "Obtain dynamic changes to converge to attractor"
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::AttrDynTargetAcquisition::AttrDynTargetAcquisition()
  :
  cedar::proc::Step(true),
  mLastTimeStamp(0),
  mpForwardAcceleration(new cedar::aux::MatData(cv::Mat::zeros(3, 1, CV_64F))),
  mpRotationalAcceleration(new cedar::aux::MatData(cv::Mat::zeros(3, 1, CV_64F))),
  mpLambdaRot(new cedar::aux::DoubleParameter(this,"rotational acceleration prefactor", 0.2,  0, 1)),
  mpLambdaFwd(new cedar::aux::DoubleParameter(this,"forward acceleration prefactor", 0.2,  0, 1)),
  mpSDes(new cedar::aux::DoubleParameter(this,"desired forward acceleration", 0.1, 0.0, 10)),
  mpMaxAngle(new cedar::aux::DoubleParameter(this,"maximal angle", 30., 0.0, 90.))
{
  this->declareInput("current velocity vector");
  this->declareInput("angle");
  this->declareInput("orthogonal acceleration vector");

  this->declareOutput("forward acceleration", mpForwardAcceleration);
  this->declareOutput("rotational acceleration", mpRotationalAcceleration);

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::AttrDynTargetAcquisition::compute(const cedar::proc::Arguments&)
{
  const double angle =  std::max(mpAngle->getData().at<double>(0, 0),  mpMaxAngle->getValue() * (2 * M_PI / 360)); // we also perform this check in the component
  const double f_rot = -mpLambdaRot->getValue() * sin(angle); //rotational dynamics
  cv::Mat w_rot = mpOrthogonalAcceleration->getData() * f_rot;

  if(isnan(w_rot.at<double>(0,0)) || isnan(w_rot.at<double>(1,0)) ||isnan(w_rot.at<double>(2,0)))
  {
    std::cout << "Rotational acceleration is NaN, setting it to 0" << std::endl; //Todo: log some error
    w_rot = cv::Mat::zeros(3, 1, CV_64F);
  }

  mpRotationalAcceleration->setData(w_rot);

  const cv::Mat current_vel = mpCurrentVelocity->getData().clone();
  const double speed = cv::norm(current_vel);
  const double s_des = mpSDes->getValue();
  const double lambda_fwd = mpLambdaFwd->getValue();

  cv::Mat F_acc;

  if(speed <= std::numeric_limits<double>::epsilon())
  {
    F_acc = current_vel + 0.001; // make a tiny step into target direction
  }
  else
  {
    const double f_fwd = -lambda_fwd * (speed - s_des); //forward dynamics
    F_acc = (current_vel / speed) * f_fwd;
  }

  if (isnan(F_acc.at<double>(0,0)) || isnan(F_acc.at<double>(1,0)) ||isnan(F_acc.at<double>(2,0)))
  {
    std::cout << "Forward acceleration is NaN, setting it to 0" << std::endl;
    F_acc = cv::Mat::zeros(3, 1, CV_64F);
  }

  mpForwardAcceleration->setData(F_acc);
}

//// validity check
cedar::proc::DataSlot::VALIDITY cedar::proc::steps::AttrDynTargetAcquisition::determineInputValidity
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
  else if( slot->getName() == "current velocity vector")
  {
    if (_input && _input->getDimensionality() == 1 && cedar::aux::math::get1DMatrixSize(_input->getData()) >= 3 && _input->getData().type() == CV_64F)
      return cedar::proc::DataSlot::VALIDITY_VALID;
  }
  
  // else
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::AttrDynTargetAcquisition::inputConnectionChanged(const std::string& inputName){
  if (inputName == "angle")
  {
    mpAngle = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
  else if (inputName == "orthogonal acceleration vector")
  {
    mpOrthogonalAcceleration = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
  else if (inputName == "current velocity vector")
  {
    mpCurrentVelocity = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
}
