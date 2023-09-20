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

    File:        QCImageDisplay.cpp

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2023 09 18

    Description: Source file for the class cedar::aux::gui::QCImageDisplay.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/auxiliaries/gui/QCImageDisplay.h"

// CEDAR INCLUDES

// SYSTEM INCLUDES

class cedar::aux::gui::QCImageDisplay::ColorMap : public QCPColorMap{
public:
  ColorMap(QImagePlot* pPlot, QCustomPlot* pChart, QCPAxis *keyAxis, QCPAxis *valueAxis)
    :
    QCPColorMap(keyAxis, valueAxis),
    mpPlot(pPlot),
    mpChart(pChart)
  {}

  void mousePressEvent (QMouseEvent *pEvent, const QVariant &details)
  {
    int global_x = pEvent->x();
    int global_y = pEvent->y();
    int label_relative_x = global_x - this->mpChart->axisRect()->topLeft().x();
    int label_relative_y = global_y - this->mpChart->axisRect()->topLeft().y();
    double image_x = static_cast<double>(label_relative_x) / static_cast<double>(this->mpChart->axisRect()->width());
    double image_y = static_cast<double>(label_relative_y) / static_cast<double>(this->mpChart->axisRect()->height());
    this->mpPlot->plotClicked(pEvent, image_x, image_y);
  }

private:
  QImagePlot* mpPlot;
  QCustomPlot* mpChart;
};

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::QCImageDisplay::QCImageDisplay(cedar::aux::gui::QImagePlot* pPlot)
:
mpPlot(pPlot),
mNan(std::nan(""))
{
  this->mpChart = new QCustomPlot();
  this->mpChart->setContextMenuPolicy(Qt::CustomContextMenu);
  QObject::connect(this->mpChart, SIGNAL(customContextMenuRequested(QPoint)), this->mpPlot, SLOT(showContextMenu(QPoint)));

  this->mpChart->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  this->mpChart->setMinimumSize(QSize(5, 5));
  this->mpChart->addGraph();
  this->mpChart->axisRect()->setupFullAxesBox(true);
  this->mpColorMap = new ColorMap(this->mpPlot, this->mpChart, this->mpChart->xAxis, this->mpChart->yAxis);
  this->mpColorMap->setDataRange(QCPRange(0, 255));
  this->mpChart->yAxis->setRangeReversed(true);
  this->mpChart->yAxis2->setRangeReversed(true);

  this->mpInfoLabel = new QCPItemText(this->mpChart);
  this->mpInfoLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
  this->mpInfoLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
  this->mpInfoLabel->position->setCoords(0.5, 0.5);
  this->mpInfoLabel->setText("");
  this->mpInfoLabel->setFont(QFont(this->mpPlot->font().family(), 12));
}

cedar::aux::gui::QCImageDisplay::~QCImageDisplay()
{
  delete this->mpInfoLabel;
  delete this->mpColorMap;
  delete this->mpChart;
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::QCImageDisplay::setText(const QString & text)
{
  if(this->mpInfoLabel->text().compare(text))
  {
    this->mpInfoLabel->setText(text);
    if(text.compare("")){
      this->mpColorMap->setVisible(false);
      this->mpChart->replot();
    }
  }
}

QWidget* cedar::aux::gui::QCImageDisplay::widget()
{
  return this->mpChart;
}

QPoint cedar::aux::gui::QCImageDisplay::mapToGlobal(const QPoint & pos) const
{
  return this->mpChart->mapToGlobal(pos);
}

void cedar::aux::gui::QCImageDisplay::updatePlot(QImage image)
{
  this->setText("");
  this->mpColorMap->setVisible(true);
  int w = image.width(), h = image.height();
  this->mpColorMap->data()->setSize(w, h);
  this->mpColorMap->data()->setRange(QCPRange(0, w - 1), QCPRange(h - 1, 0));
  for(int xIndex = 0; xIndex < w; ++xIndex)
  {
    for(int yIndex = 0; yIndex < h; ++yIndex)
    {
      if(qRed(image.pixel(xIndex, yIndex)) == 254 && qGreen(image.pixel(xIndex, yIndex)) == 255)
      {
        this->mpColorMap->data()->setCell(xIndex, yIndex, this->mNan);
      }
      else
      {
        this->mpColorMap->data()->setCell(xIndex, yIndex, qRed(image.pixel(xIndex, yIndex)));
      }
    }
  }

  this->mpChart->xAxis->setRange(-0.5, w - 0.5);
  this->mpChart->yAxis->setRange(-0.5, h - 0.5);
  this->mpChart->replot();
}

void cedar::aux::gui::QCImageDisplay::setColorJet(cedar::aux::EnumId gradient_id)
{
  QCPColorGradient gradient = QCPColorGradient::gpJet;
  switch(gradient_id)
  {
    case cedar::aux::ColorGradient::StandardGradients::Gray:
      gradient = QCPColorGradient::gpGrayscale;
      break;

    case cedar::aux::ColorGradient::StandardGradients::GrayInverse:
      gradient = QCPColorGradient(QCPColorGradient::gpGrayscale).inverted();
      break;
  }
  gradient.setNanColor(Qt::white);
  gradient.setNanHandling(QCPColorGradient::nhNanColor);
  this->mpColorMap->setGradient(gradient);
}

void cedar::aux::gui::QCImageDisplay::smoothScalingChanged(bool value)
{
  this->mpColorMap->setInterpolate(value);
}

cv::Mat cedar::aux::gui::QCImageDisplay::colorizeMatrix(cedar::aux::ColorGradientPtr /*colorGradient*/,
                                                        const cv::Mat& toColorize, bool applyLimits, double min,
                                                        double max) const
{
  return cedar::aux::ColorGradient::getPlotGrayColorJet()->applyTo
    (
      toColorize,
      applyLimits,
      min,
      max,
      cv::Scalar(255, 255, 254)
    );
}