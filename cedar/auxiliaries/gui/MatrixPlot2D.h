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

    File:        MatrixPlot.h

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

#ifndef CEDAR_AUX_GUI_MATRIX_PLOT_2D_H
#define CEDAR_AUX_GUI_MATRIX_PLOT_2D_H

// LOCAL INCLUDES
#include "auxiliaries/gui/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QWidget>
#include <QReadWriteLock>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include <qwtplot3d/qwt3d_gridplot.h>
#include <qwtplot3d/qwt3d_function.h>
#include <qwtplot3d/qwt3d_plot3d.h>
#include <qwtplot3d/qwt3d_io.h>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::aux::gui::MatrixPlot2D : public QWidget
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  class Matrix2DFunction : public Qwt3D::Function
  {
  public:
    Matrix2DFunction(Qwt3D::GridPlot* plot);
    Matrix2DFunction(QReadWriteLock *pLock, Qwt3D::GridPlot* plot, const cv::Mat& matrix);
    double operator()(double x, double y);
    void updateMatrix(const cv::Mat& matrix);

    void setLock(QReadWriteLock *lock);

  private:
    cv::Mat mMatrix;
    QReadWriteLock *mpeLock;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  MatrixPlot2D(QWidget *pParent = NULL);

  MatrixPlot2D(cv::Mat mat, QReadWriteLock *lock, QWidget *pParent = NULL);

  //!@brief Destructor
  ~MatrixPlot2D();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void display(cv::Mat mat, QReadWriteLock *lock);

public slots:
  void updatePlot();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void init();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  cv::Mat mMat;
  QTimer* mpTimer;
  QReadWriteLock *mpeLock;

  Qwt3D::GridPlot *mpPlot;

  Matrix2DFunction *mpFunction;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::gui::MatrixPlot2D

#endif // CEDAR_AUX_GUI_MATRIX_PLOT_2D_H

