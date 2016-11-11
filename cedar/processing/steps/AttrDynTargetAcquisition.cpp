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
  mpAlphaDir(new cedar::aux::DoubleParameter(this,"update rate alpha_dir", 0.2,  0, 1)),
  mpAlphaVel(new cedar::aux::DoubleParameter(this,"update rate alpha_vel", 0.2,  0, 1)),
  mpSDes(new cedar::aux::DoubleParameter(this,"desired target vel s_des", 0.1, 0.0, 10)),
  mpMaxInfluenceAngle(new cedar::aux::DoubleParameter(this,"maximal influence angle", 30., 0.0, 90.))
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

  const cedar::unit::Time estimate(0.02 * boost::units::si::seconds); // why?
  double delta_t_stabilization;
  try
  {
    delta_t_stabilization =  this->getRoundTimeAverage() / estimate;
  }
  catch ( ... )
  {
    delta_t_stabilization = 1.;
    std::cout <<  "caught an exception, no round time yet" << std::endl;
  }

  const double angle =  std::max(mpAngle->getData().at<double>(0, 0),  mpMaxInfluenceAngle->getValue() * (2 * M_PI / 360));
  const double f_dir = - mpAlphaDir->getValue() * sin(angle); //dynamics

  cv::Mat F_dir = mpOrthogonalInfluence->getData() * f_dir;
  if(isnan(F_dir.at<double>(0,0)) || isnan(F_dir.at<double>(1,0)) ||isnan(F_dir.at<double>(2,0)))
  {
    std::cout << "NaN@F_dir, AttrDynTargetAcquisition" << std::endl; //Todo: log some error
    F_dir = cv::Mat::zeros(3, 1, CV_64F);
  }

  mpRotationalAcceleration->setData(F_dir * delta_t_stabilization);

  cv::Mat currentVelocity = mpCurrentVelocity->getData().clone();
  double s = 0.;

  for( int i = 0; i < 3; ++i)
  {
    s += pow(currentVelocity.at<double>(i,0), 2);
  }

  s = sqrt(s);
  
  const double s_des = mpSDes->getValue();
  const double alpha_vel = mpAlphaVel->getValue();
  const double f_vel = - alpha_vel * ( s - s_des ); //dynamics
  cv::Mat F_vel = currentVelocity * (f_vel / s);

  if (isnan(F_vel.at<double>(0,0)) || isnan(F_vel.at<double>(1,0)) ||isnan(F_vel.at<double>(2,0)))
  {
    std::cout << "NaN@F_vel,AttrDynTargetAcquisition" << std::endl;
    F_vel = cv::Mat::zeros(3, 1, CV_64F);
  }

  mpForwardAcceleration->setData(F_vel * delta_t_stabilization);
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
    mpOrthogonalInfluence = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
  else if (inputName == "current velocity vector")
  {
    mpCurrentVelocity = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
}
