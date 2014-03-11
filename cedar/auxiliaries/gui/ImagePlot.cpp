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
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/ImageData.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES
#include <QHBoxLayout>
#include <QReadLocker>
#include <QWriteLocker>
#include <QToolTip>
#include <QMouseEvent>
#include <QThread>
#include <QMenu>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFrame>
#include <QDoubleSpinBox>
#include <QPushButton>
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
// static members
//----------------------------------------------------------------------------------------------------------------------

std::vector<char> cedar::aux::gui::ImagePlot::mLookupTableR;
std::vector<char> cedar::aux::gui::ImagePlot::mLookupTableG;
std::vector<char> cedar::aux::gui::ImagePlot::mLookupTableB;
QReadWriteLock cedar::aux::gui::ImagePlot::mLookupTableLock;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::gui::ImagePlot::ImagePlot(QWidget *pParent)
:
cedar::aux::gui::PlotInterface(pParent)
{
  this->construct();
}

cedar::aux::gui::ImagePlot::ImagePlot(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget *pParent)
:
cedar::aux::gui::PlotInterface(pParent)
{
  this->construct();

  this->plot(matData, title);
}

cedar::aux::gui::ImagePlot::ImageDisplay::ImageDisplay(cedar::aux::gui::ImagePlot* pPlot, const QString& text)
:
QLabel(text),
mpPlot(pPlot)
{
  this->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  this->setWordWrap(true);
}

cedar::aux::gui::detail::ImagePlotLegend::ImagePlotLegend()
{
  this->mpMin = new QLabel("min");
  this->mpMax = new QLabel("max");

  this->mpMin->setMinimumWidth(35);
  this->mpMax->setMinimumWidth(35);

  auto p_gradient = new QFrame();
  p_gradient->setSizePolicy(QSizePolicy::Fixed, p_gradient->sizePolicy().verticalPolicy());
  p_gradient->setFixedWidth(20);
  p_gradient->setFrameStyle(QFrame::Box);

  this->mGradient = QLinearGradient(0, 0, 0, 1);
  this->mGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
  cedar::aux::gui::ImagePlot::fillColorizationGradient(this->mGradient);

  QPalette palette = p_gradient->palette();
  palette.setBrush(QPalette::Window, QBrush(this->mGradient));
  p_gradient->setAutoFillBackground(true);
  p_gradient->setPalette(palette);

  auto p_layout = new QGridLayout();
  p_layout->setContentsMargins(1, 1, 1, 1);
  p_layout->addWidget(p_gradient, 0, 0, 3, 1);
  p_layout->addWidget(this->mpMax, 0, 1);
  p_layout->addWidget(this->mpMin, 2, 1);
  p_layout->setColumnStretch(1, 1);
  p_layout->setRowStretch(1, 1);
  this->setLayout(p_layout);
}

