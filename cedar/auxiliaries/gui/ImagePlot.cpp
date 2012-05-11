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
#include "cedar/auxiliaries/gui/PlotManager.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/ImageData.h"

// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registerPlot()
  {
    typedef cedar::aux::gui::PlotDeclarationTemplate<cedar::aux::MatData, cedar::aux::gui::ImagePlot> DeclarationTypeM;
    boost::shared_ptr<DeclarationTypeM> declaration(new DeclarationTypeM());
    cedar::aux::gui::PlotManagerSingleton::getInstance()->declare(declaration);

    cedar::aux::gui::PlotManagerSingleton::getInstance()->setDefault<cedar::aux::ImageData, cedar::aux::gui::ImagePlot>();

    return true;
  }

  bool registered = registerPlot();
}

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

std::vector<char> cedar::aux::gui::ImagePlot::mLookupTableR;
std::vector<char> cedar::aux::gui::ImagePlot::mLookupTableG;
std::vector<char> cedar::aux::gui::ImagePlot::mLookupTableB;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::gui::ImagePlot::ImagePlot(QWidget *pParent)
:
cedar::aux::gui::PlotInterface(pParent),
mTimerId(0),
mDataType(DATA_TYPE_UNKNOWN)
{
  QVBoxLayout *p_layout = new QVBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(p_layout);
  this->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

  mpImageDisplay = new QLabel("no image loaded");
  p_layout->addWidget(mpImageDisplay);
  mpImageDisplay->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  mpImageDisplay->setWordWrap(true);
}

cedar::aux::gui::ImagePlot::~ImagePlot()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::aux::gui::ImagePlot::timerEvent(QTimerEvent * /*pEvent*/)
{
  if (!this->isVisible())
  {
    return;
  }

  if (this->mDataType == DATA_TYPE_UNKNOWN)
  {
    return;
  }

  cv::Mat& mat = this->mData->getData();

  this->mData->lockForRead();
  if (mat.empty())
  {
    this->mpImageDisplay->setText("Matrix is empty.");
    this->mData->unlock();
    return;
  }
  int type = mat.type();
  this->mData->unlock();

  switch(type)
  {
    case CV_8UC1:
    {
      this->mData->lockForRead();
      cv::Mat converted = threeChannelGrayscale(mat);
      this->mData->unlock();
      CEDAR_DEBUG_ASSERT(converted.type() == CV_8UC3);
      this->imageFromMat(converted);
      break;
    }

    case CV_8UC3:
    {
      this->mData->lockForRead();
      this->imageFromMat(mat);
      this->mData->unlock();
      break;
    }

    case CV_32FC1:
    {
      //!@todo This should color-code when the data is not an image (i.e., a matrix)
      //!@todo Some code here is redundant
      // find min and max for scaling
      double min, max;

      this->mData->lockForRead();
      cv::minMaxLoc(mat, &min, &max);
      cv::Mat scaled = (mat - min) / (max - min) * 255.0;
      this->mData->unlock();
      cv::Mat mat_8u;
      scaled.convertTo(mat_8u, CV_8U);

      // convert grayscale to three-channel matrix
      cv::Mat converted = threeChannelGrayscale(mat_8u);
      CEDAR_DEBUG_ASSERT(converted.type() == CV_8UC3);
      this->imageFromMat(converted);
      break;
    }

    case CV_32FC3:
      this->mpImageDisplay->setText("Cannot display CV_32FC3 matrix.");
      return;

    default:
      QString text = QString("Unhandled matrix type %1.").arg(mat.type());
      this->mpImageDisplay->setText(text);
      return;
  }

  this->mpImageDisplay->setPixmap(QPixmap::fromImage(this->mImage));
  this->resizePixmap();
}

