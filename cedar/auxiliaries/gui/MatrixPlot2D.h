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
#include "auxiliaries/gui/DataPlotInterface.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QWidget>
#include <QReadWriteLock>
#include <opencv2/opencv.hpp>
#include <qwtplot3d/qwt3d_gridplot.h>
#include <qwtplot3d/qwt3d_function.h>
#include <qwtplot3d/qwt3d_plot3d.h>
#include <qwtplot3d/qwt3d_io.h>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::aux::gui::MatrixPlot2D : public DataPlotInterface
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
    Matrix2DFunction(cedar::aux::MatDataPtr matData, Qwt3D::GridPlot* plot);
    double operator()(double x, double y);
    void updateMatrix();
    void setMatData(cedar::aux::MatDataPtr matData);

  private:
    cedar::aux::MatDataPtr mMatData;
    cv::Mat mInternalMat;
  };

private:
  class Perspective
  {
    public:
      Perspective(const std::string& name = "perspective",
                  double rotationX = 0, double rotationY = 0, double rotationZ = 0,
                  double scaleX = 1, double scaleY = 1, double scaleZ = 1,
                  double shiftX = 0, double shiftY = 0, double shiftZ = 0,
                  double zoom = 1);

      void applyTo(Qwt3D::Plot3D* pPlot);

      const std::string& getName() const
      {
        return this->mName;
      }

    private:
      std::string mName;
      double mRotation[3];
      double mScale[3];
      double mShift[3];
      double mZoom;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  MatrixPlot2D(QWidget *pParent = NULL);

  MatrixPlot2D(cedar::aux::DataPtr matData, QWidget *pParent = NULL);

  //!@brief Destructor
  ~MatrixPlot2D();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void display(cedar::aux::DataPtr matData);
  void showGrid(bool show);
  void timerEvent(QTimerEvent *pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void contextMenuEvent(QContextMenuEvent * pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void init();
  void resetPerspective(size_t perspectiveIndex = 0);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  cedar::aux::MatDataPtr mMatData;
  bool mShowGridLines;

  Qwt3D::GridPlot *mpPlot;

  Matrix2DFunction *mpFunction;
  std::vector<Perspective> mPerspectives;

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

