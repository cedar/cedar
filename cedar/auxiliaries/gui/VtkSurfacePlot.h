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

    File:        VtkSurfacePlot.h

    Maintainer:  Kai Kuchenbecker
    Email:       Kai.Kuchenbecker@ini.ruhr-uni-bochum.de
    Date:        2012 11 07

    Description: 

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_VTK_SURFACE_PLOT_H
#define CEDAR_AUX_GUI_VTK_SURFACE_PLOT_H

#include "cedar/configuration.h"

#ifdef CEDAR_USE_VTK

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/PlotInterface.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/auxiliaries/gui/VtkSurfacePlot.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QReadWriteLock>
#include <opencv2/opencv.hpp>
// VTK INCLUDES
#include <vtkVersion.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <vtkDoubleArray.h>
#include <QVTKWidget.h>
#include <QVTKInteractor.h>
#if VTK_MAJOR_VERSION <= 5
  #include <vtkPlaneSource.h>
  #include <vtkPoints.h>
  #include <vtkPolyData.h>
  #include <vtkDataSetMapper.h>
  #include <vtkTransformPolyDataFilter.h>
  #include <vtkWarpScalar.h>
#else // VTK_MAJOR_VERSION <= 5
  #include <vtkChartXYZ.h>
  #include <vtkContextView.h>
  #include <vtkPlotSurface.h>
  #include <vtkTable.h>
#endif // VTK_MAJOR_VERSION <= 5

//!@cond SKIPPED_DOCUMENTATION
namespace cedar
{
  namespace aux
  {
    namespace gui
    {
      namespace detail
      {
        /* This is an internal class of VtkSurfacePlot that cannot be nested because Qt's moc doesn't support nested classes.
         *
         * Don't use it outside of the VtkSurfacePlot!
         */
        class VtkSurfacePlotWorker : public QObject
        {
          Q_OBJECT

          public:
            VtkSurfacePlotWorker(cedar::aux::gui::VtkSurfacePlot* pPlot)
            :
            mpPlot(pPlot)
            {

            }

          public slots:
            void convert();

          signals:
            void done();
            void dataChanged();

          public:
            cedar::aux::gui::VtkSurfacePlot* mpPlot;
        };
        CEDAR_GENERATE_POINTER_TYPES(VtkSurfacePlotWorker);
      }
    }
  }
}
//!@endcond


/*!@brief Matrix plot that can display 1D matrices (i.e. vectors).
 *
 *        This plot is capable of displaying any matrix data with a dimensionality of one. It displays the data as a
 *        line, assuming the indices of the matrix as the x axis.
 */
class cedar::aux::gui::VtkSurfacePlot : public cedar::aux::gui::PlotInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::gui::detail::VtkSurfacePlotWorker;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  VtkSurfacePlot(QWidget* pParent = NULL);

  //!@brief Constructor expecting a DataPtr.
  VtkSurfacePlot(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget* pParent = NULL);

  //!@brief Destructor
  ~VtkSurfacePlot();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief display data
  void plot(cedar::aux::ConstDataPtr matData, const std::string& title);

  //!@brief handle timer events
  void timerEvent(QTimerEvent* pEvent);

signals:
  //!@brief Signals the worker thread to convert the data to the plot's internal format.
  void convert();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief initialize
  void init();

  #if VTK_MAJOR_VERSION <= 5
    void buildPlane(unsigned int x_size, unsigned int y_size);
    void setupCamera(vtkCamera* pCamera, const cv::Mat& data);
  #endif

private slots:
  void conversionDone();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief QVTKWidget nested in QWidget. Contains the View.
  QVTKWidget* mpVtkWidget;
  //! the displayed MatData
  cedar::aux::ConstMatDataPtr mMatData;
  #if VTK_MAJOR_VERSION <= 5
    //! plane that is displayed
    vtkSmartPointer<vtkPlaneSource> mpPlane;
    //transform-filter; builds the plane.
    vtkSmartPointer<vtkTransformPolyDataFilter> mpTransformFilter;
    //! contains converted data points
    vtkSmartPointer<vtkPolyData> mpData;
    //! warps the plane according to scalar values
    vtkSmartPointer<vtkWarpScalar> mpWarper;
    //! maps the points of warped plane into polygons
    vtkSmartPointer<vtkDataSetMapper> mpMapper;
    //! renders the plot
    vtkSmartPointer<vtkRenderer> mpRenderer;
    vtkSmartPointer<vtkRenderWindow> mpRenderWindow;

    vtkSmartPointer<vtkPoints> mpPlanePoints;
    vtkSmartPointer<vtkDoubleArray> mpZScalars;
  #else
    //! the converted data
    vtkSmartPointer<vtkTable> mpTable;
    //! the 3D-Chart containing the plot
    vtkSmartPointer<vtkChartXYZ> mpChart;
    //! the surface plot
    vtkSmartPointer<vtkPlotSurface> mpSurface;
    //! the view containing the 3D-Chart
    vtkSmartPointer<vtkContextView> mpView;
  #endif


  //! For locking the plot itself.
  QReadWriteLock* mpLock;

  //! Thread in which matrix data is converted to a qwt-ready format.
  QThread* mpWorkerThread;

  //! The worker that does actual converison.
  cedar::aux::gui::detail::VtkSurfacePlotWorkerPtr mConversionWorker;

}; // class cedar::aux::gui::VtkSurfacePlot

#endif // CEDAR_USE_VTK
#endif // CEDAR_AUX_GUI_VTK_SURFACE_PLOT_H
