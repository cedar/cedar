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
  mpOrthogonalInfluence(new cedar::aux::MatData(cv::Mat::zeros(3, 1, CV_64F)))
{
  this->declareInput("reference vector");
  this->declareInput("influence vector");

  this->declareOutput("angle", mpAngle);
  this->declareOutput("plane normal", mpPlaneNormal);
  this->declareOutput("orthogonal influence", mpOrthogonalInfluence);

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::VectorsPlaneAngle::compute(const cedar::proc::Arguments&)
{
  if(!mpReferenceVector || !mpInfluenceVector)
    return;

  cv::Mat referenceVector = mpReferenceVector->getData().clone();
  cv::Mat influenceVector =  mpInfluenceVector->getData().clone();

  //angle
  double dot = referenceVector.dot( influenceVector );
  double norm_ref = cv::norm(referenceVector);
  double norm_inf = cv::norm(influenceVector);
  double norm = norm_ref * norm_inf;
  double angle;
  if( norm == 0. )
  {
    angle = 0;
  //  std::cout << "Plane Angle: 0 norm" << std::endl;
  }  
  else
    angle = acos( dot / norm ); //possible devision by 0!
  mpAngle->getData().at<double>(0,0) = angle;  

  //plane normal
  cv::Mat planeNormal = referenceVector.cross( influenceVector );
  double norm_plane = cv::norm(planeNormal);
  if( norm_plane != 0. )
    planeNormal /= norm_plane;
  mpPlaneNormal->setData(planeNormal);
  //planeNormal.copyTo(mpPlaneNormal->getData() );
  //for(int i = 0; i < 3; i++)
  //  mpPlaneNormal->getData().at<double>(i,0) = planeNormal.at<double>(i,0);

  //influence direction
  cv::Mat influenceDir = referenceVector.cross( planeNormal );
  double norm_inf_dir = cv::norm(influenceDir);
  if( norm_inf_dir != 0. )
    influenceDir /= norm_inf_dir;
  influenceDir *= norm_ref; //can change the influence dir vector to 0, if reference was 0
  mpOrthogonalInfluence->setData(influenceDir);
  //influenceDir.copyTo(mpOrthogonalInfluence->getData() );
  //for(int i = 0; i < 3; i++)
  //  mpOrthogonalInfluence->getData().at<double>(i,0) = influenceDir.at<double>(i,0);
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
  if( slot->getName() == "reference vector" || slot->getName() == "influence vector")
  {
    if (_input && _input->getDimensionality() == 1 && cedar::aux::math::get1DMatrixSize(_input->getData()) == 3 && _input->getData().type() == CV_64F)
      return cedar::proc::DataSlot::VALIDITY_VALID;
  }
  
  // else
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::VectorsPlaneAngle::inputConnectionChanged(const std::string& inputName){
  if (inputName == "reference vector")
  {
    mpReferenceVector = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
  else if (inputName == "influence vector")
  {
    mpInfluenceVector = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
}

