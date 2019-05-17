// LOCAL INCLUDES
#include "AnglesOnPlane.h"
#include "cedar/processing/ElementDeclaration.h"
#include <cedar/auxiliaries/DoubleVectorParameter.h>
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
      new ElementDeclarationTemplate<cedar::proc::steps::AnglesOnPlane>
      (
        "Robotics",
        "cedar.processing.steps.AnglesOnPlane"
      )
    );
    declaration->setIconPath(":/steps/rotation_on_plane.svg");
    declaration->setDescription
    (
      "Finds a plane (2D plane embedded in the 3D space) given by the "
      "input vectors and returns the angle between them and other useful "
      "information.\n"
      "The plane is specified by the two vectors ('vector a') and "
      "('vector b'), which can be 3D. "
      "The output 'angle ab' is calculated between these two base vectors. "
      "USAGE: This is typically used for finding a plane for a target "
      "position, "
      "a current position (use their difference as 'vector b') "
      "and a velocity vector "
      "('vector a') and returning the angle between these, "
      "as well as returning "
      "a vector (the output 'vector orthogonal to a') that rotates "
      "'vector a' around that "
      "angle, while staying on the plane.\n"
      "DETAILS: The output 'vector orthogonal to a' is normalized. "
      "If one of the vectors has length zero, the returned angle value "
      "will be NaN. You may want to treat this with the NaNCheck step."
//      "You can specify with which value to replace angles if an angle is not "
//      "defined, for example either because of numeric underflow or because "
//      "the base vectors are colinear. The sensible default for this "
//      "replacement is zero."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::AnglesOnPlane::AnglesOnPlane()
  :
  mAngle(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
  mOrthogonalVector1(new cedar::aux::MatData(cv::Mat::zeros(3, 1, CV_32F))),
  //mUndefindeAngleReplacement(new cedar::aux::DoubleParameter(this, "angle replacement when undefined", 0.0)),
  mUndefinedOrthogonalVector1Replacement(new cedar::aux::DoubleVectorParameter(this, "vector orthogonal to a replacement when undefined", 3, 0.0001))
{

  this->declareInput("vector a");
  this->declareInput("vector b");

  this->declareOutput("angle a-b", mAngle);
  this->declareOutput("vector orthogonal to a", mOrthogonalVector1);

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::AnglesOnPlane::compute(const cedar::proc::Arguments&)
{
  if(!mVector1 || !mVector2)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Not all inputs to AnglesOnPlane are valid. Please check your architecture.",
      CEDAR_CURRENT_FUNCTION_NAME
    );
    return;
  }

  const cv::Mat vector1 = mVector1->getData();
  const cv::Mat vector2 = mVector2->getData();


  // calculate angle as phi = acos(v.k / |v||k|)
  const float dotproduct = vector1.dot(vector2);
  const float normvectors = cv::norm(vector1) * cv::norm(vector2);
  float angle = 0;

  if (!cedar::aux::math::isZero(normvectors))
  {
    angle = acos(dotproduct / normvectors);

    if (std::isnan(angle)
        ||  std::isinf(angle))
    {
      angle = std::nan(""); // this can be captured by subsequent steps
        // mUndefindeAngleReplacement->getValue();
        // generally 0 : assume that we want to move towards the target
    }
  }    
  else
  { 
    angle = std::nan(""); // this can be captured by subsequent steps
    //angle = mUndefindeAngleReplacement->getValue();
  }

  mAngle->getData().at<float>(0,0) = angle;



  // calculate the orthogonal vector:
  cv::Mat orthogonal_vector = vector2.cross(vector1).cross(vector1);

  const float norm_orthogonal_vector = cv::norm(orthogonal_vector);
  if (!cedar::aux::math::isZero( norm_orthogonal_vector ))
  {
    //norm_term = cv::norm(vector1) / norm_orthogonal_vector;
    mOrthogonalVector1->setData(orthogonal_vector / norm_orthogonal_vector);
  }
  else
  {
    mOrthogonalVector1->setData(orthogonal_vector); // TODO
  }
}

//// validity check
cedar::proc::DataSlot::VALIDITY cedar::proc::steps::AnglesOnPlane::determineInputValidity
  (
   cedar::proc::ConstDataSlotPtr slot,
   cedar::aux::ConstDataPtr data
  ) const
{
  //all inputs have same type
  cedar::aux::ConstMatDataPtr _input = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
  if( slot->getName() == "vector a" || slot->getName() == "vector b" )
  {
    if (_input && _input->getDimensionality() == 1 && cedar::aux::math::get1DMatrixSize(_input->getData()) == 3 && _input->getData().type() == CV_32F)
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }
  
  // else
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::AnglesOnPlane::inputConnectionChanged(const std::string& inputName)
{
  if (inputName == "vector a")
  {
    mVector1 = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
  else if (inputName == "vector b")
  {
    mVector2 = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
}

