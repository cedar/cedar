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

    File:        ColorGradient.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 06 27

    Description: Header file for the class cedar::aux::ColorGradient.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_COLOR_GRADIENT_H
#define CEDAR_AUX_COLOR_GRADIENT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/EnumType.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/ColorGradient.fwd.h"

// SYSTEM INCLUDES
#ifdef CEDAR_USE_QWTPLOT3D
  #include <qwtplot3d/qwt3d_color_std.h>
#endif // CEDAR_USE_QWTPLOT3D
#include <opencv2/opencv.hpp>
#include <QColor>
#include <map>
#include <vector>


/*!@brief Represents a gradient of colors.
 *
 * Can be used to colorize grayscale matrices.
 */
class cedar::aux::ColorGradient
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! an enum for standard gradients
  class StandardGradients
  {
  public:
    //! Type of the enum.
    typedef cedar::aux::EnumId Id;

    //! Pointer to the enumeration type.
    typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

    //! Constructs the enumeration values.
    static void construct();

    //! Returns the enum base class.
    static const cedar::aux::EnumBase& type();

    //! Returns a pointer to the enum base class.
    static const TypePtr& typePtr();


    //! Default color gradient for plots.
    static const Id PlotDefault = 0;

    //! Grayscale gradient.
    static const Id Gray = 1;

    //! Inverted grayscale gradient.
    static const Id GrayInverse = 2;

  private:
    static cedar::aux::EnumType<cedar::aux::ColorGradient::StandardGradients> mType;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! set a color at a specific location along the gradient
  void setStop(double location, const QColor& color);

  //! applies the color gradient to a matrix
  cv::Mat applyTo(const cv::Mat& matrix, bool limits = false, double min = 0.0, double max = 1.0);

  //! get a map of all color stops along the gradient
  const std::map<double, QColor>& getStops() const;

  //! Checks if the gradient is empty.
  bool empty() const;

#ifdef CEDAR_USE_QWTPLOT3D
  Qwt3D::StandardColor toQwt3DStandardColor(size_t steps) const;
#endif // CEDAR_USE_QWTPLOT3D

  //! Returns the (interpolated) color for the given position.
  QColor getColor(double position) const;

  //! get the default gradient color jet
  static ColorGradientPtr getDefaultPlotColorJet();

  //! get a gray scale gradient color jet
  static ColorGradientPtr getPlotGrayColorJet();

  //! get an inverse gray scale gradient color jet
  static ColorGradientPtr getPlotInverseGrayColorJet();

  //! get a gradient from enum
  static ColorGradientPtr getStandardGradient(const cedar::aux::Enum& id);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void updateLookupTable();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Lookup table for red.
  std::vector<char> mLookupTableR;
  //! Lookup table for green.
  std::vector<char> mLookupTableG;
  //! Lookup table for blue.
  std::vector<char> mLookupTableB;

  //! Colors to be applied along with their locations.
  std::map<double, QColor> mGradientColors;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::ColorGradient

#endif // CEDAR_AUX_COLOR_GRADIENT_H

