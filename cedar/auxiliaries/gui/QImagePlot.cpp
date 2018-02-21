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
#include "cedar/auxiliaries/annotation/ValueRangeHint.h"
#include "cedar/auxiliaries/gui/QImagePlot.h"
#include "cedar/auxiliaries/ColorGradient.h"
#include "cedar/auxiliaries/MatData.h"

// SYSTEM INCLUDES
#include <QReadLocker>
#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolTip>
#include <QMenu>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QFileDialog>


//!@cond SKIPPED_DOCUMENTATION

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

//!@endcond

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::LockableMember<QDir> cedar::aux::gui::QImagePlot::mLastSaveLocation;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::QImagePlot::QImagePlot(QWidget* pParent)
:
cedar::aux::gui::ThreadedPlot(pParent),
mLegendAvailable(false),
mpLegend(nullptr),
_mSmoothScaling(new cedar::aux::BoolParameter(this, "smooth scaling", true)),
_mKeepAspectRatio(new cedar::aux::BoolParameter(this, "keep aspect ratio", true)),
_mAutoScaling(new cedar::aux::BoolParameter(this, "automatic value scaling", true)),
_mShowLegend(new cedar::aux::BoolParameter(this, "show legend", true)),
_mValueLimits(new cedar::aux::math::DoubleLimitsParameter(this, "value limits", 0.0, 1.0)),
_mColorJet(new cedar::aux::EnumParameter(this, "color jet", cedar::aux::ColorGradient::StandardGradients::typePtr(), cedar::aux::ColorGradient::StandardGradients::PlotDefault))
{
  this->setColorJet(cedar::aux::ColorGradient::getDefaultPlotColorJet());
  auto p_layout = new QHBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(p_layout);
  this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  this->mpImageDisplay = new cedar::aux::gui::QImagePlot::ImageDisplay(this, "no data");
  p_layout->addWidget(mpImageDisplay);
  this->mpImageDisplay->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  this->mpImageDisplay->setMinimumSize(QSize(5, 5));

  this->showLegendChanged(); //Initialize the Legend with this, as _mShowLegend is true by default now.

  QObject::connect
  (
    this,
    SIGNAL(minMaxChanged(double, double)),
    this,
    SLOT(updateMinMax(double, double))
  );

  QObject::connect
  (
    this->_mShowLegend.get(),
    SIGNAL(valueChanged()),
    this,
    SLOT(showLegendChanged())
  );

  QObject::connect
  (
    this->_mValueLimits.get(),
    SIGNAL(valueChanged()),
    this,
    SLOT(valueLimitsChanged())
  );

  QObject::connect
  (
    this->_mColorJet.get(),
    SIGNAL(valueChanged()),
    this,
    SLOT(colorJetChanged())
  );
}

//!@cond SKIPPED_DOCUMENTATION
cedar::aux::gui::detail::QImagePlotLegend::QImagePlotLegend()
{
  this->mpMin = new QLabel("min");
  this->mpMax = new QLabel("max");

  this->mpMin->setMinimumWidth(35);
  this->mpMax->setMinimumWidth(35);

  this->mpGradientDisplay = new QFrame();
  this->mpGradientDisplay->setSizePolicy(QSizePolicy::Fixed, this->mpGradientDisplay->sizePolicy().verticalPolicy());
  this->mpGradientDisplay->setFixedWidth(20);
  this->mpGradientDisplay->setFrameStyle(QFrame::Box);

  this->mpGradientDisplay->setAutoFillBackground(true);

  auto p_layout = new QGridLayout();
  p_layout->setContentsMargins(1, 1, 1, 1);
  p_layout->addWidget(this->mpGradientDisplay, 0, 0, 3, 1);
  p_layout->addWidget(this->mpMax, 0, 1);
  p_layout->addWidget(this->mpMin, 2, 1);
  p_layout->setColumnStretch(1, 1);
  p_layout->setRowStretch(1, 1);
  this->setLayout(p_layout);
}
//!@endcond

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::QImagePlot::plot(cedar::aux::ConstDataPtr data, const std::string& /*title*/)
{
  if (data->hasAnnotation<cedar::aux::annotation::ValueRangeHint>())
  {
    this->mValueHint = data->getAnnotation<cedar::aux::annotation::ValueRangeHint>();

    // set fixed scaling based on the value hint
    this->setLimits(this->mValueHint->getRange().getLower(), this->mValueHint->getRange().getUpper());
  }
  else
  {
    // if a value hint was previously set, revert to automatic scaling
    if (this->mValueHint)
    {
      this->setAutomaticScaling();
    }
    // reset the value hint
    this->mValueHint.reset();
  }
}