cedar::aux::gui::ImagePlot::~ImagePlot()
{
  if (this->mpWorkerThread)
  {
    this->mpWorkerThread->quit();
    this->mpWorkerThread->wait();
    delete this->mpWorkerThread;
    this->mpWorkerThread = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::ImagePlot::construct()
{
  this->mTimerId = 0;
  this->mDataType = DATA_TYPE_UNKNOWN;
  this->mConverting = false;
  this->mSmoothScaling = true;
  this->mAutoScaling = true;
  this->mpLegend = NULL;

  QHBoxLayout *p_layout = new QHBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(p_layout);
  this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  mpImageDisplay = new cedar::aux::gui::ImagePlot::ImageDisplay(this, "no image loaded");
  p_layout->addWidget(mpImageDisplay);
  mpImageDisplay->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  mpImageDisplay->setMinimumSize(QSize(50, 50));

  this->mpWorkerThread = new QThread();
  mWorker = cedar::aux::gui::detail::ImagePlotWorkerPtr(new cedar::aux::gui::detail::ImagePlotWorker(this));
  mWorker->moveToThread(this->mpWorkerThread);

  QObject::connect(this, SIGNAL(convert()), mWorker.get(), SLOT(convert()));
  QObject::connect(mWorker.get(), SIGNAL(done()), this, SLOT(conversionDone()));
  QObject::connect(mWorker.get(), SIGNAL(failed()), this, SLOT(conversionFailed()));

  this->mpWorkerThread->start(QThread::LowPriority);
}

void cedar::aux::gui::ImagePlot::setLimits(double min, double max)
{
  if (this->mpLegend)
  {
    this->mpLegend->updateMinMax(min, max);
  }
  this->mValueLimits.setLower(min);
  this->mValueLimits.setUpper(max);
  this->mAutoScaling = false;
}

//! Enables automatic scaling.
void cedar::aux::gui::ImagePlot::setAutomaticScaling()
{
  this->mAutoScaling = true;
}

void cedar::aux::gui::detail::ImagePlotLegend::updateMinMax(double min, double max)
{
  int precision = 2;
  double diff = std::abs(max - min);
  double log = std::log10(diff);
  if (log < 0)
  {
    precision = std::max(precision, static_cast<int>(cedar::aux::math::round(std::abs(log))));
  }
  this->mpMin->setText(QString("%1").arg(min, 0, 'g', precision));
  this->mpMax->setText(QString("%1").arg(max, 0, 'g', precision));
}

void cedar::aux::gui::ImagePlot::contextMenuEvent(QContextMenuEvent *pEvent)
{
  QMenu menu(this);

  int mat_type = -1;

  if (this->mData)
  {
    mat_type = this->mData->getCvType();
  }

  QAction *p_legend = menu.addAction("legend");
  p_legend->setCheckable(true);
  QObject::connect(p_legend, SIGNAL(toggled(bool)), this, SLOT(showLegend(bool)));
  p_legend->setChecked(this->mpLegend != NULL && this->mpLegend->isVisible());
  p_legend->setEnabled(mDataType == DATA_TYPE_MAT);

  auto p_smooth = menu.addAction("smooth");
  p_smooth->setCheckable(true);
  QObject::connect(p_smooth, SIGNAL(toggled(bool)), this, SLOT(setSmoothScaling(bool)));
  p_smooth->setChecked(this->mSmoothScaling);

  QMenu* p_scaling = menu.addMenu("value scaling");
  p_scaling->setEnabled(mDataType == DATA_TYPE_MAT || mat_type == CV_32F);

  auto p_auto_scale = p_scaling->addAction("automatic");
  p_auto_scale->setCheckable(true);
  p_auto_scale->setChecked(this->mAutoScaling);
  QObject::connect(p_auto_scale, SIGNAL(triggered()), this, SLOT(setAutomaticScaling()));

  auto p_fixed_scaling = p_scaling->addAction("fixed ...");
  p_fixed_scaling->setCheckable(true);
  p_fixed_scaling->setChecked(!this->mAutoScaling);
  QObject::connect(p_fixed_scaling, SIGNAL(triggered()), this, SLOT(queryFixedValueScale()));

  menu.exec(pEvent->globalPos());
}

void cedar::aux::gui::ImagePlot::queryFixedValueScale()
{
  QDialog* p_dialog = new QDialog();
  p_dialog->setModal(true);
  auto p_layout = new QGridLayout();
  p_dialog->setLayout(p_layout);
  QLabel* p_label;
  p_label = new QLabel("lower limit:");
  p_layout->addWidget(p_label, 0, 0);

  auto p_lower = new QDoubleSpinBox();
  p_layout->addWidget(p_lower, 0, 1);
  p_lower->setMinimum(boost::numeric::bounds<double>::lowest());
  p_lower->setMaximum(boost::numeric::bounds<double>::highest());
  p_lower->setValue(this->mValueLimits.getLower());

  p_label = new QLabel("upper limit:");
  p_layout->addWidget(p_label, 1, 0);

  auto p_upper = new QDoubleSpinBox();
  p_layout->addWidget(p_upper, 1, 1);
  p_upper->setMinimum(boost::numeric::bounds<double>::lowest());
  p_upper->setMaximum(boost::numeric::bounds<double>::highest());
  p_upper->setValue(this->mValueLimits.getUpper());

  auto p_buttons = new QDialogButtonBox();
  p_buttons->addButton(QDialogButtonBox::Ok);
  p_buttons->addButton(QDialogButtonBox::Cancel);
  p_layout->addWidget(p_buttons, 2, 0, 1, 2);

  QObject::connect(p_buttons->button(QDialogButtonBox::Ok), SIGNAL(clicked()), p_dialog, SLOT(accept()));
  QObject::connect(p_buttons->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), p_dialog, SLOT(reject()));

  int res = p_dialog->exec();

  if (res == QDialog::Accepted)
  {
    this->setLimits(p_lower->value(), p_upper->value());
  }
}

void cedar::aux::gui::ImagePlot::showLegend(bool show)
{
  if (this->mpLegend == NULL)
  {
    auto p_layout = cedar::aux::asserted_cast<QHBoxLayout*>(this->layout());
    this->mpLegend = new cedar::aux::gui::detail::ImagePlotLegend();
    p_layout->addWidget(this->mpLegend);
    p_layout->setStretch(0, 1);
    p_layout->setStretch(1, 0);
    QObject::connect(this->mWorker.get(), SIGNAL(minMaxChanged(double, double)), this->mpLegend, SLOT(updateMinMax(double, double)));
  }

  this->mpLegend->setVisible(show);
}

void cedar::aux::gui::ImagePlot::setSmoothScaling(bool smooth)
{
  this->mSmoothScaling = smooth;
}

void cedar::aux::gui::ImagePlot::ImageDisplay::mousePressEvent(QMouseEvent* pEvent)
{
  if (!this->pixmap() || !this->mData || pEvent->button() != Qt::LeftButton)
    return;

  QReadLocker locker(&this->mData->getLock());

  const cv::Mat& matrix = this->mData->getData();

  if (matrix.empty())
  {
    return;
  }

  int label_x = pEvent->x();
  int label_y = pEvent->y();
  int image_x = label_x - (this->width() - this->pixmap()->width()) / 2;
  int image_y = label_y - (this->height() - this->pixmap()->height()) / 2;

  double rel_x = static_cast<double>(image_x) / static_cast<double>(this->pixmap()->width());
  double rel_y = static_cast<double>(image_y) / static_cast<double>(this->pixmap()->height());
  int x = static_cast<int>(rel_x * static_cast<double>(matrix.cols));
  int y = static_cast<int>(rel_y * static_cast<double>(matrix.rows));

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
      switch(matrix.depth())
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

        default:
          QToolTip::showText(pEvent->globalPos(), QString("Matrix depth (%1) not handled.").arg(matrix.depth()));
          return;
      }
      break;

    default:
      // should never happen, all cases should be handled above
      QToolTip::showText(pEvent->globalPos(), QString("Matrix channel count (%1) not handled.").arg(matrix.channels()));
      return;
  }

  // if applicable, display color space as well
  if (this->mpPlot->mDataColorSpace)
  {
    info_text += QString(" (%1)").arg(QString::fromStdString(this->mpPlot->mDataColorSpace->getChannelCode()));
  }

  locker.unlock();

  QToolTip::showText(pEvent->globalPos(), info_text);
}

