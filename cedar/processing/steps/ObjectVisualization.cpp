/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ObjectVisualization.cpp

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2014 10 21

    Description: Source file for the class cedar::proc::steps::ObjectVisualization.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/steps/ObjectVisualization.h"
#include "cedar/auxiliaries/gl/GlobalScene.h"
#include "cedar/auxiliaries/gl/Block.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"

// SYSTEM INCLUDES

namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration
    (
      new ElementDeclarationTemplate<cedar::proc::steps::ObjectVisualization>
      (
        "Simulated Scene",
        "cedar.processing.steps.ObjectVisualization"
      )
    );
    declaration->setDescription
                      (
                        "A simple geometric shape in the simulated scene."
                      );
//    declaration->setIconPath(":/steps/convolution.svg");

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::ObjectVisualization::ObjectVisualization() :
mOutput(new cedar::aux::MatData(cv::Mat::zeros(3, 1, CV_64F))),
_mVisualizedObject( 
  new cedar::proc::steps::ObjectVisualization::ObjectVisualizationParameter(
    this, 
    "default rigid body", 
    cedar::aux::gl::ObjectVisualizationPtr( new cedar::aux::gl::Block ) 
  )
)
{
  // declare all data
  this->declareInput("Cartesian position");
  this->declareOutput("Cartesian position", this->mOutput);

  this->mInputs = this->getInputSlot("Cartesian position");

  // a default rigid body:
  auto scene = cedar::aux::gl::GlobalSceneSingleton::getInstance();

  scene->addObjectVisualization( _mVisualizedObject->getValue() );
std::cout << "started" << std::endl;  
}

cedar::proc::steps::ObjectVisualization::~ObjectVisualization()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::ObjectVisualization::compute(const cedar::proc::Arguments&)
{
  // todo
}

