// LOCAL INCLUDES
#include "VectorsPlaneAngle.h"
#include "cedar/processing/ElementDeclaration.h"
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
      new ElementDeclarationTemplate<cedar::proc::steps::VectorsPlaneAngle>
      (
        "Robotics",
        "cedar.processing.steps.VectorsPlaneAngle"
      )
    );
    declaration->setIconPath(":/steps/vectors_plane_angle.svg");
    declaration->setDescription
    (
      "Obtain angle, plane normal and orthogonal influence from given reference and influence"
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::VectorsPlaneAngle::VectorsPlaneAngle()
  :
  mpAngle(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_64F))),
  mpOrthogonalAcceleration(new cedar::aux::MatData(cv::Mat::zeros(3, 1, CV_64F)))
{
  this->declareInput("endeffector velocity");
  this->declareInput("endeffector target position difference");

  this->declareOutput("angle", mpAngle);
  this->declareOutput("orthogonal acceleration", mpOrthogonalAcceleration);

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::VectorsPlaneAngle::compute(const cedar::proc::Arguments&)
{
  if(!mpEndeffectorVelocity || !mpDifferenceVector)
  {
    // TODO: log some error
    return;
  }

  cv::Mat current_vel = mpEndeffectorVelocity->getData().clone();
  const cv::Mat &current_pos_diff = mpDifferenceVector->getData();

  // we have a problem if v = (0, 0, 0) ... there goes a quick fix, that bothers me a lot:
  if(current_vel.at<double>(0) == 0 && current_vel.at<double>(1) == 0 && current_vel.at<double>(2) == 0)
  {
    current_vel = current_pos_diff.cross(current_pos_diff); // make something orthogonal
  }

  // calculate angle as phi = acos(v.k / |v||k|)
  const double dot = current_vel.dot(current_pos_diff);
  const double norm = cv::norm(current_vel) * cv::norm(current_pos_diff);
  double angle = 0;

  if(norm != 0)
  {
    angle = acos(dot / norm);
  }    

  mpAngle->getData().at<double>(0,0) = angle;  

  // direction of most effective change w_dir = v x ( v x k ) * ( |v| / |v x (v x k)| )
  cv::Mat w_dir = current_vel.cross(current_vel.cross( current_pos_diff ));
  const double norm_inf_dir = cv::norm(w_dir);

  if(norm_inf_dir != 0)
  {
    w_dir /= norm_inf_dir;
  }

  w_dir *= cv::norm(current_vel); //can change the influence dir vector to 0, if reference was 0
  mpOrthogonalAcceleration->setData(w_dir);
}

//// validity check
cedar::proc::DataSlot::VALIDITY cedar::proc::steps::VectorsPlaneAngle::determineInputValidity
  (
   cedar::proc::ConstDataSlotPtr slot,
   cedar::aux::ConstDataPtr data
  ) const
{
  //all inputs have same type
  cedar::aux::ConstMatDataPtr _input = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
  if( slot->getName() == "endeffector velocity" || slot->getName() == "endeffector target position difference")
  {
    if (_input && _input->getDimensionality() == 1 && cedar::aux::math::get1DMatrixSize(_input->getData()) == 3 && _input->getData().type() == CV_64F)
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }
  
  // else
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::VectorsPlaneAngle::inputConnectionChanged(const std::string& inputName)
{
  if (inputName == "endeffector velocity")
  {
    mpEndeffectorVelocity = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
  else if (inputName == "endeffector target position difference")
  {
    mpDifferenceVector = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
}

