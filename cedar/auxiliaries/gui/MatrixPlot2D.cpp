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
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::MatrixPlot2D::MatrixPlot2D(QWidget *pParent)
:
QWidget(pParent),
mpeLock(NULL),
mpFunction(NULL)
{
  this->init();
}

cedar::aux::gui::MatrixPlot2D::MatrixPlot2D(cv::Mat mat, QReadWriteLock *lock, QWidget *pParent)
:
QWidget(pParent),
mpeLock(NULL),
mpFunction(NULL)
{
  this->init();
  this->display(mat, lock);
}

cedar::aux::gui::MatrixPlot2D::Matrix2DFunction::Matrix2DFunction
                                                 (
                                                   QReadWriteLock *pLock,
                                                   Qwt3D::GridPlot* plot,
                                                   const cv::Mat& matrix
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

  delete mpTimer;
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::MatrixPlot2D::Matrix2DFunction::updateMatrix(const cv::Mat& matrix)
{
  this->mpeLock->lockForRead();
  int x_mesh = matrix.rows;
  int y_mesh = matrix.cols;
  this->setMesh(x_mesh, y_mesh);
  this->setDomain(0, x_mesh - 1, 0, y_mesh - 1);
  this->mMatrix = matrix.clone();
  this->mpeLock->unlock();
}

void cedar::aux::gui::MatrixPlot2D::Matrix2DFunction::setLock(QReadWriteLock *lock)
{
  this->mpeLock = lock;
}

double cedar::aux::gui::MatrixPlot2D::Matrix2DFunction::operator()(double x, double y)
{
  //!@todo properly map to some sort of bounds.
  return this->mMatrix.at<float>((int)x,(int)y);
}

void cedar::aux::gui::MatrixPlot2D::display(cv::Mat mat, QReadWriteLock *lock)
{
  this->mMat = mat;
  this->mpeLock = lock;
  this->mpFunction->setLock(lock);
  this->mpFunction->updateMatrix(this->mMat);
  this->mpTimer->start();
}

void cedar::aux::gui::MatrixPlot2D::init()
{
  // create a new layout for the widget
  QVBoxLayout *p_layout = new QVBoxLayout();
  this->setLayout(p_layout);

  // create the plot object and add it to this widget
  this->mpPlot = new Qwt3D::GridPlot(this);
  p_layout->addWidget(this->mpPlot);

  // setup plot style
  this->mpPlot->setCoordinateStyle(Qwt3D::BOX);

  // create the function for displaying the matrix
  this->mpFunction = new Matrix2DFunction(this->mpPlot);

  // setup the timer
  this->mpTimer = new QTimer(this);
  connect(this->mpTimer, SIGNAL(timeout()), this, SLOT(updatePlot()));

  // apply the standard color vector
  Qwt3D::StandardColor col;
  col.setColorVector(cedar::aux::gui::MatrixPlot::getStandardColorVector());
  this->mpPlot->setDataColor(col);
  this->mpPlot->updateGL();
}

void cedar::aux::gui::MatrixPlot2D::updatePlot()
{
  if (this->isVisible())
  {
    this->mpFunction->updateMatrix(this->mMat);
    this->mpFunction->create();
    this->mpPlot->updateData();
    this->mpPlot->updateNormals();
    this->mpPlot->updateGL();
  }
}