void cedar::aux::gui::QImagePlot::colorJetChanged()
{
  auto enum_id = this->_mColorJet->getValue();
  auto gradient = cedar::aux::ColorGradient::getStandardGradient(enum_id);
  this->setColorJet(gradient);
}

void cedar::aux::gui::QImagePlot::setColorJet(cedar::aux::ColorGradientPtr gradient)
{
  this->mColorGradient = gradient;

  if (this->mpLegend)
  {
    this->mpLegend->setGradient(this->mColorGradient);
  }
}

//!@cond SKIPPED_DOCUMENTATION
void cedar::aux::gui::detail::QImagePlotLegend::setGradient(cedar::aux::ColorGradientPtr gradient)
{
  this->mGradient = QLinearGradient(0, 1, 0, 0);
  for (auto stop_color_pair : gradient->getStops())
  {
    this->mGradient.setColorAt(static_cast<qreal>(stop_color_pair.first), stop_color_pair.second);
  }
  this->mGradient.setCoordinateMode(QGradient::ObjectBoundingMode);

  QPalette palette = this->mpGradientDisplay->palette();
  palette.setBrush(QPalette::Window, QBrush(this->mGradient));
  this->mpGradientDisplay->setPalette(palette);
}
//!@endcond

cv::Mat cedar::aux::gui::QImagePlot::colorizeMatrix(const cv::Mat& toColorize)
{
  double min = -std::numeric_limits<double>::max(), max = std::numeric_limits<double>::max();
  if (this->isAutoScaling())
  {
    if (toColorize.type() == CV_8UC1)
    {
      min = 0.0;
      max = 255.0;
    }
    else
    {
      cv::minMaxLoc(toColorize, &min, &max);
    }
    emit minMaxChanged(min, max);
  }
  else
  {
    min = this->_mValueLimits->getLowerLimit();
    max = this->_mValueLimits->getUpperLimit();
  }
  return this->colorizeMatrix
  (
    toColorize,
    !this->isAutoScaling(),
    min,
    max
  );
}

cv::Mat cedar::aux::gui::QImagePlot::colorizeMatrix(const cv::Mat& toColorize, bool applyLimits, double min, double max) const
{
  return this->mColorGradient->applyTo
  (
    toColorize,
    applyLimits,
    min,
    max
  );
}

void cedar::aux::gui::QImagePlot::setAutomaticScaling()
{
  this->_mAutoScaling->setValue(true);
}

void cedar::aux::gui::QImagePlot::setLimits(double min, double max)
{
  this->updateMinMax(min, max);

  this->_mValueLimits->setLowerLimit(min);
  this->_mValueLimits->setUpperLimit(max);
  this->_mAutoScaling->setValue(false);
}

void cedar::aux::gui::QImagePlot::queryFixedValueScale()
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
  p_lower->setValue(this->getValueLimits().getLower());

  p_label = new QLabel("upper limit:");
  p_layout->addWidget(p_label, 1, 0);

  auto p_upper = new QDoubleSpinBox();
  p_layout->addWidget(p_upper, 1, 1);
  p_upper->setMinimum(boost::numeric::bounds<double>::lowest());
  p_upper->setMaximum(boost::numeric::bounds<double>::highest());
  p_upper->setValue(this->getValueLimits().getUpper());

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

void cedar::aux::gui::QImagePlot::updateMinMax(double min, double max)
{
  if (this->mpLegend)
  {
    this->mpLegend->updateMinMax(min, max);
  }
}

