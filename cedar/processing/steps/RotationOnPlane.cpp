// LOCAL INCLUDES
#include "RotationOnPlane.h"
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
      new ElementDeclarationTemplate<cedar::proc::steps::RotationOnPlane>
      (
        "Robotics",
        "cedar.processing.steps.RotationOnPlane"
      )
    );
    declaration->setIconPath(":/steps/rotation_on_plane.svg");
    declaration->setDescription
    (
      "Obtain angle and orthogonal acceleration, given target position, eef position, eef velocity"
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::RotationOnPlane::RotationOnPlane()
  :
  mpAngle(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
  mpOrthogonalAcceleration(new cedar::aux::MatData(cv::Mat::zeros(3, 1, CV_32F))),
  _mVisualiseTarget(new cedar::aux::BoolParameter(this, "visualise target position", false)),
  _mVisualisationColour(new cedar::aux::BoolParameter(this, "visualisation is obstacle", false))
{

  this->declareInput("target position");
  this->declareInput("endeffector position");
  this->declareInput("endeffector velocity");

  this->declareOutput("angle", mpAngle);
  this->declareOutput("orthogonal acceleration", mpOrthogonalAcceleration);

  QObject::connect(_mVisualiseTarget.get(), SIGNAL(valueChanged()), this, SLOT(visualisationChanged()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::RotationOnPlane::compute(const cedar::proc::Arguments&)
{
  if(!mpEndeffectorVelocity || !mpEndeffectorPosition || !mpTargetPosition)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Not all inputs to RotationOnPlane are valid. Please check your architecture.",
      CEDAR_CURRENT_FUNCTION_NAME
    );
    return;
  }

  const cv::Mat &current_vel = mpEndeffectorVelocity->getData();

  // calculate distance from current endeffector position to target position
  const cv::Mat current_pos_diff = mpTargetPosition->getData() - mpEndeffectorPosition->getData();

  if(_mVisualiseTarget->getValue())
  {
    mVisualisationPtr->getLocalCoordinateFrame()->setTranslation
              (
                std::vector<float>
                (
                  {
                    mpTargetPosition->getData().at<float>(0),
                    mpTargetPosition->getData().at<float>(1),
                    mpTargetPosition->getData().at<float>(2)
                  }
                )
              );
  }

  // calculate angle as phi = acos(v.k / |v||k|)
  const float dot = current_vel.dot(current_pos_diff);
  const float norm = cv::norm(current_vel) * cv::norm(current_pos_diff);
  float angle = 0;

  if(norm != 0)
  {
    angle = acos(dot / norm);
  }    

  mpAngle->getData().at<float>(0,0) = angle;

  // direction of most effective change w_dir = (k x v) x v * ( ||v|| / ||(k x v) x v|| )
  cv::Mat w_dir = current_pos_diff.cross(current_vel).cross(current_vel);
  float norm_term = 0;

  const float norm_w_dir = cv::norm(w_dir);
  if(norm_w_dir != 0)
  {
    norm_term = cv::norm(current_vel) / norm_w_dir;
  }

  mpOrthogonalAcceleration->setData(w_dir * norm_term);
}

//// validity check
cedar::proc::DataSlot::VALIDITY cedar::proc::steps::RotationOnPlane::determineInputValidity
  (
   cedar::proc::ConstDataSlotPtr slot,
   cedar::aux::ConstDataPtr data
  ) const
{
  //all inputs have same type
  cedar::aux::ConstMatDataPtr _input = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
  if( slot->getName() == "endeffector velocity" || slot->getName() == "endeffector position" || slot->getName() == "target position")
  {
    if (_input && _input->getDimensionality() == 1 && cedar::aux::math::get1DMatrixSize(_input->getData()) == 3 && _input->getData().type() == CV_32F)
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }
  
  // else
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::RotationOnPlane::inputConnectionChanged(const std::string& inputName)
{
  if (inputName == "endeffector velocity")
  {
    mpEndeffectorVelocity = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
  else if (inputName == "endeffector position")
  {
    mpEndeffectorPosition = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
  else if (inputName == "target position")
  {
    mpTargetPosition = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
}

void cedar::proc::steps::RotationOnPlane::visualisationChanged()
{
  auto scene = cedar::aux::gl::GlobalSceneSingleton::getInstance();

  // add or remove visualisation
  if(_mVisualiseTarget->getValue())
  {

    float r = 0;
    float g = 1;

    if(_mVisualisationColour->getValue())
    {
      r=1;
      g=0;
    }

    mVisualisationPtr = cedar::aux::gl::ObjectVisualizationPtr
    (
      new cedar::aux::gl::Sphere
        (
          cedar::aux::LocalCoordinateFramePtr(new cedar::aux::LocalCoordinateFrame),
          0.05, r, g, 0
        )
    );

    if(mpTargetPosition)
    {
      mVisualisationPtr->getLocalCoordinateFrame()->setTranslation(std::vector<float>
                                                                   (
                                                                     {
                                                                       mpTargetPosition->getData().at<float>(0),
                                                                       mpTargetPosition->getData().at<float>(1),
                                                                       mpTargetPosition->getData().at<float>(2)
                                                                     }
                                                                   ));

    }

    _mVisualisationID = scene->addObjectVisualization(mVisualisationPtr);
  }
  else
  {
    scene->deleteObjectVisualization(_mVisualisationID);
  }
}
