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

#define NOMINMAX // prevents MSVC conflicts

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/MatrixPlot2D.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/gui/MatrixPlot.h"
#include "cedar/auxiliaries/annotation/Dimensions.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <QMenu>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::MatrixPlot2D::MatrixPlot2D(QWidget *pParent)
:
cedar::aux::gui::PlotInterface(pParent)
{
  this->init();
}

cedar::aux::gui::MatrixPlot2D::Perspective::Perspective(const std::string& name,
                                                        double rotationX, double rotationY, double rotationZ,
                                                        double scaleX, double scaleY, double scaleZ,
                                                        double shiftX, double shiftY, double shiftZ,
                                                        double zoom)
:
mName(name),
mZoom(zoom)
{
  mRotation[0] = rotationX;
  mRotation[1] = rotationY;
  mRotation[2] = rotationZ;
  mScale[0]    = scaleX;
  mScale[1]    = scaleY;
  mScale[2]    = scaleZ;
  mShift[0]    = shiftX;
  mShift[1]    = shiftY;
  mShift[2]    = shiftZ;
}

cedar::aux::gui::MatrixPlot2D::MatrixPlot2D(cedar::aux::DataPtr matData, const std::string& title, QWidget *pParent)
:
cedar::aux::gui::PlotInterface(pParent),
mShowGridLines(false),
mppArrayData(NULL)
{
  this->init();
  this->plot(matData, title);
}

cedar::aux::gui::MatrixPlot2D::~MatrixPlot2D()
{
  this->deleteArrayData();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::MatrixPlot2D::deleteArrayData()
{
  if (mppArrayData != NULL)
  {
    for (size_t i = 0; i < mDataRows; ++i)
    {
      delete[] mppArrayData[i];
    }
    mppArrayData = NULL;
  }
}

void cedar::aux::gui::MatrixPlot2D::updateArrayData()
{
  cv::Mat data;
  this->mMatData->lockForRead();
  this->mMatData->getData().convertTo(data, CV_64F);
  this->mMatData->unlock();
  if (cedar::aux::math::getDimensionalityOf(data) != 2) // plot is no longer capable of displaying the data
  {
    emit dataChanged();
    return;
  }
  if (static_cast<size_t>(data.rows) != mDataRows || static_cast<size_t>(data.cols) != mDataCols)
  {
    this->deleteArrayData();
  }

  if (!mppArrayData)
  {
    mDataRows = static_cast<size_t>(data.rows);
    mDataCols = static_cast<size_t>(data.cols);
    mppArrayData = new Qwt3D::Triple*[mDataRows];
    for (size_t i = 0; i < mDataRows; ++i)
    {
      mppArrayData[i] = new Qwt3D::Triple[mDataCols];
    }
  }

  for (size_t i = 0; i < mDataRows; ++i)
  {
    for (size_t j = 0; j < mDataCols; ++j)
    {
      Qwt3D::Triple& val = mppArrayData[i][j];
      val.x = static_cast<double>(i);
      val.y = static_cast<double>(j);
      val.z = data.at<double>(i, j);
    }
  }
}

void cedar::aux::gui::MatrixPlot2D::Perspective::applyTo(Qwt3D::Plot3D* pPlot)
{
  pPlot->setRotation(mRotation[0], mRotation[1], mRotation[2]);
  pPlot->setScale(mScale[0], mScale[1], mScale[2]);
  pPlot->setShift(mShift[0], mShift[1], mShift[2]);
  pPlot->setZoom(mZoom);
}

void cedar::aux::gui::MatrixPlot2D::plot(cedar::aux::DataPtr data, const std::string& /* title */)
{
  this->mMatData = boost::shared_dynamic_cast<cedar::aux::MatData>(data);

  if (!this->mMatData)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "Could not cast to cedar::aux::MatData in cedar::aux::gui::MatrixPlot2D::plot.");
  }

  this->startTimer(60); //!@todo make the refresh time configurable.
}

void cedar::aux::gui::MatrixPlot2D::init()
{
  this->mPerspectives.push_back(Perspective("default",
                                            90, 0, -90,
                                            1, 1, 5,
                                            0.15, 0, 0,
                                            1.0
                                            ));

  this->mPerspectives.push_back(Perspective("view 2",
                                            45, 0, 45,
                                            1, 1, 5,
                                            0.15, 0, 0,
                                            1.0
                                            ));
  // create a new layout for the widget
  QVBoxLayout *p_layout = new QVBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(p_layout);

  // create the plot object and add it to this widget
  this->mpPlot = new Qwt3D::GridPlot(this);
  p_layout->addWidget(this->mpPlot);

  //!@todo Add this as an option to a right-click context menu for the plot.
  this->mpPlot->setCoordinateStyle(Qwt3D::BOX);
  this->resetPerspective();
  this->showGrid(false);

  // apply the standard color vector
  Qwt3D::StandardColor col;
  col.setColorVector(cedar::aux::gui::MatrixPlot::getStandardColorVector());
  this->mpPlot->setDataColor(col);
  this->mpPlot->updateGL();
}