void cedar::aux::gui::QImagePlot::valueLimitsChanged()
{
  if (!this->isAutoScaling() && this->mpLegend)
  {
    this->mpLegend->updateMinMax(this->getValueLimits().getLower(), this->getValueLimits().getUpper());
  }
}

void cedar::aux::gui::QImagePlot::setSmoothScaling(bool smooth)
{
  this->_mSmoothScaling->setValue(smooth);
}

void cedar::aux::gui::QImagePlot::contextMenuEvent(QContextMenuEvent *pEvent)
{
  QMenu menu(this);

  auto p_smooth = menu.addAction("smooth");
  p_smooth->setCheckable(true);
  QObject::connect(p_smooth, SIGNAL(toggled(bool)), this, SLOT(setSmoothScaling(bool)));
  p_smooth->setChecked(this->isSmoothScaling());

  auto p_ratio = menu.addAction("keep aspect ratio");
  p_ratio->setCheckable(true);
  p_ratio->setChecked(this->keepAspectRatio());
  QObject::connect(p_ratio, SIGNAL(toggled(bool)), this, SLOT(setKeepAspectRatio(bool)));

  QAction *p_legend = menu.addAction("legend");
  p_legend->setCheckable(true);
  QObject::connect(p_legend, SIGNAL(toggled(bool)), this, SLOT(showLegend(bool)));
  p_legend->setChecked(this->mpLegend != NULL && this->mpLegend->isVisible());
  p_legend->setEnabled(this->mLegendAvailable);

  QMenu* p_jet = menu.addMenu("color jet");
  for (auto enum_id : cedar::aux::ColorGradient::StandardGradients::type().list())
  {
    auto p_action = p_jet->addAction(QString::fromStdString(enum_id.prettyString()));
    p_action->setCheckable(true);
    p_action->setData(static_cast<unsigned int>(enum_id.id()));
    p_action->setChecked(enum_id == this->_mColorJet->getValue());
    QObject::connect(p_action, SIGNAL(triggered()), this, SLOT(colorJetActionTriggered()));
  }

  QMenu* p_scaling = menu.addMenu("value scaling");
  p_scaling->setEnabled(this->mValueScalingAvailable);

  auto p_auto_scale = p_scaling->addAction("automatic");
  p_auto_scale->setCheckable(true);
  p_auto_scale->setChecked(this->isAutoScaling());
  QObject::connect(p_auto_scale, SIGNAL(triggered()), this, SLOT(setAutomaticScaling()));

  auto p_fixed_scaling = p_scaling->addAction("fixed ...");
  p_fixed_scaling->setCheckable(true);
  p_fixed_scaling->setChecked(!this->isAutoScaling());
  QObject::connect(p_fixed_scaling, SIGNAL(triggered()), this, SLOT(queryFixedValueScale()));

  this->fillContextMenu(menu);

  menu.addSeparator();
  auto p_save = menu.addAction("save image ...");
  p_save->setIcon(QIcon(":/cedar/auxiliaries/gui/menu_save.svg"));
  QObject::connect(p_save, SIGNAL(triggered()), this, SLOT(saveImageActionTriggered()));

  menu.exec(pEvent->globalPos());
}

void cedar::aux::gui::QImagePlot::saveImageActionTriggered()
{
  // make a copy of the current image (make it now so the time taken to select the destination doesn't lead to a change
  // in the image)
  QReadLocker l(&mImageLock);
  QImage image_copy = this->mImage.copy();
  l.unlock();

  // query where the user wants to save it
  QReadLocker last_dir_l(mLastSaveLocation.getLockPtr());

  QString selected_path = QFileDialog::getSaveFileName
  (
    this,
    "select a destination for the image",
    mLastSaveLocation.member().absolutePath(),
    "Image (*.png *.jpg *.ppm)"
  );
  last_dir_l.unlock();

  if (selected_path.isEmpty())
  {
    return;
  }

  // remember the last save location
  QWriteLocker last_dir_wl(mLastSaveLocation.getLockPtr());
  mLastSaveLocation.member() = selected_path;
  last_dir_wl.unlock();

  // save the image
  image_copy.save(selected_path);
}