//!@todo encapsulate lookup table functionality in own class
cv::Mat cedar::aux::gui::ImagePlot::threeChannelGrayscale(const cv::Mat& in) const
{
  if (mLookupTableR.empty() || mLookupTableG.empty() || mLookupTableB.empty())
  {
    const size_t steps = 256;
    const Qwt3D::ColorVector& standard = cedar::aux::gui::MatrixPlot::getStandardColorVector();
    mLookupTableR.resize(steps, 0);
    mLookupTableG.resize(steps, 0);
    mLookupTableB.resize(steps, 0);

    for (size_t v = 0; v < steps; ++v)
    {
      char& r = mLookupTableR.at(v);
      char& g = mLookupTableG.at(v);
      char& b = mLookupTableB.at(v);

      size_t closest_standard = (v * standard.size()) / steps;
      size_t lower_closest = (closest_standard * steps) / standard.size();
      size_t upper_closest = ((closest_standard + 1) * steps) / standard.size();

      CEDAR_DEBUG_ASSERT(closest_standard < standard.size());

      double r_lower = static_cast<double>(standard.at(closest_standard).r) * 255.0;
      double g_lower = static_cast<double>(standard.at(closest_standard).g) * 255.0;
      double b_lower = static_cast<double>(standard.at(closest_standard).b) * 255.0;
      if (closest_standard + 1 < standard.size())
      {
        double r_upper = static_cast<double>(standard.at(closest_standard + 1).r);
        double g_upper = static_cast<double>(standard.at(closest_standard + 1).g);
        double b_upper = static_cast<double>(standard.at(closest_standard + 1).b);

        double factor = static_cast<double>(v - lower_closest) / static_cast<double>(upper_closest - lower_closest);

        double d_r = (1.0 - factor) * r_lower + factor * r_upper;
        double d_g = (1.0 - factor) * g_lower + factor * g_upper;
        double d_b = (1.0 - factor) * b_lower + factor * b_upper;
        r = static_cast<char>(d_r);
        g = static_cast<char>(d_g);
        b = static_cast<char>(d_b);
      }
      else
      {
        r = r_lower;
        g = g_lower;
        b = b_lower;
      }
    }
  }

  switch (this->mDataType)
  {
    default:
    case DATA_TYPE_IMAGE:
    {
      cv::Mat converted;
      std::vector<cv::Mat> merge_vec;
      merge_vec.push_back(in);
      merge_vec.push_back(in);
      merge_vec.push_back(in);
      cv::merge(merge_vec, converted);
      return converted;
    }

    case DATA_TYPE_MAT:
    {
      // accept only char type matrices
      CEDAR_ASSERT(in.depth() != sizeof(char));

      int channels = in.channels();

      int rows = in.rows * channels;
      int cols = in.cols;

      if (in.isContinuous())
      {
          cols *= rows;
          rows = 1;
      }

      cv::Mat converted = cv::Mat(in.rows, in.cols, CV_8UC3);

      int i,j;
      const unsigned char* p_in;
      unsigned char* p_converted;
      for( i = 0; i < rows; ++i)
      {
          p_in = in.ptr<unsigned char>(i);
          p_converted = converted.ptr<unsigned char>(i);
          for ( j = 0; j < cols; ++j)
          {
            size_t v = static_cast<size_t>(p_in[j]);
            // channel 0
            p_converted[3 * j + 0] = mLookupTableB.at(v);
            // channel 1
            p_converted[3 * j + 1] = mLookupTableG.at(v);
            // channel 2
            p_converted[3 * j + 2] = mLookupTableR.at(v);
          }
      }
      return converted;
    }
  }
}

void cedar::aux::gui::ImagePlot::imageFromMat(const cv::Mat& mat)
{
  this->mImage = QImage
  (
    mat.data,
    mat.cols,
    mat.rows,
    mat.step,
    QImage::Format_RGB888
  ).rgbSwapped();
}

void cedar::aux::gui::ImagePlot::plot(cedar::aux::DataPtr data, const std::string& /* title */)
{
  if (mTimerId != 0)
    this->killTimer(mTimerId);

  mDataType = DATA_TYPE_MAT;

  this->mData = boost::dynamic_pointer_cast<cedar::aux::MatData>(data);
  if (!this->mData)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "Cannot cast to cedar::aux::MatData in cedar::aux::gui::ImagePlot::display.");
  }

  if (boost::dynamic_pointer_cast<cedar::aux::ImageData>(data))
  {
    mDataType = DATA_TYPE_IMAGE;
  }

  mpImageDisplay->setText("no image loaded");

  if (!this->mData->getData().empty())
  {
    mpImageDisplay->setText("");
    this->mTimerId = this->startTimer(70);
    CEDAR_DEBUG_ASSERT(mTimerId != 0);
  }
}

void cedar::aux::gui::ImagePlot::resizeEvent(QResizeEvent * /*pEvent*/)
{
  this->resizePixmap();
}

void cedar::aux::gui::ImagePlot::resizePixmap()
{
  QSize scaled_size = this->mImage.size();
  scaled_size.scale(this->mpImageDisplay->size(), Qt::KeepAspectRatio);
  if ( (!this->mpImageDisplay->pixmap()
        || scaled_size != this->mpImageDisplay->pixmap()->size()
        )
        && !this->mImage.isNull()
      )
  {
    QImage scaled_image = this->mImage.scaled(this->mpImageDisplay->size(),
                                              Qt::KeepAspectRatio,
                                              Qt::SmoothTransformation);
    this->mpImageDisplay->setPixmap(QPixmap::fromImage(scaled_image));
  }
}