//!@cond SKIPPED_DOCUMENTATION
void cedar::aux::gui::detail::ImagePlotWorker::convert()
{
  QReadLocker read_lock(&this->mpPlot->mData->getLock());
  if (this->mpPlot->mData->getDimensionality() > 2)
  {
    this->mpPlot->mpImageDisplay->setText("cannot display matrices of dimensionality > 2");
    emit failed();
    return;
  }
  const cv::Mat& mat = this->mpPlot->mData->getData();

  if (mat.empty())
  {
    this->mpPlot->mpImageDisplay->setText("Matrix is empty.");
    emit failed();
    return;
  }
  int type = mat.type();

  switch(type)
  {
    case CV_8UC1:
    {
      cv::Mat converted = this->mpPlot->threeChannelGrayscale(this->mpPlot->mData->getData());
      read_lock.unlock();
      CEDAR_DEBUG_ASSERT(converted.type() == CV_8UC3);
      this->mpPlot->imageFromMat(converted);
      break;
    }

    case CV_8UC3:
    {

      // check if this is a HSV image
      if
      (
        this->mpPlot->mDataColorSpace
        && this->mpPlot->mDataColorSpace->getNumberOfChannels() == 3
        && this->mpPlot->mDataColorSpace->getChannelType(0) == cedar::aux::annotation::ColorSpace::Hue
        && this->mpPlot->mDataColorSpace->getChannelType(1) == cedar::aux::annotation::ColorSpace::Saturation
        && this->mpPlot->mDataColorSpace->getChannelType(2) == cedar::aux::annotation::ColorSpace::Value
      )
      {
        cv::Mat converted;
        cv::cvtColor(this->mpPlot->mData->getData(), converted, CV_HSV2BGR);
				read_lock.unlock();
        this->mpPlot->imageFromMat(converted);
      }
      else
      {
        this->mpPlot->imageFromMat(this->mpPlot->mData->getData());
				read_lock.unlock();
      }
      break;
    }

    case CV_32FC1:
    {
      // convert grayscale to three-channel matrix
      cv::Mat copy = mat.clone();
      double min, max;
      if (this->mpPlot->mAutoScaling)
      {
        cv::minMaxLoc(mat, &min, &max);
      }
      read_lock.unlock();
      cv::Mat converted = this->mpPlot->threeChannelGrayscale(copy);
      CEDAR_DEBUG_ASSERT(converted.type() == CV_8UC3);
      this->mpPlot->imageFromMat(converted);

      if (this->mpPlot->mAutoScaling)
      {
        emit minMaxChanged(min, max);
      }
      break;
    }

    default:
    {
      read_lock.unlock();
      std::string matrix_type_name = cedar::aux::math::matrixTypeToString(mat);
      QString text = "Cannot display matrix of type " + QString::fromStdString(matrix_type_name) + ".";
      this->mpPlot->mpImageDisplay->setText(text);
      emit failed();
      return;
    }
  }

  emit done();
}
//!@endcond

