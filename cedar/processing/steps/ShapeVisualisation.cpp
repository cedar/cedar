/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
    This file is part of cedar.

    cedar is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    cedar is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with cedar. If not, see <http://www.gnu.org/licenses/>.

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        ShapeVisualisation.cpp

    Maintainer:  Nico Kuerschner
    Email:       Nico.Kuerschner@ini.rub.de
    Date:        2017 04 28

    Description: Source file for the class cedar::proc::steps::ShapeVisualisation.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/ShapeVisualisation.h"

// CEDAR INCLUDES
#include "cedar/processing/ElementDeclaration.h"

// SYSTEM INCLUDES

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
      new ElementDeclarationTemplate<cedar::proc::steps::ShapeVisualisation>
      (
        "Sinks",
        "cedar.processing.steps.ShapeVisualisation"
      )
    );
    declaration->setIconPath(":/steps/shape_visualisation.svg");
    declaration->setDescription
    (
      "Visualise a target position with a shape in the virtual scene"
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::EnumType<cedar::proc::steps::ShapeVisualisation::Colour>
  cedar::proc::steps::ShapeVisualisation::Colour::mColourType("ShapeVisualisation.Colour.");

cedar::aux::EnumType<cedar::proc::steps::ShapeVisualisation::Shape>
  cedar::proc::steps::ShapeVisualisation::Shape::mShapeType("ShapeVisualisation.Shape.");

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::ShapeVisualisation::ShapeVisualisation()
  :
  Step(false),
  _mColour
  (
    new cedar::aux::EnumParameter
    (
      this,
      "colour",
      cedar::proc::steps::ShapeVisualisation::Colour::typePtr(),
      cedar::proc::steps::ShapeVisualisation::Colour::green
    )
  ),
  _mShape
  (
    new cedar::aux::EnumParameter
    (
      this,
      "shape",
      cedar::proc::steps::ShapeVisualisation::Shape::typePtr(),
      cedar::proc::steps::ShapeVisualisation::Shape::sphere
    )
  ),
  _mRadius(new cedar::aux::DoubleParameter(this, "radius", 0.1, 0, 10000.0, 0.025)),
  _mLength(new cedar::aux::DoubleParameter(this, "length", 0.1, 0, 10000.0, 0.025)),
  _mWidth(new cedar::aux::DoubleParameter( this, "width",  0.1, 0, 10000.0, 0.025)),
  _mHeight(new cedar::aux::DoubleParameter(this, "height", 0.1, 0, 10000.0, 0.025)),


  mInputName("target position")
{  
  this->declareInput(mInputName);
  visualisationChanged();

  /*@TODO: It would be better if these could be hidden instead of set constant.
           setHidden exists as a Function, however changing a parameters visibility
           this way only transfers to the UI after some change was made to this steps input.
  */
  _mRadius->setConstant(false);
  _mLength->setConstant(true);
  _mWidth->setConstant(true);
  _mHeight->setConstant(true);

  QObject::connect(this->_mColour.get(), SIGNAL(valueChanged()), this, SLOT(visualisationChanged()));
  QObject::connect(this->_mShape.get(), SIGNAL(valueChanged()), this, SLOT(visualisationChanged()));
  QObject::connect(this->_mRadius.get(), SIGNAL(valueChanged()), this, SLOT(visualisationChanged()));
  QObject::connect(this->_mLength.get(), SIGNAL(valueChanged()), this, SLOT(visualisationChanged()));
  QObject::connect(this->_mWidth.get(), SIGNAL(valueChanged()), this, SLOT(visualisationChanged()));
  QObject::connect(this->_mHeight.get(), SIGNAL(valueChanged()), this, SLOT(visualisationChanged()));

}

cedar::proc::steps::ShapeVisualisation::~ShapeVisualisation()
{
  if (mVisualisationPtr)
  {
    auto scene = cedar::aux::gl::GlobalSceneSingleton::getInstance();

    scene->deleteObjectVisualizationPtr(mVisualisationPtr);
  }
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::ShapeVisualisation::determineInputValidity
  (
   cedar::proc::ConstDataSlotPtr slot,
   cedar::aux::ConstDataPtr data
  ) const
{

  cedar::aux::ConstMatDataPtr _input = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
  if(slot->getName() == mInputName)
  {
    if (_input && _input->getDimensionality() == 1 && cedar::aux::math::get1DMatrixSize(_input->getData()) == 3 && _input->getData().type() == CV_32F)
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }

  // else
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::ShapeVisualisation::compute(const cedar::proc::Arguments &)
{
    float modifier = 0.0;

    //! Adjust vertical Position so all Shapes are placed ON the ground by default
    switch (this->_mShape->getValue()) {
        default:
        case Shape::sphere:
        case Shape::torus:
            //! Shift 1 Radius upwards
            modifier = _mRadius->getValue();
            break;
        case Shape::block:
        case Shape::cylinder:
        case Shape::pyramid:
            //! Shift 1/2 Height upwards
            modifier = 0.5 * _mHeight->getValue();
            break;
        case Shape::prism:
        case Shape::cone:
            //! No adjustment needed
            modifier = 0.0;
            break;
    }
    mVisualisationPtr->getLocalCoordinateFrame()->setTranslation
    (
    std::vector<float>
              (
              {
                 mpTargetPosition->getData().at<float>(0),
                 mpTargetPosition->getData().at<float>(1),
                 static_cast<float>(mpTargetPosition->getData().at<float>(2)  + modifier)
                 }
              )
    );
}

void cedar::proc::steps::ShapeVisualisation::inputConnectionChanged(const std::string &inputName)
{
  if (inputName == mInputName)
  {
    mpTargetPosition = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>( this->getInput(inputName) );
  }

  visualisationChanged();
}

void cedar::proc::steps::ShapeVisualisation::visualisationChanged()
{
  auto scene = cedar::aux::gl::GlobalSceneSingleton::getInstance();

  if(mVisualisationPtr)
  {
    scene->deleteObjectVisualizationPtr(mVisualisationPtr);


    mVisualisationPtr.reset();
  }

  float r = 0;
  float g = 0;
  float b = 0;

  switch (this->_mColour->getValue())
  {
    default:
    case Colour::black:
      break;

    case Colour::red:
    {
      r = 1;
      break;
    }

    case Colour::green:
    {
      g = 1;
      break;
    }

    case Colour::blue:
    {
      b = 1;
      break;
    }

    case Colour::yellow:
    {
      r = 1;
      g = 1;
      break;
    }

    case Colour::purple:
    {
      r = 1;
      b = 1;
      break;
    }

    case Colour::cyan:
    {
      g = 1;
      b = 1;
      break;
    }

    case Colour::white:
    {
      r = 1;
      g = 1;
      b = 1;
      break;
    }
  }
  const float radius = _mRadius->getValue();
  const float length = _mLength->getValue();
  const float width = _mWidth->getValue();
  const float height = _mHeight->getValue();


  switch (this->_mShape->getValue())
  {
    default:
    case Shape::sphere:
    {
        _mRadius->setConstant(false);
        _mLength->setConstant(true);
        _mWidth->setConstant(true);
        _mHeight->setConstant(true);

      mVisualisationPtr = cedar::aux::gl::ObjectVisualizationPtr
      (
        new cedar::aux::gl::Sphere
        (
          cedar::aux::LocalCoordinateFramePtr(new cedar::aux::LocalCoordinateFrame),
          radius, r, g, b
        )
      );
      break;
    }

    case Shape::block:
    {
        _mRadius->setConstant(true);
        _mLength->setConstant(false);
        _mWidth->setConstant(false);
        _mHeight->setConstant(false);


        mVisualisationPtr = cedar::aux::gl::ObjectVisualizationPtr
      (
        new cedar::aux::gl::Block
        (
          cedar::aux::LocalCoordinateFramePtr(new cedar::aux::LocalCoordinateFrame),
          length, width, height, r, g, b
        )
      );
      break;
    }

    case Shape::cone:
    {
        _mRadius->setConstant(false);
        _mLength->setConstant(true);
        _mWidth->setConstant(true);
        _mHeight->setConstant(false);
      mVisualisationPtr = cedar::aux::gl::ObjectVisualizationPtr
      (
        new cedar::aux::gl::Cone
        (
          cedar::aux::LocalCoordinateFramePtr(new cedar::aux::LocalCoordinateFrame),
          radius, height, r, g, b
        )
      );
      break;
    }

    case Shape::cylinder:
    {
        _mRadius->setConstant(false);
        _mLength->setConstant(true);
        _mWidth->setConstant(true);
        _mHeight->setConstant(false);
      mVisualisationPtr = cedar::aux::gl::ObjectVisualizationPtr
      (
        new cedar::aux::gl::Cylinder
        (
          cedar::aux::LocalCoordinateFramePtr(new cedar::aux::LocalCoordinateFrame),
          radius, height, r, g, b
        )
      );
      break;
    }

    case Shape::prism:
    {
        _mRadius->setConstant(true);
        _mLength->setConstant(true);
        _mWidth->setConstant(false);
        _mHeight->setConstant(false);
      mVisualisationPtr = cedar::aux::gl::ObjectVisualizationPtr
      (
        new cedar::aux::gl::Prism
        (
          cedar::aux::LocalCoordinateFramePtr(new cedar::aux::LocalCoordinateFrame),
          width, height, r, g, b
        )
      );
      break;
    }

    case Shape::pyramid:
    {
        _mRadius->setConstant(true);
        _mLength->setConstant(false);
        _mWidth->setConstant(false);
        _mHeight->setConstant(false);
      mVisualisationPtr = cedar::aux::gl::ObjectVisualizationPtr
      (
        new cedar::aux::gl::Pyramid
        (
          cedar::aux::LocalCoordinateFramePtr(new cedar::aux::LocalCoordinateFrame),
          width, length, height, r, g, b
        )
      );
      break;
    }

    case Shape::torus:
    {
        _mRadius->setConstant(false);
        _mLength->setConstant(true);
        _mWidth->setConstant( false);
        _mHeight->setConstant(true);
      mVisualisationPtr = cedar::aux::gl::ObjectVisualizationPtr
      (
        new cedar::aux::gl::Torus
        (
          cedar::aux::LocalCoordinateFramePtr(new cedar::aux::LocalCoordinateFrame),
          width, radius, r, g, b
        )
      );
      break;
    }
  }

  if(!mpTargetPosition)
  {
      return;
  }
  if (mVisualisationPtr) {


      scene->addObjectVisualization(mVisualisationPtr);
  }
  this->callComputeWithoutTriggering();

}
