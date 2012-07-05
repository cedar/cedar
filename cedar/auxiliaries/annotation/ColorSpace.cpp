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

    File:        Dimensions.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 06 29

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/annotation/ColorSpace.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------


cedar::aux::annotation::ColorSpace::ColorSpace(ChannelType channelType)
{
  this->mChannelTypes.push_back(channelType);
}

cedar::aux::annotation::ColorSpace::ColorSpace
(
  ChannelType channel1Type,
  ChannelType channel2Type,
  ChannelType channel3Type
)
{
  this->mChannelTypes.push_back(channel1Type);
  this->mChannelTypes.push_back(channel2Type);
  this->mChannelTypes.push_back(channel3Type);
}

cedar::aux::annotation::ColorSpace::ColorSpace
(
  ChannelType channel1Type,
  ChannelType channel2Type,
  ChannelType channel3Type,
  ChannelType channel4Type
)
{
  this->mChannelTypes.push_back(channel1Type);
  this->mChannelTypes.push_back(channel2Type);
  this->mChannelTypes.push_back(channel3Type);
  this->mChannelTypes.push_back(channel4Type);
}

cedar::aux::annotation::ColorSpace::ColorSpace(const cedar::aux::annotation::ColorSpace& copy)
:
cedar::aux::annotation::Annotation(copy)
{
  this->mChannelTypes = copy.mChannelTypes;
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

unsigned int cedar::aux::annotation::ColorSpace::getNumberOfChannels() const
{
  return this->mChannelTypes.size();
}

cedar::aux::annotation::ColorSpace::ChannelType cedar::aux::annotation::ColorSpace::getChannelType
                                                (
                                                  unsigned int channel
                                                ) const
{
  CEDAR_ASSERT(channel < this->getNumberOfChannels());

  return this->mChannelTypes[channel];
}
