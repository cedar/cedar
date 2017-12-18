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

    File:        ColorValueRGBA.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2013 10 25

    Description: A color value in RGBA (red, green, blue, alpha).

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_COLOR_VALUE_RGBA_H
#define CEDAR_AUX_GUI_COLOR_VALUE_RGBA_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/ColorValueRGBA.fwd.h"

// SYSTEM INCLUDES


//!@brief A color value in RGBA (red, green, blue, alpha).
class cedar::aux::gui::ColorValueRGBA
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructor
  //--------------------------------------------------------------------------------------------------------------------
  //!@brief The standard constructor.
  ColorValueRGBA()
  {
  }

  //!@brief Constructor that initializes RGB values.
  ColorValueRGBA(double r, double g, double b, double a = 1.0)
  {
    red = r;
    green = g;
    blue = b;
    alpha = a;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public members
  //--------------------------------------------------------------------------------------------------------------------
  //!@brief the red value
  double red;

  //!@brief the gren value
  double green;

  //!@brief the blue value
  double blue;

  //!@brief the alpha value
  double alpha;
}; // class cedar::aux::gui::ColorValueRGBA

#endif // CEDAR_AUX_GUI_COLOR_VALUE_RGBA_H

