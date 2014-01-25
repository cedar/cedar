/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        QImagePlot.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 01 24

    Description: Source file for the class cedar::aux::gui::QImagePlot.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/QImagePlot.h"
#include "cedar/auxiliaries/MatData.h"

// SYSTEM INCLUDES
#include <QReadLocker>
#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolTip>
#include <QMenu>


//! Widget used for displaying the image.
class cedar::aux::gui::QImagePlot::ImageDisplay : public QLabel
{
public:
  ImageDisplay(cedar::aux::gui::QImagePlot* pPlot, const QString& text)
  :
  QLabel(text),
  mpPlot(pPlot)
  {
    this->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    this->setWordWrap(true);
  }

protected:
  void mousePressEvent(QMouseEvent * pEvent);

public:
  cedar::aux::ConstMatDataPtr mData;

  cedar::aux::gui::QImagePlot* mpPlot;
};

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::QImagePlot::QImagePlot(QWidget* pParent)
:
cedar::aux::gui::ThreadedPlot(pParent),
mSmoothScaling(true),
mLegendAvailable(false)
{
  this->mpLegend = NULL;

  auto p_layout = new QHBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(p_layout);
  this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  this->mpImageDisplay = new cedar::aux::gui::QImagePlot::ImageDisplay(this, "no data");
  p_layout->addWidget(mpImageDisplay);
  this->mpImageDisplay->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  this->mpImageDisplay->setMinimumSize(QSize(50, 50));

  QObject::connect
  (
    this,
    SIGNAL(minMaxChanged(double, double)),
    this,
    SLOT(updateMinMax(double, double))
  );
}

cedar::aux::gui::detail::QImagePlotLegend::QImagePlotLegend()
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
  cedar::aux::gui::QImagePlot::fillColorizationGradient(this->mGradient);

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

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::QImagePlot::updateMinMax(double min, double max)
{
  if (this->mpLegend)
  {
    this->mpLegend->updateMinMax(min, max);
  }
}

void cedar::aux::gui::QImagePlot::fillColorizationGradient(QGradient& gradient)
{
  gradient.setColorAt(static_cast<qreal>(1.0), QColor(0, 0, 127));
  gradient.setColorAt(static_cast<qreal>(1.0 - 32.0 / 256.0), QColor(0, 0, 255));
  gradient.setColorAt(static_cast<qreal>(1.0 - 96.0 / 256.0), QColor(0, 255, 255));
  gradient.setColorAt(static_cast<qreal>(1.0 - 160.0 / 256.0), QColor(255, 255, 0));
  gradient.setColorAt(static_cast<qreal>(1.0 - 224.0 / 256.0), QColor(255, 0, 0));
  gradient.setColorAt(static_cast<qreal>(0.0), QColor(127, 0, 0));
}

void cedar::aux::gui::QImagePlot::setSmoothScaling(bool smooth)
{
  this->mSmoothScaling = smooth;
}

void cedar::aux::gui::QImagePlot::contextMenuEvent(QContextMenuEvent *pEvent)
{
  QMenu menu(this);

  auto p_smooth = menu.addAction("smooth");
  p_smooth->setCheckable(true);
  QObject::connect(p_smooth, SIGNAL(toggled(bool)), this, SLOT(setSmoothScaling(bool)));
  p_smooth->setChecked(this->mSmoothScaling);

  QAction *p_legend = menu.addAction("legend");
  p_legend->setCheckable(true);
  QObject::connect(p_legend, SIGNAL(toggled(bool)), this, SLOT(showLegend(bool)));
  p_legend->setChecked(this->mpLegend != NULL && this->mpLegend->isVisible());
  p_legend->setEnabled(this->mLegendAvailable);

  this->fillContextMenu(menu);

  menu.exec(pEvent->globalPos());
}

void cedar::aux::gui::QImagePlot::resizeEvent(QResizeEvent * /*pEvent*/)
{
  this->resizePixmap();
}

void cedar::aux::gui::QImagePlot::resizePixmap()
{
  QReadLocker lock(&this->mImageLock);
  QSize scaled_size = this->mImage.size();
  scaled_size.scale(this->mpImageDisplay->size(), Qt::KeepAspectRatio);
  if ((!this->mpImageDisplay->pixmap()
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

void cedar::aux::gui::QImagePlot::setInfo(const std::string& text)
{
  this->mpImageDisplay->setText("cannot display matrices of dimensionality > 2");
}

void cedar::aux::gui::QImagePlot::updatePlot()
{
  QReadLocker lock(&this->mImageLock);
  this->mpImageDisplay->setPixmap(QPixmap::fromImage(this->mImage));
  lock.unlock();

  this->resizePixmap();
}

void cedar::aux::gui::QImagePlot::displayMatrix(const cv::Mat& matrix)
{
  QWriteLocker lock(&this->mImageLock);
  this->mImage = QImage
                 (
                   matrix.data,
                   matrix.cols,
                   matrix.rows,
                   matrix.step,
                   QImage::Format_RGB888
                 ).rgbSwapped();
}

void cedar::aux::gui::QImagePlot::ImageDisplay::mousePressEvent(QMouseEvent* pEvent)
{
  if (!this->pixmap())
    return;

  int global_x = pEvent->x();
  int global_y = pEvent->y();
  int label_relative_x = global_x - (this->width() - this->pixmap()->width()) / 2;
  int label_relative_y = global_y - (this->height() - this->pixmap()->height()) / 2;

  double image_x = static_cast<double>(label_relative_x) / static_cast<double>(this->pixmap()->width());
  double image_y = static_cast<double>(label_relative_y) / static_cast<double>(this->pixmap()->height());

  this->mpPlot->plotClicked(pEvent, image_x, image_y);
}

void cedar::aux::gui::QImagePlot::fillContextMenu(QMenu& menu)
{
}

void cedar::aux::gui::QImagePlot::plotClicked(QMouseEvent* pEvent, int imageX, int imageY)
{
}

void cedar::aux::gui::QImagePlot::showLegend(bool show)
{
  if (this->mpLegend == NULL)
  {
    auto p_layout = cedar::aux::asserted_cast<QHBoxLayout*>(this->layout());
    this->mpLegend = new cedar::aux::gui::detail::QImagePlotLegend();
    p_layout->addWidget(this->mpLegend);
    p_layout->setStretch(0, 1);
    p_layout->setStretch(1, 0);
  }

  this->mpLegend->setVisible(show);
}

void cedar::aux::gui::detail::QImagePlotLegend::updateMinMax(double min, double max)
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