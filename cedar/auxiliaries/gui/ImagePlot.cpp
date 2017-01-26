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

    File:        MatrixPlot.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 14

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/ImagePlot.h"
#include "cedar/auxiliaries/gui/MatrixPlot.h" // for the color map
#include "cedar/auxiliaries/gui/ColorValueRGBA.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.h"
#include "cedar/auxiliaries/annotation/ColorSpace.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/ColorGradient.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES
#include <QHBoxLayout>
#include <QReadLocker>
#include <QWriteLocker>
#include <QToolTip>
#include <QMouseEvent>
#include <QThread>
#include <QMenu>
#include <QFrame>
#include <QLinearGradient>
#include <QPalette>
#include <iostream>
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registerPlot()
  {
    typedef cedar::aux::gui::PlotDeclarationTemplate<cedar::aux::MatData, cedar::aux::gui::ImagePlot> DeclarationType;
    boost::shared_ptr<DeclarationType> declaration(new DeclarationType());
    declaration->declare();

    return true;
  }

  bool registered = registerPlot();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::gui::ImagePlot::ImagePlot(QWidget *pParent)
:
cedar::aux::gui::QImagePlot(pParent)
{
  this->construct();
}

cedar::aux::gui::ImagePlot::ImagePlot(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget *pParent)
:
cedar::aux::gui::QImagePlot(pParent)
{
  this->construct();

  this->plot(matData, title);
}