void cedar::aux::gui::ImagePlot::conversionDone()
{
  QReadLocker lock(&this->mImageLock);
  this->mpImageDisplay->setPixmap(QPixmap::fromImage(this->mImage));
  lock.unlock();

  this->resizePixmap();
  mConverting = false;
}

void cedar::aux::gui::ImagePlot::conversionFailed()
{
  mConverting = false;
}

void cedar::aux::gui::ImagePlot::timerEvent(QTimerEvent * /*pEvent*/)
{
  if
  (
    !this->isVisible() // plot widget is not visible -- no need to plot
    || this->mDataType == DATA_TYPE_UNKNOWN // data type cannot be plotted
    || mConverting // already converting -- skip this timer event
  )
  {
    return;
  }

  mConverting = true;
  emit convert();
}

void cedar::aux::gui::ImagePlot::fillColorizationGradient(QGradient& gradient)
{
  gradient.setColorAt(static_cast<qreal>(1.0), QColor(0, 0, 127));
  gradient.setColorAt(static_cast<qreal>(1.0 - 32.0/256.0), QColor(0, 0, 255));
  gradient.setColorAt(static_cast<qreal>(1.0 - 96.0/256.0), QColor(0, 255, 255));
  gradient.setColorAt(static_cast<qreal>(1.0 - 160.0/256.0), QColor(255, 255, 0));
  gradient.setColorAt(static_cast<qreal>(1.0 - 224.0/256.0), QColor(255, 0, 0));
  gradient.setColorAt(static_cast<qreal>(0.0), QColor(127, 0, 0));
}

