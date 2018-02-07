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
  _mScale(new cedar::aux::DoubleParameter(this, "scale", 0.025)),
  mInputName("target position")
{  
  this->declareInput(mInputName);

  visualisationChanged();

  QObject::connect(this->_mColour.get(), SIGNAL(valueChanged()), this, SLOT(visualisationChanged()));
  QObject::connect(this->_mShape.get(), SIGNAL(valueChanged()), this, SLOT(visualisationChanged()));
  QObject::connect(this->_mScale.get(), SIGNAL(valueChanged()), this, SLOT(visualisationChanged()));

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

    if(!mpTargetPosition)
    {
      return;
    }

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

  const float scale = _mScale->getValue();

  switch (this->_mShape->getValue())
  {
    default:
    case Shape::sphere:
    {
      mVisualisationPtr = cedar::aux::gl::ObjectVisualizationPtr
      (
        new cedar::aux::gl::Sphere
        (
          cedar::aux::LocalCoordinateFramePtr(new cedar::aux::LocalCoordinateFrame),
          2*scale, r, g, b
        )
      );
      break;
    }

    case Shape::block:
    {
      mVisualisationPtr = cedar::aux::gl::ObjectVisualizationPtr
      (
        new cedar::aux::gl::Block
        (
          cedar::aux::LocalCoordinateFramePtr(new cedar::aux::LocalCoordinateFrame),
          3*scale, 2*scale, scale, r, g, b
        )
      );
      break;
    }

    case Shape::cone:
    {
      mVisualisationPtr = cedar::aux::gl::ObjectVisualizationPtr
      (
        new cedar::aux::gl::Cone
        (
          cedar::aux::LocalCoordinateFramePtr(new cedar::aux::LocalCoordinateFrame),
          2*scale, 2*scale, r, g, b
        )
      );
      break;
    }

    case Shape::cylinder:
    {
      mVisualisationPtr = cedar::aux::gl::ObjectVisualizationPtr
      (
        new cedar::aux::gl::Cylinder
        (
          cedar::aux::LocalCoordinateFramePtr(new cedar::aux::LocalCoordinateFrame),
          scale, 2*scale, r, g, b
        )
      );
      break;
    }

    case Shape::prism:
    {
      mVisualisationPtr = cedar::aux::gl::ObjectVisualizationPtr
      (
        new cedar::aux::gl::Prism
        (
          cedar::aux::LocalCoordinateFramePtr(new cedar::aux::LocalCoordinateFrame),
          3*scale, scale, r, g, b
        )
      );
      break;
    }

    case Shape::pyramid:
    {
      mVisualisationPtr = cedar::aux::gl::ObjectVisualizationPtr
      (
        new cedar::aux::gl::Pyramid
        (
          cedar::aux::LocalCoordinateFramePtr(new cedar::aux::LocalCoordinateFrame),
          2*scale, 3*scale, 2*scale, r, g, b
        )
      );
      break;
    }

    case Shape::torus:
    {
      mVisualisationPtr = cedar::aux::gl::ObjectVisualizationPtr
      (
        new cedar::aux::gl::Torus
        (
          cedar::aux::LocalCoordinateFramePtr(new cedar::aux::LocalCoordinateFrame),
          3*scale, 0.5*scale, r, g, b
        )
      );
      break;
    }
  }

  if (mVisualisationPtr)
  {
    scene->addObjectVisualization(mVisualisationPtr);
  }

}