cedar::aux::ColorGradient::StandardGradients::Id cedar::aux::gui::QImagePlot::getColorJet() const
{
  return this->_mColorJet->getValue().id();
}

void cedar::aux::gui::QImagePlot::colorJetActionTriggered()
{
  auto p_action = dynamic_cast<QAction*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(p_action);
  auto id = static_cast<cedar::aux::ColorGradient::StandardGradients::Id>(p_action->data().toUInt());
  auto enum_id = cedar::aux::ColorGradient::StandardGradients::type().get(id);
  this->_mColorJet->setValue(enum_id, true);
}

void cedar::aux::gui::QImagePlot::resizeEvent(QResizeEvent * /*pEvent*/)
{
  this->resizePixmap();
}

void cedar::aux::gui::QImagePlot::resizePixmap()
{
  Qt::AspectRatioMode aspect_ratio_mode = Qt::KeepAspectRatio;

  if (!this->keepAspectRatio())
  {
    aspect_ratio_mode = Qt::IgnoreAspectRatio;
  }

  QReadLocker lock(&this->mImageLock);
  QSize scaled_size = this->mImage.size();
  scaled_size.scale(this->mpImageDisplay->size(), aspect_ratio_mode);
  if ((!this->mpImageDisplay->pixmap()
    || scaled_size != this->mpImageDisplay->pixmap()->size()
    )
    && !this->mImage.isNull()
    )
  {
    Qt::TransformationMode transformation_mode;
    if (this->isSmoothScaling())
    {
      transformation_mode = Qt::SmoothTransformation;
    }
    else
    {
      transformation_mode = Qt::FastTransformation;
    }

    QImage scaled_image = this->mImage.scaled(this->mpImageDisplay->size(), aspect_ratio_mode, transformation_mode);
    this->mpImageDisplay->setPixmap(QPixmap::fromImage(scaled_image));
  }
}

void cedar::aux::gui::QImagePlot::setInfo(const std::string& text)
{
  this->mpImageDisplay->setText(QString::fromStdString(text));
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

//!@cond SKIPPED_DOCUMENTATION
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
//!@endcond

void cedar::aux::gui::QImagePlot::fillContextMenu(QMenu&)
{
  // empty default implementation
}

void cedar::aux::gui::QImagePlot::plotClicked(QMouseEvent* /* pEvent */, double /* imageX */, double /* imageY */)
{
  // empty default implementation
}

void cedar::aux::gui::QImagePlot::showLegend(bool show)
{
  this->_mShowLegend->setValue(show);
}

void cedar::aux::gui::QImagePlot::showLegendChanged()
{
  bool show = this->_mShowLegend->getValue();
  if (show && this->mpLegend == nullptr)
  {
    auto p_layout = cedar::aux::asserted_cast<QHBoxLayout*>(this->layout());
    this->mpLegend = new cedar::aux::gui::detail::QImagePlotLegend();
    p_layout->addWidget(this->mpLegend);
    p_layout->setStretch(0, 1);
    p_layout->setStretch(1, 0);

    if (this->_mAutoScaling->getValue() == false)
    {
      this->mpLegend->updateMinMax(this->_mValueLimits->getLowerLimit(), this->_mValueLimits->getUpperLimit());
    }
  }

  if (this->mpLegend)
  {
    this->mpLegend->setVisible(show);
    this->mpLegend->setGradient(this->mColorGradient);
  }
}

void cedar::aux::gui::QImagePlot::setKeepAspectRatio(bool keepAspectRatio)
{
  this->_mKeepAspectRatio->setValue(keepAspectRatio, true);
}

bool cedar::aux::gui::QImagePlot::keepAspectRatio() const
{
  cedar::aux::Parameter::ReadLocker locker(this->_mKeepAspectRatio.get());
  bool keep = this->_mKeepAspectRatio->getValue();
  return keep;
}

//!@cond SKIPPED_DOCUMENTATION
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
//!@endcond
