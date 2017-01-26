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
#include "cedar/processing/Arguments.h"
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
      new ElementDeclarationTemplate<cedar::proc::steps::ChannelSplit>
      (
        "Image Processing",
        "cedar.processing.steps.ChannelSplit"
      )
    );
    declaration->setIconPath(":/steps/channel_split.svg");
    declaration->setDescription
    (
      "Splits a matrix with up to four channels into the according number of separate matrices."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::ChannelSplit::ChannelSplit()
{
  this->declareInput("three channel input");

  this->mChannelData.resize(4);
  for (size_t i = 0; i < this->mChannelData.size(); ++i)
  {
    this->mChannelData.at(i) = cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat()));
    this->declareOutput(this->generateDataName(i), this->mChannelData.at(i));
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::ChannelSplit::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr /* slot */,
                                  cedar::aux::ConstDataPtr data
                                ) const
{
  if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
  {
    if
    (
      !mat_data->isEmpty()
      && mat_data->getData().channels() > 1
      && mat_data->getData().channels() <= 4
      && cedar::aux::math::getDimensionalityOf(mat_data->getData()) < 3
    )
    {
      try
      {
        mat_data->getAnnotation<cedar::aux::annotation::ColorSpace>();
        return cedar::proc::DataSlot::VALIDITY_VALID;
      }
      catch (cedar::aux::AnnotationNotFoundException)
      {
        // the data is mat data but not annotated; that's ok
        return cedar::proc::DataSlot::VALIDITY_WARNING;
      }
    }
  }

  // finally, return that this is an errorneous connection
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::ChannelSplit::inputConnectionChanged(const std::string& inputName)
{
  cedar::aux::ConstDataPtr data = this->getInput(inputName);

  if (!data)
  {
    this->mInput.reset();
    return;
  }

  this->mInput = cedar::aux::asserted_pointer_cast<cedar::aux::ConstMatData>(data);

  // copy annotations
  for (size_t i = 0; i < this->mChannels.size(); ++i)
  {
    this->mChannelData.at(i)->copyAnnotationsFrom(this->mInput);
  }

  // split the channels
  unsigned int num_channels = static_cast<unsigned int>(this->mInput->getData().channels());

  cedar::aux::annotation::ConstColorSpacePtr color_space;
  try
  {
    color_space = this->mInput->getAnnotation<cedar::aux::annotation::ColorSpace>();

    CEDAR_ASSERT(color_space->getNumberOfChannels() >= num_channels);
  }
  catch (cedar::aux::AnnotationNotFoundException)
  {
  }


  // reset channels
  for (size_t i = 0; i < this->mChannelData.size(); ++i)
  {
    this->mChannelData.at(i)->setData(cv::Mat());
  }

  this->mChannels.resize(num_channels);

  int type = CV_MAKETYPE(this->mInput->getData().depth(), 1);

  if (cedar::aux::math::getDimensionalityOf(this->mInput->getData()) > 2)
  {
    // there are now empty matrices, notify the subsequent steps
    for (size_t i = 0; i < this->mChannelData.size(); ++i)
    {
      this->emitOutputPropertiesChangedSignal(this->generateDataName(i));
    }
    return;
  }

  for (size_t i = 0; i < num_channels; ++i)
  {
    if (color_space)
    {
      this->mChannelData.at(i)->setAnnotation
      (
        cedar::aux::annotation::ColorSpacePtr(new cedar::aux::annotation::ColorSpace(color_space->getChannelType(i)))
      );
    }
    this->mChannelData.at(i)->setData
    (
      cv::Mat::zeros(this->mInput->getData().rows, this->mInput->getData().cols, type)
    );
  }

  if
  (
    !this->mInput->isEmpty()
    && this->mInput->getData().channels() > 1
    && this->mInput->getData().channels() <= 4
    && cedar::aux::math::getDimensionalityOf(this->mInput->getData()) < 3
  )
  {
    this->callComputeWithoutTriggering();
    for (size_t i = 0; i < this->mChannelData.size(); ++i)
    {
      this->emitOutputPropertiesChangedSignal(this->generateDataName(i));
    }
  }
}

std::string cedar::proc::steps::ChannelSplit::generateDataName(unsigned int channel) const
{
  return "channel " + cedar::aux::toString(channel + 1);
}

void cedar::proc::steps::ChannelSplit::compute(const cedar::proc::Arguments& /* arguments */)
{
  const cv::Mat& input = this->mInput->getData();

  CEDAR_ASSERT(static_cast<size_t>(input.channels()) <= this->mChannelData.size());

  cv::split(input, this->mChannels);

  for (size_t i = 0; i < static_cast<size_t>(input.channels()); ++i)
  {
    this->mChannelData.at(i)->setData(this->mChannels.at(i));
  }
}
