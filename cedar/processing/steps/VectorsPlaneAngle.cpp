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
  mpOrthogonalAcceleration(new cedar::aux::MatData(cv::Mat::zeros(3, 1, CV_64F))),
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

void cedar::proc::steps::VectorsPlaneAngle::compute(const cedar::proc::Arguments&)
{
  if(!mpEndeffectorVelocity || !mpEndeffectorPosition || !mpTargetPosition)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Not all inputs to VectorsPlaneAngle are valid. Please check your architecture.",
      CEDAR_CURRENT_FUNCTION_NAME
    );
    return;
  }

  const cv::Mat &current_vel = mpEndeffectorVelocity->getData();

  // calculate distance from current endeffector position to target position
  const cv::Mat current_pos_diff = mpTargetPosition->getData() - mpEndeffectorPosition->getData();

  if(_mVisualiseTarget->getValue())
  {
    mVisualisationPtr->getLocalCoordinateFrame()->setTranslation(mpTargetPosition->getData().at<cedar::unit::Length>(0),
                                                                 mpTargetPosition->getData().at<cedar::unit::Length>(1),
                                                                 mpTargetPosition->getData().at<cedar::unit::Length>(2));
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
  if( slot->getName() == "endeffector velocity" || slot->getName() == "endeffector position" || slot->getName() == "target position")
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
  else if (inputName == "endeffector position")
  {
    mpEndeffectorPosition = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
  else if (inputName == "target position")
  {
    mpTargetPosition = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }
}

void cedar::proc::steps::VectorsPlaneAngle::visualisationChanged()
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
      mVisualisationPtr->getLocalCoordinateFrame()->setTranslation(mpTargetPosition->getData().at<cedar::unit::Length>(0),
                                                                   mpTargetPosition->getData().at<cedar::unit::Length>(1),
                                                                   mpTargetPosition->getData().at<cedar::unit::Length>(2));
    }

    _mVisualisationID = scene->addObjectVisualization(mVisualisationPtr);
  }
  else
  {
    scene->deleteObjectVisualization(_mVisualisationID);
  }
}
