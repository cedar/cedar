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

    File:        ChannelMerge.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2014 06 11

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/steps/ChannelMerge.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/Arguments.h"
#include "cedar/processing/typecheck/SameSize.h"
#include "cedar/processing/typecheck/And.h"
#include "cedar/processing/typecheck/Matrix.h"
#include "cedar/auxiliaries/annotation/ColorSpace.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/casts.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/stringFunctions.h"

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
      new ElementDeclarationTemplate<cedar::proc::steps::ChannelMerge>
      (
        "Image Processing",
        "cedar.processing.steps.ChannelMerge"
      )
    );
    declaration->setIconPath(":/steps/channel_merge.svg");
    declaration->setDescription
    (
      "merges matrices (up to four channels) into a single, multi-channel matrix."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::ChannelMerge::ChannelMerge()
:
mOutput(new cedar::aux::MatData(cv::Mat()))
{
  // declare slots
  auto slot_1 = this->declareInput("channel 1");
  auto slot_2 = this->declareInput("channel 2");
  auto slot_3 = this->declareInput("channel 3");

  // define checks
  cedar::proc::typecheck::And input_check;
  cedar::proc::typecheck::SameSize same_check;
  cedar::proc::typecheck::Matrix matrix_check;
  matrix_check.addAcceptedDimensionality(2);
  matrix_check.addAcceptedType(CV_8U);
  same_check.addSlot(slot_1);
  same_check.addSlot(slot_2);
  same_check.addSlot(slot_3);
  input_check.addCheck(same_check);
  input_check.addCheck(matrix_check);

  // set check
  slot_1->setCheck(input_check);
  slot_2->setCheck(input_check);
  slot_3->setCheck(input_check);

  this->mChannels.resize(3);
  this->declareOutput("output", mOutput);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::steps::ChannelMerge::inputConnectionChanged(const std::string& inputName)
{
  cedar::aux::ConstDataPtr data = this->getInput(inputName);

  if (!data)
  {
    return;
  }

  auto mat_data = cedar::aux::asserted_pointer_cast<cedar::aux::ConstMatData>(data);
  if (inputName == "channel 1")
  {
    this->mChannels.at(0) = cedar::aux::asserted_pointer_cast<cedar::aux::ConstMatData>(mat_data);
    this->redetermineInputValidity("channel 2");
    this->redetermineInputValidity("channel 3");
  }
  if (inputName == "channel 2")
  {
    this->mChannels.at(1) = cedar::aux::asserted_pointer_cast<cedar::aux::ConstMatData>(mat_data);
    this->redetermineInputValidity("channel 1");
    this->redetermineInputValidity("channel 3");
  }
  if (inputName == "channel 3")
  {
    this->mChannels.at(2) = cedar::aux::asserted_pointer_cast<cedar::aux::ConstMatData>(mat_data);
    this->redetermineInputValidity("channel 1");
    this->redetermineInputValidity("channel 2");
  }
  this->mOutput->setData(cv::Mat::zeros(mat_data->getData().rows, mat_data->getData().cols, CV_8UC3));

  this->emitOutputPropertiesChangedSignal("output");
  this->onTrigger();
}

void cedar::proc::steps::ChannelMerge::compute(const cedar::proc::Arguments& /* arguments */)
{
  cv::Mat& output = this->mOutput->getData();

  std::vector<cv::Mat> channels;
  for (unsigned int i = 0; i < 3; ++i)
  {
    if (this->mChannels.at(i))
    {
      channels.push_back(this->mChannels.at(i)->getData().clone());
    }
  }
  if (channels.size() == 3)
  {
    cv::merge(channels, output);
  }
}