cv::Mat cedar::aux::gui::ImagePlot::colorizedMatrix(cv::Mat matrix, bool limits, double min, double max)
{
  QReadLocker lookup_readlock(&mLookupTableLock);
  if (mLookupTableR.empty() || mLookupTableG.empty() || mLookupTableB.empty())
  {
    lookup_readlock.unlock();
    QWriteLocker lookup_writelock(&mLookupTableLock);
    // retest condition, might have changed since elevation of the lock
    if (mLookupTableR.empty() || mLookupTableG.empty() || mLookupTableB.empty())
    {
      const size_t steps = 256;
      const std::vector<cedar::aux::gui::ColorValueRGBA>& standard
        = cedar::aux::gui::MatrixPlot::getStandardColorVector();
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

        double r_lower = static_cast<double>(standard.at(closest_standard).red) * 255.0;
        double g_lower = static_cast<double>(standard.at(closest_standard).green) * 255.0;
        double b_lower = static_cast<double>(standard.at(closest_standard).blue) * 255.0;
        if (closest_standard + 1 < standard.size())
        {
          double r_upper = static_cast<double>(standard.at(closest_standard + 1).red);
          double g_upper = static_cast<double>(standard.at(closest_standard + 1).green);
          double b_upper = static_cast<double>(standard.at(closest_standard + 1).blue);

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
    lookup_writelock.unlock();
    lookup_readlock.relock();
  }

  // accept only char type matrices

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

cv::Mat cedar::aux::gui::ImagePlot::threeChannelGrayscale(const cv::Mat& in) const
{
  CEDAR_DEBUG_ASSERT(in.channels() == 1);

  // find min and max for scaling
  switch (this->mDataType)
  {
    default:
    case DATA_TYPE_IMAGE:
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
        cv::cvtColor(merged, converted, CV_HSV2BGR);
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
          {
            double min_val = this->mValueLimits.getLower();
            double max_val = this->mValueLimits.getUpper();
            if (this->mAutoScaling)
            {
              cv::minMaxLoc(in, &min_val, &max_val);
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

    case DATA_TYPE_MAT:
    {
      double min = 0, max = 0;
      if (this->mAutoScaling)
      {
        cv::minMaxLoc(in, &min, &max);
      }
      else
      {
        min = this->mValueLimits.getLower();
        max = this->mValueLimits.getUpper();
      }
      return cedar::aux::gui::ImagePlot::colorizedMatrix(in, !this->mAutoScaling, min, max);
    }
  }
}

void cedar::aux::gui::ImagePlot::imageFromMat(const cv::Mat& mat)
{
  QWriteLocker lock(&this->mImageLock);
  this->mImage = QImage
  (
    mat.data,
    mat.cols,
    mat.rows,
    mat.step,
    QImage::Format_RGB888
  ).rgbSwapped();
}

void cedar::aux::gui::ImagePlot::plot(cedar::aux::ConstDataPtr data, const std::string& /* title */)
{
  if (mTimerId != 0)
    this->killTimer(mTimerId);

  mDataType = DATA_TYPE_MAT;

  this->mData = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
  this->mpImageDisplay->mData = this->mData;
  if (!this->mData)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "Cannot cast to cedar::aux::MatData in cedar::aux::gui::ImagePlot::plot.");
  }

  if (this->mData->getDimensionality() > 2)
  {
    mpImageDisplay->setText("cannot display matrices of dimensionality > 2");
    // start the timer anyway
    this->mTimerId = this->startTimer(70);
    CEDAR_DEBUG_ASSERT(mTimerId != 0);
    return;
  }

  this->mDataColorSpace.reset();
  try
  {
    this->mDataColorSpace = this->mData->getAnnotation<cedar::aux::annotation::ColorSpace>();
    mDataType = DATA_TYPE_IMAGE;
  }
  catch (cedar::aux::AnnotationNotFoundException&)
  {
    // ok, not an image
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
  QReadLocker lock(&this->mImageLock);
  QSize scaled_size = this->mImage.size();
  scaled_size.scale(this->mpImageDisplay->size(), Qt::KeepAspectRatio);
  if ( (!this->mpImageDisplay->pixmap()
        || scaled_size != this->mpImageDisplay->pixmap()->size()
        )
        && !this->mImage.isNull()
      )
  {
    Qt::TransformationMode transformation_mode;
    if (this->mSmoothScaling)
    {
      transformation_mode = Qt::SmoothTransformation;
    }
    else
    {
      transformation_mode = Qt::FastTransformation;
    }

    QImage scaled_image = this->mImage.scaled(this->mpImageDisplay->size(),
                                              Qt::KeepAspectRatio,
                                              transformation_mode);
    this->mpImageDisplay->setPixmap(QPixmap::fromImage(scaled_image));
  }
}
