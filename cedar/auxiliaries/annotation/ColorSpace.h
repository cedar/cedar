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

    File:        Dimensions.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 06 29

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_ANNOTATION_COLOR_SPACE_H
#define CEDAR_AUX_ANNOTATION_COLOR_SPACE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/annotation/Annotation.h"
#include "cedar/auxiliaries/Cloneable.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/annotation/ColorSpace.fwd.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>

/*!@brief An annotation that indicates a certain color space.
 *
 *        The color space annotation has a number of channes, and each channel can have a specific meaning. For example,
 *        channel one can be red, channel two green, and channel three blue to indicate an RGB color space.
 */
class cedar::aux::annotation::ColorSpace
:
public cedar::aux::annotation::Annotation,
public cedar::aux::Cloneable<cedar::aux::annotation::ColorSpace, cedar::aux::annotation::Annotation>
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Enum that indicates a channel's contents.
  enum ChannelType
  {
    Red,
    Green,
    Blue,
    Gray,
    Hue,
    Saturation,
    Value,
    Alpha,
    Luminance,
    ChromaticRed,
    ChromaticBlue,
    Lab_L,
    Lab_a,
    Lab_b
  };


  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor for a single channel.
  ColorSpace(ChannelType channelType);

  //!@brief Constructor for three channels.
  ColorSpace(ChannelType channel1Type, ChannelType channel2Type, ChannelType channel3Type);

  //!@brief Constructor for four channels.
  ColorSpace(ChannelType channel1Type, ChannelType channel2Type, ChannelType channel3Type, ChannelType channel4Type);

  //!@brief The copy constructor.
  ColorSpace(const cedar::aux::annotation::ColorSpace& copy);

  //!@brief Returns the channel type as a string.
  static const std::string& channelTypeToString(cedar::aux::annotation::ColorSpace::ChannelType type);

  //! Returns an annotation with the channels BGR
  static cedar::aux::annotation::ColorSpacePtr bgr();

  //! Returns an annotation with the channels BGRA
  static cedar::aux::annotation::ColorSpacePtr bgra();

  //! Returns an annotation with one gray channel
  static cedar::aux::annotation::ColorSpacePtr gray();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Returns the channel count.
  unsigned int getNumberOfChannels() const;

  //! Returns the type of the given channel.
  cedar::aux::annotation::ColorSpace::ChannelType getChannelType(unsigned int channel) const;

  //! Generates the a description of the color space.
  std::string getDescription() const;

  //! Returns a channel code, e.g., BGR, HSV, ...
  std::string getChannelCode() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Labels corresponding to the dimensions
  std::vector<ChannelType> mChannelTypes;

}; // class cedar::aux::annotation::Dimensions

#endif // CEDAR_AUX_ANNOTATION_IMAGE_H

