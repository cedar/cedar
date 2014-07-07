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

    File:        ColorGradient.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 06 27

    Description: Source file for the class cedar::aux::ColorGradient.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/ColorGradient.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EnumType<cedar::aux::ColorGradient::StandardGradients>
  cedar::aux::ColorGradient::StandardGradients::mType("cedar::aux::ColorGradient::StandardGradients::");

#ifndef CEDAR_COMPILER_MSVC
const cedar::aux::ColorGradient::StandardGradients::Id cedar::aux::ColorGradient::StandardGradients::PlotDefault;
const cedar::aux::ColorGradient::StandardGradients::Id cedar::aux::ColorGradient::StandardGradients::Gray;
const cedar::aux::ColorGradient::StandardGradients::Id cedar::aux::ColorGradient::StandardGradients::GrayInverse;
#endif // CEDAR_COMPILER_MSVC

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::ColorGradient::StandardGradients::construct()
{
  mType.type()->def(cedar::aux::Enum(cedar::aux::ColorGradient::StandardGradients::PlotDefault, "PlotDefault", "Default Plot Gradient"));
  mType.type()->def(cedar::aux::Enum(cedar::aux::ColorGradient::StandardGradients::Gray, "Gray", "Grayscale Gradient"));
  mType.type()->def(cedar::aux::Enum(cedar::aux::ColorGradient::StandardGradients::GrayInverse, "GrayInverse", "Inverse grayscale Gradient"));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::ColorGradientPtr cedar::aux::ColorGradient::getStandardGradient(const cedar::aux::Enum& id)
{
  switch (id.id())
  {
    default:
    case cedar::aux::ColorGradient::StandardGradients::PlotDefault:
      return cedar::aux::ColorGradient::getDefaultPlotColorJet();

    case cedar::aux::ColorGradient::StandardGradients::Gray:
      return cedar::aux::ColorGradient::getPlotGrayColorJet();

    case cedar::aux::ColorGradient::StandardGradients::GrayInverse:
      return cedar::aux::ColorGradient::getPlotInverseGrayColorJet();
  }
}

const cedar::aux::EnumBase& cedar::aux::ColorGradient::StandardGradients::type()
{
  return *cedar::aux::ColorGradient::StandardGradients::mType.type();
}

const cedar::aux::ColorGradient::StandardGradients::TypePtr& cedar::aux::ColorGradient::StandardGradients::typePtr()
{
  return cedar::aux::ColorGradient::StandardGradients::mType.type();
}

const std::map<double, QColor>& cedar::aux::ColorGradient::getStops() const
{
  return this->mGradientColors;
}

cedar::aux::ColorGradientPtr cedar::aux::ColorGradient::getDefaultPlotColorJet()
{
  static cedar::aux::ColorGradientPtr gradient;
  //!@todo Locking?
  if (!gradient)
  {
    gradient = cedar::aux::ColorGradientPtr(new cedar::aux::ColorGradient());

    gradient->setStop(0.000, QColor::fromRgbF(0.0, 0.0, 0.5));
    gradient->setStop(0.125, QColor::fromRgbF(0.0, 0.0, 1.0));
    gradient->setStop(0.375, QColor::fromRgbF(0.0, 1.0, 1.0));
    gradient->setStop(0.625, QColor::fromRgbF(1.0, 1.0, 0.0));
    gradient->setStop(0.875, QColor::fromRgbF(1.0, 0.0, 0.0));
    gradient->setStop(1.000, QColor::fromRgbF(0.5, 0.0, 0.0));
  }

  return gradient;
}

cedar::aux::ColorGradientPtr cedar::aux::ColorGradient::getPlotGrayColorJet()
{
  static cedar::aux::ColorGradientPtr gradient;
  //!@todo Locking?
  if (!gradient)
  {
    gradient = cedar::aux::ColorGradientPtr(new cedar::aux::ColorGradient());

    gradient->setStop(0.000, QColor::fromRgbF(0.0, 0.0, 0.0));
    gradient->setStop(1.000, QColor::fromRgbF(1.0, 1.0, 1.0));
  }

  return gradient;
}

cedar::aux::ColorGradientPtr cedar::aux::ColorGradient::getPlotInverseGrayColorJet()
{
  static cedar::aux::ColorGradientPtr gradient;
  //!@todo Locking?
  if (!gradient)
  {
    gradient = cedar::aux::ColorGradientPtr(new cedar::aux::ColorGradient());

    gradient->setStop(0.000, QColor::fromRgbF(1.0, 1.0, 1.0));
    gradient->setStop(1.000, QColor::fromRgbF(0.0, 0.0, 0.0));
  }

  return gradient;
}

void cedar::aux::ColorGradient::setStop(double location, const QColor& color)
{
  CEDAR_NON_CRITICAL_ASSERT(location >= 0.0);
  CEDAR_NON_CRITICAL_ASSERT(location <= 1.0);
  this->mGradientColors[location] = color;
  this->updateLookupTable();
}

void cedar::aux::ColorGradient::updateLookupTable()
{
  mLookupTableR.clear();
  mLookupTableG.clear();
  mLookupTableB.clear();

  mLookupTableR.resize(256);
  mLookupTableG.resize(256);
  mLookupTableB.resize(256);

  for (size_t gray = 0; gray < mLookupTableR.size(); ++ gray)
  {
    QColor color;

    if (this->mGradientColors.size() == 0)
    {
      color = QColor(0, 0, 0);
    }
    else if (this->mGradientColors.size() == 1)
    {
      color = this->mGradientColors.begin()->second;
    }
    else
    {
      double gray_f = static_cast<double>(gray) / static_cast<double>(mLookupTableR.size());

      // The first element in the map with a key >= gray_f
      auto upper_it = this->mGradientColors.lower_bound(gray_f);

      if (upper_it == this->mGradientColors.begin())
      {
        color = upper_it->second;
      }
      else if (upper_it == this->mGradientColors.end())
      {
        upper_it--;
        color = upper_it->second;
      }
      else
      {
        auto lower_it = upper_it;
        lower_it--;

        double lower = lower_it->first;
        double upper = upper_it->first;

        const QColor& lower_col = lower_it->second;
        const QColor& upper_col = upper_it->second;

        double mix = 0.0;
        if (lower < upper)
        {
          mix = (gray_f - lower) / (upper - lower);
        }
        else
        {
          std::cout << lower << " >= " << upper << " for " << gray_f << std::endl;
        }
        color.setRed((1.0 - mix) * lower_col.red() + mix * upper_col.red());
        color.setBlue((1.0 - mix) * lower_col.blue() + mix * upper_col.blue());
        color.setGreen((1.0 - mix) * lower_col.green() + mix * upper_col.green());
      }
    }

    this->mLookupTableR[gray] = static_cast<char>(color.red());
    this->mLookupTableG[gray] = static_cast<char>(color.green());
    this->mLookupTableB[gray] = static_cast<char>(color.blue());
  }
}

cv::Mat cedar::aux::ColorGradient::applyTo(const cv::Mat& matrix, bool limits, double min, double max)
{
  int channels = matrix.channels();

  int rows = matrix.rows * channels;
  int cols = matrix.cols;

  if (matrix.isContinuous())
  {
    cols *= rows;
    rows = 1;
  }

  cv::Mat in_converted;

  if (matrix.type() != CV_8UC1)
  {
    switch (matrix.type())
    {
      case CV_32F:
      {
        cv::Mat scaled = (matrix - min) / (max - min) * 255.0;
        scaled.convertTo(in_converted, CV_8UC1);
        break;
      }
      default:
        matrix.convertTo(in_converted, CV_8UC1);
        break;
    }
  }
  else
  {
    in_converted = matrix;
  }

  cv::Mat converted = cv::Mat(matrix.rows, matrix.cols, CV_8UC3);

  const unsigned char* p_in;
  unsigned char* p_converted;
  for (int i = 0; i < rows; ++i)
  {
    p_in = in_converted.ptr<unsigned char>(i);
    p_converted = converted.ptr<unsigned char>(i);
    for (int j = 0; j < cols; ++j)
    {
      size_t v = static_cast<size_t>(p_in[j]);

      CEDAR_DEBUG_ASSERT(v < mLookupTableB.size());
      CEDAR_DEBUG_ASSERT(v < mLookupTableG.size());
      CEDAR_DEBUG_ASSERT(v < mLookupTableR.size());

      // channel 0
      p_converted[3 * j + 0] = mLookupTableB.at(v);
      // channel 1
      p_converted[3 * j + 1] = mLookupTableG.at(v);
      // channel 2
      p_converted[3 * j + 2] = mLookupTableR.at(v);
    }
  }

  if (limits)
  {
    cv::Mat min_vals = (matrix < min);
    cv::Mat max_vals = (matrix > max);
    converted.setTo(0xFFFFFF, min_vals | max_vals);
  }

  return converted;
}