void cedar::aux::gui::MatrixPlot2D::timerEvent(QTimerEvent * /* pEvent */)
{
  if (this->isVisible() && this->mMatData)
  {
    this->updateArrayData();
    if(this->mppArrayData == NULL)
    {
      return;
    }
    this->mpPlot->createDataset(this->mppArrayData, mDataRows, mDataCols);
    this->applyLabels();
    this->mpPlot->updateGL();
  }
}


void cedar::aux::gui::MatrixPlot2D::applyLabels()
{
  try
  {
    cedar::aux::annotation::DimensionsPtr dimensions
      = this->mMatData->getAnnotation<cedar::aux::annotation::Dimensions>();

    if (dimensions->getDimensionality() == 2)
    {
      QString label_x = QString::fromStdString(dimensions->getLabel(0));
      QString label_y = QString::fromStdString(dimensions->getLabel(1));
      this->mpPlot->coordinates()->axes[Qwt3D::X1].setLabelString(label_x);
      this->mpPlot->coordinates()->axes[Qwt3D::X2].setLabelString(label_x);
      this->mpPlot->coordinates()->axes[Qwt3D::X3].setLabelString(label_x);
      this->mpPlot->coordinates()->axes[Qwt3D::X4].setLabelString(label_x);
      this->mpPlot->coordinates()->axes[Qwt3D::Y1].setLabelString(label_y);
      this->mpPlot->coordinates()->axes[Qwt3D::Y2].setLabelString(label_y);
      this->mpPlot->coordinates()->axes[Qwt3D::Y3].setLabelString(label_y);
      this->mpPlot->coordinates()->axes[Qwt3D::Y4].setLabelString(label_y);
    }
    else
    {
      cedar::aux::LogSingleton::getInstance()->debugMessage
      (
        "Wrong number of entries in dimension annotations.",
        "cedar::aux::gui::MatrixPlot2D::plot"
      );
    }
  }
  catch (cedar::aux::UnknownTypeException&)
  {
    // ok, no annotation found
  }
}

void cedar::aux::gui::MatrixPlot2D::resetPerspective(size_t perspectiveIndex)
{
  CEDAR_ASSERT(perspectiveIndex < this->mPerspectives.size());
  Perspective& perspective = this->mPerspectives.at(perspectiveIndex);
  perspective.applyTo(this->mpPlot);
}

void cedar::aux::gui::MatrixPlot2D::showGrid(bool show)
{
  this->mShowGridLines = show;
  if (show)
  {
    this->mpPlot->setPlotStyle(Qwt3D::FILLEDMESH);
    this->mpPlot->updateGL();
  }
  else
  {
    this->mpPlot->setPlotStyle(Qwt3D::FILLED);
    this->mpPlot->updateGL();
  }
}

void cedar::aux::gui::MatrixPlot2D::contextMenuEvent(QContextMenuEvent * pEvent)
{
  QMenu menu(this);

  QAction *p_reset_perspective = menu.addAction("reset perspective");

  QMenu* p_perspectives = menu.addMenu("set perspective");
  for (size_t i = 0; i < this->mPerspectives.size(); ++i)
  {
    QAction *p_action = p_perspectives->addAction(this->mPerspectives.at(i).getName().c_str());
    p_action->setData(QVariant(static_cast<uint>(i)));
  }

  menu.addSeparator();

  QAction *p_show_grid = menu.addAction("show grid");
  p_show_grid->setCheckable(true);
  p_show_grid->setChecked(this->mShowGridLines);

  QAction *p_action = menu.exec(pEvent->globalPos());

  if (p_action != NULL)
  {
    if (p_action == p_reset_perspective)
    {
      this->resetPerspective();
    }
    else if (p_action->parentWidget() == p_perspectives)
    {
      size_t index = static_cast<size_t>(p_action->data().toUInt());
      this->resetPerspective(index);
    }
    else if (p_action == p_show_grid)
    {
      this->showGrid(!this->mShowGridLines);
    }
  }
}
