/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ColorConversion.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 07 03

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/steps/ChannelSplit.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/annotation/ColorSpace.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/casts.h"

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
      new ElementDeclarationTemplate<cedar::proc::steps::ChannelSplit>
      (
        "Image Processing",
        "cedar.processing.steps.ChannelSplit"
      )
    );
    declaration->setIconPath(":/steps/channel_split.svg");
    cedar::aux::Singleton<cedar::proc::DeclarationRegistry>::getInstance()->declareClass(declaration);

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::ChannelSplit::ChannelSplit()
:
mChannel1(new cedar::aux::MatData(cv::Mat(2, 2, CV_32F))),
mChannel2(new cedar::aux::MatData(cv::Mat(2, 2, CV_32F))),
mChannel3(new cedar::aux::MatData(cv::Mat(2, 2, CV_32F))),
mChannel4(new cedar::aux::MatData(cv::Mat(2, 2, CV_32F)))
{
  this->declareInput("three channel input");

  this->declareOutput("channel 1", mChannel1);
  this->declareOutput("channel 2", mChannel2);
  this->declareOutput("channel 3", mChannel3);
  this->declareOutput("channel 4", mChannel4);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::ChannelSplit::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr /* slot */,
                                  cedar::aux::DataPtr data
                                ) const
{
  if (cedar::aux::MatDataPtr mat_data = boost::dynamic_pointer_cast<cedar::aux::MatData>(data))
  {
    try
    {
      mat_data->getAnnotation<cedar::aux::annotation::ColorSpace>();

      if (mat_data->getData().channels() <= 4)
      {
        return cedar::proc::DataSlot::VALIDITY_VALID;
      }
    }
    catch (cedar::aux::AnnotationNotFoundException)
    {
      // not valid data
    }
  }

  // finally, return that this is an errorneous connection
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::ChannelSplit::inputConnectionChanged(const std::string& inputName)
{
  this->mInput = cedar::aux::asserted_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));

  // copy annotations
  this->mChannel1->copyAnnotationsFrom(this->mInput);
  this->mChannel2->copyAnnotationsFrom(this->mInput);
  this->mChannel3->copyAnnotationsFrom(this->mInput);
  this->mChannel4->copyAnnotationsFrom(this->mInput);

  // split the channels
  cedar::aux::annotation::ConstColorSpacePtr color_space
    = this->mInput->getAnnotation<cedar::aux::annotation::ColorSpace>();
  unsigned int num_channels = static_cast<unsigned int>(this->mInput->getData().channels());

  CEDAR_ASSERT(color_space->getNumberOfChannels() == num_channels);


  // reset channels
  this->mChannel1->setData(cv::Mat::zeros(2, 2, this->mInput->getData().type()));
  this->mChannel2->setData(cv::Mat::zeros(2, 2, this->mInput->getData().type()));
  this->mChannel3->setData(cv::Mat::zeros(2, 2, this->mInput->getData().type()));
  this->mChannel4->setData(cv::Mat::zeros(2, 2, this->mInput->getData().type()));

  int type = CV_MAKETYPE(this->mInput->getData().depth(), 1);

  // apply channel annotations and preallocate matrices
  switch (num_channels)
  {
  case 4:
    this->mChannel4->setAnnotation
    (
      cedar::aux::annotation::ColorSpacePtr(new cedar::aux::annotation::ColorSpace(color_space->getChannelType(3)))
    );
    this->mChannel4->setData
    (
      cv::Mat::zeros(this->mInput->getData().rows, this->mInput->getData().cols, type)
    );

  case 3:
    this->mChannel3->setAnnotation
    (
      cedar::aux::annotation::ColorSpacePtr(new cedar::aux::annotation::ColorSpace(color_space->getChannelType(2)))
    );
    this->mChannel3->setData
    (
      cv::Mat::zeros(this->mInput->getData().rows, this->mInput->getData().cols, type)
    );

  case 2:
    this->mChannel2->setAnnotation
    (
      cedar::aux::annotation::ColorSpacePtr(new cedar::aux::annotation::ColorSpace(color_space->getChannelType(1)))
    );
    this->mChannel2->setData
    (
      cv::Mat::zeros(this->mInput->getData().rows, this->mInput->getData().cols, type)
    );

  case 1:
    this->mChannel1->setAnnotation
    (
      cedar::aux::annotation::ColorSpacePtr(new cedar::aux::annotation::ColorSpace(color_space->getChannelType(0)))
    );
    this->mChannel1->setData
    (
      cv::Mat::zeros(this->mInput->getData().rows, this->mInput->getData().cols, type)
    );

  default:
    // do nothing
    break;
  }

  this->onTrigger();
}

void cedar::proc::steps::ChannelSplit::compute(const cedar::proc::Arguments& /* arguments */)
{
  const cv::Mat& input = this->mInput->getData();

  //!@todo Store this vector as a member to make this faster
  std::vector<cv::Mat> channels;
  for (int i = 0; i < input.channels(); ++i)
  {
    channels.push_back(cv::Mat());
  }

  cv::split(input, channels);

  switch (input.channels())
  {
    case 4:
      this->mChannel4->setData(channels.at(3));
    case 3:
      this->mChannel3->setData(channels.at(2));
    case 2:
      this->mChannel2->setData(channels.at(1));
    case 1:
    default:
      this->mChannel1->setData(channels.at(0));
  }
}
