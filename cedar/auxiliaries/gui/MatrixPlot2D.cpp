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
#include "auxiliaries/exceptions.h"
#include "auxiliaries/gui/MatrixPlot.h"

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
QWidget(pParent),
mpMat(NULL),
mpeLock(NULL),
mpFunction(NULL)
{
  this->init();
}

cedar::aux::gui::MatrixPlot2D::MatrixPlot2D(cv::Mat* mat, QReadWriteLock *lock, QWidget *pParent)
:
QWidget(pParent),
mpMat(mat),
mpeLock(NULL),
mShowGridLines(false),
mpFunction(NULL)
{
  this->init();
  this->display(mat, lock);
}

cedar::aux::gui::MatrixPlot2D::Matrix2DFunction::Matrix2DFunction
                                                 (
                                                   QReadWriteLock *pLock,
                                                   Qwt3D::GridPlot* plot,
                                                   cv::Mat* matrix
                                                 )
:
Qwt3D::Function(plot),
mpeLock(pLock)
{
  this->updateMatrix(matrix);
}

cedar::aux::gui::MatrixPlot2D::Matrix2DFunction::Matrix2DFunction(Qwt3D::GridPlot* plot)
:
Qwt3D::Function(plot),
mpeLock(NULL)
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


void cedar::aux::gui::MatrixPlot2D::Matrix2DFunction::updateMatrix(cv::Mat* matrix)
{
  if (this->mpeLock)
  {
    this->mpeLock->lockForRead();
    int x_mesh = matrix->rows;
    int y_mesh = matrix->cols;
    this->setMesh(x_mesh, y_mesh);
    this->setDomain(0, x_mesh - 1, 0, y_mesh - 1);
    this->mMatrix = matrix->clone();
    this->mpeLock->unlock();
  }
}

void cedar::aux::gui::MatrixPlot2D::Matrix2DFunction::setLock(QReadWriteLock *lock)
{
  this->mpeLock = lock;
}

double cedar::aux::gui::MatrixPlot2D::Matrix2DFunction::operator()(double x, double y)
{
  int row = static_cast<int>(x);
  int col = static_cast<int>(y);
  //!@todo properly map to some sort of bounds.
  switch (this->mMatrix.type())
  {
    case CV_32F:
      return static_cast<double>(this->mMatrix.at<float>(row, col));

    case CV_64F:
      return static_cast<double>(this->mMatrix.at<double>(row, col));

    default:
      CEDAR_THROW(cedar::aux::UnhandledTypeException, "Unhandled matrix type in MatrixPlot2D::Matrix2DFunction.");
      return 0;
  }
}


void cedar::aux::gui::MatrixPlot2D::display(cv::Mat* mat, QReadWriteLock *lock)
{
  this->mpMat = mat;
  this->mpeLock = lock;
  this->mpFunction->setLock(lock);
  this->mpFunction->updateMatrix(this->mpMat);
  this->startTimer(30); //!@todo make the refresh time configurable.
}

void cedar::aux::gui::MatrixPlot2D::init()
{
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
  if (this->isVisible() && this->mpMat != NULL)
  {
    this->mpFunction->updateMatrix(this->mpMat);
    this->mpFunction->create();
    this->mpPlot->updateData();
    this->mpPlot->updateNormals();
    this->mpPlot->updateGL();
  }
}

void cedar::aux::gui::MatrixPlot2D::resetPerspective()
{
  this->mpPlot->setRotation(90, 0, -90);
  this->mpPlot->setScale(1, 1, 5);
  this->mpPlot->setShift(0.15, 0, 0);
  this->mpPlot->setZoom(1.0);
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
    else if (p_action == p_show_grid)
    {
      this->showGrid(!this->mShowGridLines);
    }
  }
}
