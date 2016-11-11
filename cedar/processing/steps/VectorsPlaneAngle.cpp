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
  mpPlaneNormal(new cedar::aux::MatData(cv::Mat::zeros(3, 1, CV_64F))),
  mpOrthogonalAcceleration(new cedar::aux::MatData(cv::Mat::zeros(3, 1, CV_64F)))
{
  this->declareInput("endeffector position vector");
  this->declareInput("difference vector to target location");

  this->declareOutput("angle", mpAngle);
  this->declareOutput("plane normal", mpPlaneNormal);
  this->declareOutput("orthogonal acceleration", mpOrthogonalAcceleration);

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::VectorsPlaneAngle::compute(const cedar::proc::Arguments&)
{
  if(!mpCurrentPositionVector || !mpTargetDifferenceVector)
  {
    // TODO: log some error
    return;
  }

  cv::Mat current_pos = mpCurrentPositionVector->getData().clone();
  cv::Mat diff_current_target = mpTargetDifferenceVector->getData().clone();

  // calculate angle as phi=(v x (v x k))/(|v||k|)
  const double dot = current_pos.dot(diff_current_target);
  const double norm = cv::norm(current_pos) * cv::norm(diff_current_target);
  double angle;

  if(norm == 0)
  {
    angle = 0;
  }  
  else
  {
    angle = acos(dot / norm);
  }

  mpAngle->getData().at<double>(0,0) = angle;  

  //plane normal
  cv::Mat planeNormal = current_pos.cross( diff_current_target );
  const double norm_plane = cv::norm(planeNormal);

  if(norm_plane != 0)
  {
    planeNormal /= norm_plane;
  }

  mpPlaneNormal->setData(planeNormal);

  cv::Mat influenceDir = current_pos.cross( planeNormal );
  const double norm_inf_dir = cv::norm(influenceDir);

  if( norm_inf_dir != 0)
  {
    influenceDir /= norm_inf_dir;
  }

  influenceDir *= cv::norm(current_pos); //can change the influence dir vector to 0, if reference was 0
  mpOrthogonalAcceleration->setData(influenceDir);
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
  if( slot->getName() == "endeffector position vector" || slot->getName() == "difference vector to target location")
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
  if (inputName == "endeffector position vector")
  {
    mpCurrentPositionVector = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
  else if (inputName == "difference vector to target location")
  {
    mpTargetDifferenceVector = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
}