cedar::aux::gui::ImagePlot::~ImagePlot()
{
  this->stop();
  this->wait();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::ImagePlot::construct()
{
  this->mDataType = DATA_TYPE_UNKNOWN;

  this->setValueScalingEnabled(false);
}

void cedar::aux::gui::ImagePlot::plotClicked(QMouseEvent* pEvent, double relativeImageX, double relativeImageY)
{
  if (!this->mData || pEvent->button() != Qt::LeftButton)
    return;

  QReadLocker locker(&this->mData->getLock());

  const cv::Mat& matrix = this->mData->getData();

  if (matrix.empty())
  {
    return;
  }
  
  int x = static_cast<int>(relativeImageX * static_cast<double>(matrix.cols));
  int y = static_cast<int>(relativeImageY * static_cast<double>(matrix.rows));

  if (x < 0 || y < 0 || x >= matrix.cols || y >= matrix.rows)
  {
    return;
  }

  QString info_text;
  info_text += QString("x, y = %1, %2").arg(x).arg(y);

  info_text += "<br />value: ";
  switch (matrix.channels())
  {
  case 1:
    info_text += QString("%1").arg(cedar::aux::math::getMatrixEntry<double>(matrix, y, x));
    break;

  case 3:
  {
    switch (matrix.depth())
    {
    case CV_8U:
    {
                const cv::Vec3b& value = matrix.at<cv::Vec3b>(y, x);
                info_text += QString("%1, %2, %3").arg(static_cast<int>(value[0]))
                  .arg(static_cast<int>(value[1]))
                  .arg(static_cast<int>(value[2]));
                break;
    }

    case CV_8S:
    {
                const cv::Vec3s& value = matrix.at<cv::Vec3s>(y, x);
                info_text += QString("%1, %2, %3").arg(static_cast<int>(value[0]))
                  .arg(static_cast<int>(value[1]))
                  .arg(static_cast<int>(value[2]));
                break;
    }

    case CV_32F:
    {
      const cv::Vec3s& value = matrix.at<cv::Vec3f>(y, x);
      info_text += QString("%1, %2, %3").arg(static_cast<float>(value[0]))
        .arg(static_cast<float>(value[1]))
        .arg(static_cast<float>(value[2]));
      break;
    }

    default:
      QToolTip::showText(pEvent->globalPos(), QString("Matrix depth (%1) not handled.").arg(matrix.depth()));
      return;
    }
    break;
  } // case: three channels

  default:
    // should never happen, all cases should be handled above
    QToolTip::showText(pEvent->globalPos(), QString("Matrix channel count (%1) not handled.").arg(matrix.channels()));
    return;
  }

  // if applicable, display color space as well
  if (this->mDataColorSpace)
  {
    info_text += QString(" (%1)").arg(QString::fromStdString(this->mDataColorSpace->getChannelCode()));
  }

  locker.unlock();

  QToolTip::showText(pEvent->globalPos(), info_text);
}

//!@cond SKIPPED_DOCUMENTATION
bool cedar::aux::gui::ImagePlot::doConversion()
{
  QReadLocker read_lock(&this->mData->getLock());

  if (this->mDataType == DATA_TYPE_UNKNOWN)
  {
    return false;
  }

  if (this->mData->getDimensionality() > 2)
  {
    this->setInfo("cannot display matrices of dimensionality > 2");
    return false;
  }

  const cv::Mat& mat = this->mData->getData();

  if (mat.empty())
  {
    this->setInfo("Matrix is empty.");
    return false;
  }
  int type = mat.type();

  switch(type)
  {
    case CV_16UC1:
    case CV_8UC1:
    {
      cv::Mat copy = this->mData->getData().clone();
      read_lock.unlock();
      cv::Mat converted = this->threeChannelGrayscale(copy);
      CEDAR_DEBUG_ASSERT(converted.type() == CV_8UC3);
      this->displayMatrix(converted);
      break;
    }

    case CV_8UC3:
    {

      // check if this is a HSV image
      if
      (
        this->mDataColorSpace
        && this->mDataColorSpace->getNumberOfChannels() == 3
        && this->mDataColorSpace->getChannelType(0) == cedar::aux::annotation::ColorSpace::Hue
        && this->mDataColorSpace->getChannelType(1) == cedar::aux::annotation::ColorSpace::Saturation
        && this->mDataColorSpace->getChannelType(2) == cedar::aux::annotation::ColorSpace::Value
      )
      {
        cv::Mat converted;
        cv::cvtColor
        (
          this->mData->getData(),
          converted,
#if CEDAR_OPENCV_MAJOR_VERSION >= 3
          cv::COLOR_HSV2BGR
#else
          CV_HSV2BGR
#endif
        );
				read_lock.unlock();
        this->displayMatrix(converted);
      }
      else
      {
        this->displayMatrix(this->mData->getData());
				read_lock.unlock();
      }
      break;
    }

    case CV_32FC1:
    case CV_64FC1:
    {
      // convert grayscale to three-channel matrix
      cv::Mat copy = mat.clone();
      read_lock.unlock();
      cv::Mat converted = this->threeChannelGrayscale(copy);
      CEDAR_DEBUG_ASSERT(converted.type() == CV_8UC3);
      this->displayMatrix(converted);
      break;
    }

    case CV_32FC3:
    {
      // convert grayscale to three-channel matrix
      cv::Mat copy = mat.clone();
      read_lock.unlock();
      cv::Mat channels[3];
      double min_all = std::numeric_limits<double>::max(), max_all = -std::numeric_limits<double>::max();
      cv::split(copy, channels);
      for (size_t c = 0; c < 3; ++c)
      {
        double min, max;
        cv::minMaxLoc(channels[c], &min, &max);
        if (min < min_all)
        {
          min_all = min;
        }
        if (max > max_all)
        {
          max_all = max;
        }
      }
      cv::Mat converted;
      copy = (copy - min_all) / (max_all - min_all);
      copy.convertTo(converted, CV_8UC3, 255.0);
      CEDAR_DEBUG_ASSERT(converted.type() == CV_8UC3);
      this->displayMatrix(converted);
      break;
    }

    default:
    {
      read_lock.unlock();
      std::string matrix_type_name = cedar::aux::math::matrixTypeToString(mat);
      this->setInfo("Cannot display matrix of type " + matrix_type_name + ".");
      return false;
    }
  }

  return true;
}
//!@endcond

cv::Mat cedar::aux::gui::ImagePlot::threeChannelGrayscale(const cv::Mat& in)
{
  CEDAR_DEBUG_ASSERT(in.channels() == 1);

  // find min and max for scaling
  switch (this->mDataType)
  {
    default:
    case DATA_TYPE_IMAGE:
    {
      if (this->getColorJet() != cedar::aux::ColorGradient::StandardGradients::PlotDefault)
      {
        return this->colorizeMatrix(in);
      }
      else
      {
        cedar::aux::annotation::ColorSpace::ChannelType type = cedar::aux::annotation::ColorSpace::Gray;

        if (this->mDataColorSpace)
        {
          type = this->mDataColorSpace->getChannelType(0);
        }

        if (type == cedar::aux::annotation::ColorSpace::Hue)
        {
          cv::Mat ones = 0.0 * in + 255.0;
          std::vector<cv::Mat> merge_vec;
          merge_vec.push_back(in);
          merge_vec.push_back(ones);
          merge_vec.push_back(ones);
          cv::Mat merged, converted;
          cv::merge(merge_vec, merged);
          cv::cvtColor
          (
            merged,
            converted,
#if CEDAR_OPENCV_MAJOR_VERSION >= 3
            cv::COLOR_HSV2BGR
#else
            CV_HSV2BGR
#endif
          );
          cv::Mat converted_8uc3;
          converted.convertTo(converted_8uc3, CV_8UC3);
          return converted_8uc3;
        }
        else
        {
          std::vector<cv::Mat> merge_vec;
          cv::Mat zeros = 0.0 * in;

          cv::Mat in_scaled;
          switch (in.type())
          {
            case CV_32F:
            case CV_64F:
            {
              double min_val = this->getValueLimits().getLower();
              double max_val = this->getValueLimits().getUpper();
              if (this->isAutoScaling())
              {
                cv::minMaxLoc(in, &min_val, &max_val);
                emit minMaxChanged(min_val, max_val);
              }
              if (min_val != max_val)
              {
                in_scaled = 255.0 * (in - min_val) / (max_val - min_val);
              }
              else
              {
                in_scaled = in;
              }
              break;
            }
            default:
              in_scaled = in;
              break;
          }

          switch (type)
          {
            case cedar::aux::annotation::ColorSpace::Red:
              merge_vec.push_back(zeros);
              merge_vec.push_back(zeros);
              merge_vec.push_back(in_scaled);
              break;

            case cedar::aux::annotation::ColorSpace::Green:
              merge_vec.push_back(zeros);
              merge_vec.push_back(in_scaled);
              merge_vec.push_back(zeros);
              break;

            case cedar::aux::annotation::ColorSpace::Blue:
              merge_vec.push_back(in_scaled);
              merge_vec.push_back(zeros);
              merge_vec.push_back(zeros);
              break;

            default:
            case cedar::aux::annotation::ColorSpace::Gray:
              merge_vec.push_back(in_scaled);
              merge_vec.push_back(in_scaled);
              merge_vec.push_back(in_scaled);
              break;
          }

          cv::Mat converted;
          cv::merge(merge_vec, converted);
          cv::Mat converted_8uc3;
          converted.convertTo(converted_8uc3, CV_8UC3);
          return converted_8uc3;
        }
      }
    }

    case DATA_TYPE_MAT:
    {
      return this->colorizeMatrix(in);
    }
  }
}

void cedar::aux::gui::ImagePlot::plot(cedar::aux::ConstDataPtr data, const std::string& title)
{
  this->stop();
  this->cedar::aux::gui::QImagePlot::plot(data, title);

  this->mDataType = DATA_TYPE_MAT;
  this->setLegendAvailable(true);

  this->mData = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
  // this->mpImageDisplay->mData = this->mData;
  if (!this->mData)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "Cannot cast to cedar::aux::MatData in cedar::aux::gui::ImagePlot::plot.");
  }

  if (this->mData->getDimensionality() > 2)
  {
    this->setInfo("cannot display matrices of dimensionality > 2");
    this->start();
    return;
  }

  this->mDataColorSpace.reset();
  if (this->mData->hasAnnotation<cedar::aux::annotation::ColorSpace>())
  {
    this->mDataColorSpace = this->mData->getAnnotation<cedar::aux::annotation::ColorSpace>();
    mDataType = DATA_TYPE_IMAGE;
    this->setLegendAvailable(false);
  }

  this->setInfo("no image loaded");

  this->setValueScalingEnabled(mDataType == DATA_TYPE_MAT || this->mData->getData().type() == CV_32F);

  if (!this->mData->getData().empty())
  {
    this->setInfo("");
    this->start();
  }
}
