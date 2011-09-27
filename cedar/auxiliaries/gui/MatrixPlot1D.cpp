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

    File:        MatrixPlot2D.cpp

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

// LOCAL INCLUDES
#include "auxiliaries/gui/MatrixPlot1D.h"
#include "auxiliaries/gui/exceptions.h"
#include "auxiliaries/DataT.h"
#include "auxiliaries/exceptions.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QContextMenuEvent>
#include <QVBoxLayout>
#include <QPalette>
#include <QMenu>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::MatrixPlot1D::MatrixPlot1D(QWidget *pParent)
:
cedar::aux::gui::DataPlotInterface(pParent)
{
  this->init();
}

cedar::aux::gui::MatrixPlot1D::MatrixPlot1D(cedar::aux::DataPtr matData, QWidget *pParent)
:
cedar::aux::gui::DataPlotInterface(pParent)
{
  this->init();
  this->display(matData);
}

cedar::aux::gui::MatrixPlot1D::~MatrixPlot1D()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::aux::gui::MatrixPlot1D::display(cedar::aux::DataPtr data)
{
  this->mMatData = boost::shared_dynamic_cast<cedar::aux::MatData>(data);

  if (!this->mMatData)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "Could not cast to cedar::aux::MatData in cedar::aux::gui::MatrixPlot1D::display.");
  }
  if (this->mpCurve != NULL)
  {
    delete this->mpCurve;
  }
  this->mpCurve = new QwtPlotCurve("data");
  this->setPlotStyle(this->mpCurve);

  data->lockForRead();
  const cv::Mat& mat = this->mMatData->getData();
  size_t num = static_cast<size_t>(mat.rows);
  if (num == 1)
  {
    num = static_cast<size_t>(mat.cols);
  }
  data->unlock();
  mpXValues.resize(num);
  mpYValues.resize(num);

  for (size_t i = 0; i < num; ++i)
  {
    mpXValues.at(i) = static_cast<double>(i);
  }

  this->mpCurve->setData(&this->mpXValues.at(0),
                         &this->mpYValues.at(0),
                         num);

  this->mpCurve->attach(this->mpPlot);


  this->startTimer(30); //!@todo make the refresh time configurable.
}

void cedar::aux::gui::MatrixPlot1D::init()
{
  mpCurve = NULL;

  QPalette palette = this->palette();
  palette.setColor(QPalette::Window, Qt::white);
  this->setPalette(palette);

  QVBoxLayout *p_layout = new QVBoxLayout();
  this->setLayout(p_layout);

  mpPlot = new QwtPlot(this);
  this->layout()->addWidget(mpPlot);
}

void cedar::aux::gui::MatrixPlot1D::setPlotStyle(QwtPlotCurve *pCurve)
{
  QPen pen = pCurve->pen();
  pen.setWidthF(2);
  pCurve->setPen(pen);
}

void cedar::aux::gui::MatrixPlot1D::contextMenuEvent(QContextMenuEvent *pEvent)
{
  QMenu menu(this);
  QAction *p_antialiasing = menu.addAction("antialiasing");
  p_antialiasing->setCheckable(true);
  p_antialiasing->setChecked(this->mpCurve->testRenderHint(QwtPlotItem::RenderAntialiased));

  QAction *p_action = menu.exec(pEvent->globalPos());
  if (p_action == NULL)
  {
    return;
  }

  if (p_action == p_antialiasing)
  {
    this->mpCurve->setRenderHint(QwtPlotItem::RenderAntialiased, p_action->isChecked());
  }
}


void cedar::aux::gui::MatrixPlot1D::timerEvent(QTimerEvent * /* pEvent */)
{
  if (!this->isVisible())
  {
    return;
  }

  const cv::Mat& mat = this->mMatData->getData();
  this->mMatData->lockForRead();

  CEDAR_DEBUG_ASSERT(mpXValues.size() == mpYValues.size());

  for (size_t i = 0; i < mpXValues.size(); ++i)
  {
    switch (mat.type())
    {
      case CV_32F:
        mpYValues.at(i) = mat.at<float>(static_cast<int>(i));
        break;

      case CV_64F:
        mpYValues.at(i) = mat.at<double>(static_cast<int>(i));
        break;

      default:
        std::cout << "Unhandled matrix type " << mat.type() << " in cedar::aux::gui::MatrixPlot1D::timerEvent"
                  << std::endl;
    }
  }
  this->mMatData->unlock();

  this->mpCurve->setData(&this->mpXValues.at(0),
                         &this->mpYValues.at(0),
                         static_cast<int>(this->mpXValues.size()));
  this->mpPlot->replot();
}
