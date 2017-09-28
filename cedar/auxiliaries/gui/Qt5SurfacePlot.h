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

    File:        Qt5SurfacePlot.h

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

#ifndef CEDAR_AUX_GUI_QT5_SURFACE_PLOT_H
#define CEDAR_AUX_GUI_QT5_SURFACE_PLOT_H

#include "cedar/configuration.h"

#ifdef CEDAR_USE_QT5_PLOTS
// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/PlotInterface.h"
#include "cedar/auxiliaries/gui/Qt5SurfacePlotInputHandler.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/auxiliaries/gui/Qt5SurfacePlot.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QReadWriteLock>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>

//!@cond SKIPPED_DOCUMENTATION
namespace cedar
{
  namespace aux
  {
    namespace gui
    {
      namespace detail
      {
        /* This is an internal class of Qt5SurfacePlot that cannot be nested because Qt's moc doesn't support nested classes.
         *
         * Don't use it outside of the Qt5SurfacePlot!
         */
        class Qt5SurfacePlotWorker : public QObject
        {
          Q_OBJECT

          public:
            Qt5SurfacePlotWorker(cedar::aux::gui::Qt5SurfacePlot* pPlot)
            :
            mpPlot(pPlot)
            {
            }

          public slots:
            void convert();

          signals:
            void done();

          public:
            cedar::aux::gui::Qt5SurfacePlot *mpPlot;
        };
        CEDAR_GENERATE_POINTER_TYPES(Qt5SurfacePlotWorker);
      }
    }
  }
}
//!@endcond


/*!@brief Matrix plot that can display 2D matrices (i.e. vectors).
 *
 *        Matrices displayed by this plot are plotted as a three-dimensional surface, where the x- and y-coordinates are
 *        assumed to be the indices of the 2d matrix while the z-coordinate is the value stored within the matrix.
 */
class cedar::aux::gui::Qt5SurfacePlot : public PlotInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::gui::detail::Qt5SurfacePlotWorker;
  friend class cedar::aux::gui::Qt5SurfacePlotInputHandler;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  class Perspective
  {
    public:
      Perspective(const std::string& name = "perspective",
                  QtDataVisualization::Q3DCamera::CameraPreset camPreset = QtDataVisualization::Q3DCamera::CameraPresetNone,
                  double rotationX = 0, double rotationY = 0);

      const std::string& getName() const
      {
        return this->mName;
      }

      QtDataVisualization::Q3DCamera::CameraPreset getPreset()
      {
        return this->mCamPreset;
      }

      double getRotationX()
      {
        return this->mRotation[0];
      }

      double getRotationY()
      {
        return this->mRotation[1];
      }


    private:
      std::string mName;
      QtDataVisualization::Q3DCamera::CameraPreset mCamPreset;
      double mRotation[2];
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Qt5SurfacePlot(QWidget *pParent = NULL);

  //!@brief Constructor expecting a DataPtr.
  Qt5SurfacePlot(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget* pParent = NULL);

  //!@brief Destructor
  ~Qt5SurfacePlot();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief display data
  void plot(cedar::aux::ConstDataPtr matData, const std::string& title);
  //!@brief show or hide the plot grid
  void showGrid(bool show);

  void setBold(bool isBold);
  //!@brief handle timer events
  void timerEvent(QTimerEvent *pEvent);

signals:
  //!@brief Signals the worker thread to convert the data to the plot's internal format.
  void convert();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief create and handle the context menu
//  void contextMenuEvent(QContextMenuEvent * pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief initialize
  void init();

  //!@brief reset the perspective of the plot
  void resetPerspective(size_t perspectiveIndex = 0);

  //!@brief delete the allocated array data
  void deleteArrayData();

  //!@brief update the allocated array data
  void updateArrayData();

  //!@brief Applies the labels from the data object to the plot.
  void applyLabels();

private slots:
  void conversionDone();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! the displayed MatData
  cedar::aux::ConstMatDataPtr mMatData;

  //! flag if plot grid should be displayed
  bool mShowGridLines;

  bool mIsBold;

  cedar::aux::gui::Qt5SurfacePlotInputHandler *m_inputHandler;

  double y_min;
  double y_max;

  //! the plot object
//  Qwt3D::GridPlot* mpPlot;

  QtDataVisualization::QSurfaceDataArray *dataArray;

  QtDataVisualization::Q3DSurface *m_graph;

  QtDataVisualization::QSurfaceDataProxy *m_sqrtSinProxy;

  QtDataVisualization::QSurface3DSeries *m_sqrtSinSeries;


  //! vector of possible perspectives
  std::vector<Perspective> mPerspectives;

  //! row count of data
  size_t mDataRows;

  //! column count of data
  size_t mDataCols;

  //! Thread in which conversion of mat data to qwt triple is done.
  QThread* mpWorkerThread;

  //! For locking the plot itself.
  QReadWriteLock *mpLock;

  //! Worker object.
  cedar::aux::gui::detail::Qt5SurfacePlotWorkerPtr mWorker;
}; // class cedar::aux::gui::Qt5SurfacePlot

#endif // CEDAR_USE_QT5_PLOTS
#endif // CEDAR_AUX_GUI_QT5_SURFACE_PLOT_H
