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
#include "auxiliaries/gui/MatrixPlot2D.h"
#include "auxiliaries/gui/exceptions.h"
#include "auxiliaries/gui/MatrixPlot.h"
#include "auxiliaries/DataT.h"
#include "auxiliaries/exceptions.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <QMenu>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::MatrixPlot2D::MatrixPlot2D(QWidget *pParent)
:
cedar::aux::gui::DataPlotInterface(pParent),
mpFunction(NULL)
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

cedar::aux::gui::MatrixPlot2D::MatrixPlot2D(cedar::aux::DataPtr matData, QWidget *pParent)
:
cedar::aux::gui::DataPlotInterface(pParent),
mShowGridLines(false),
mpFunction(NULL)
{
  this->init();
  this->display(matData);
}

cedar::aux::gui::MatrixPlot2D::Matrix2DFunction::Matrix2DFunction
                                                 (
                                                   cedar::aux::MatDataPtr matData,
                                                   Qwt3D::GridPlot* plot
                                                 )
:
Qwt3D::Function(plot),
mMatData(matData)
{
  this->updateMatrix();
}

cedar::aux::gui::MatrixPlot2D::Matrix2DFunction::Matrix2DFunction(Qwt3D::GridPlot* plot)
:
Qwt3D::Function(plot)
{
}

cedar::aux::gui::MatrixPlot2D::~MatrixPlot2D()
{
  if (mpFunction != NULL)
  {
    delete mpFunction;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::MatrixPlot2D::Perspective::applyTo(Qwt3D::Plot3D* pPlot)
{
  pPlot->setRotation(mRotation[0], mRotation[1], mRotation[2]);
  pPlot->setScale(mScale[0], mScale[1], mScale[2]);
  pPlot->setShift(mShift[0], mShift[1], mShift[2]);
  pPlot->setZoom(mZoom);
}

void cedar::aux::gui::MatrixPlot2D::Matrix2DFunction::updateMatrix()
{
  this->mMatData->lockForRead();
  this->mInternalMat = this->mMatData->getData().clone();
  this->mMatData->unlock();

  int x_mesh = this->mInternalMat.rows;
  int y_mesh = this->mInternalMat.cols;
  this->setMesh(x_mesh, y_mesh);
  this->setDomain(0, x_mesh - 1, 0, y_mesh - 1);
}

void cedar::aux::gui::MatrixPlot2D::Matrix2DFunction::setMatData(cedar::aux::MatDataPtr matData)
{
  this->mMatData = matData;
}

double cedar::aux::gui::MatrixPlot2D::Matrix2DFunction::operator()(double x, double y)
{
  //!@todo properly map to some sort of bounds.
  int row = static_cast<int>(x);
  int col = static_cast<int>(y);
  cv::Mat& mat = this->mMatData->getData();
  switch (mat.type())
  {
    case CV_32F:
      return static_cast<double>(mat.at<float>(row, col));

    case CV_64F:
      return static_cast<double>(mat.at<double>(row, col));

    default:
      CEDAR_THROW(cedar::aux::UnhandledTypeException,
                  "Unhandled matrix type in cedar::aux::gui::MatrixPlot2D::Matrix2DFunction::operator().");
      return 0;
  }
}


void cedar::aux::gui::MatrixPlot2D::display(cedar::aux::DataPtr data)
{
  this->mMatData = boost::shared_dynamic_cast<cedar::aux::MatData>(data);

  if (!this->mMatData)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "Could not cast to cedar::aux::MatData in cedar::aux::gui::MatrixPlot2D::display.");
  }

  this->mpFunction->setMatData(this->mMatData);
  this->startTimer(30); //!@todo make the refresh time configurable.
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
  this->setLayout(p_layout);

  // create the plot object and add it to this widget
  this->mpPlot = new Qwt3D::GridPlot(this);
  p_layout->addWidget(this->mpPlot);

  //!@todo Add these as options to a right-click context menu for the plot.
  this->mpPlot->setCoordinateStyle(Qwt3D::BOX);
  this->resetPerspective();
  this->showGrid(false);

  // create the function for displaying the matrix
  this->mpFunction = new Matrix2DFunction(this->mpPlot);

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
    this->mpFunction->updateMatrix();
    this->mpFunction->create();
    this->mpPlot->updateData();
    this->mpPlot->updateNormals();
    this->mpPlot->updateGL();
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
