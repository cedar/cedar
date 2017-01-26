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

    File:        VtkLinePlot.h

    Maintainer:  Kai Kuchenbecker
    Email:       Kai.Kuchenbecker@ini.ruhr-uni-bochum.de
    Date:        2012 11 07

    Description: 

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_VTK_LINE_PLOT_H
#define CEDAR_AUX_GUI_VTK_LINE_PLOT_H

#include "cedar/configuration.h"

#ifdef CEDAR_USE_VTK

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/MultiPlotInterface.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/auxiliaries/gui/VtkLinePlot.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QReadWriteLock>
#include <opencv2/opencv.hpp>
#include <vtkVersion.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkChartXY.h>
#include <vtkTable.h>
#include <vtkPlot.h>
#include <vtkDoubleArray.h>
#include <vtkContextView.h>
#include <vtkContextScene.h>
#include <vtkPen.h>
#include <vtkNew.h>
#include <QVTKWidget.h>
#include <vector>
#include <string>

//!@cond SKIPPED_DOCUMENTATION
namespace cedar
{
  namespace aux
  {
    namespace gui
    {
      namespace detail
      {
        /* This is an internal class of VtkLinePlot that cannot be nested because Qt's moc doesn't support nested classes.
         *
         * Don't use it outside of the VtkLinePlot!
         */
        class VtkLinePlotWorker : public QObject
        {
          Q_OBJECT

          public:
            VtkLinePlotWorker(cedar::aux::gui::VtkLinePlot* pPlot)
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
            cedar::aux::gui::VtkLinePlot* mpPlot;
        };
        CEDAR_GENERATE_POINTER_TYPES(VtkLinePlotWorker);
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
class cedar::aux::gui::VtkLinePlot : public cedar::aux::gui::MultiPlotInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::gui::detail::VtkLinePlotWorker;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  struct PlotSeries
  {
    PlotSeries(vtkSmartPointer<vtkTable> table, vtkSmartPointer<vtkChartXY> chart)
    :
    mpCurve(NULL),
    mpChart(chart),
    mpVtkTable(table)
    {
    }

    ~PlotSeries()
    {
      mpChart->RemovePlotInstance(mpCurve);
      mpVtkTable->RemoveColumnByName(mYColumnName.c_str());
    }

    //!@brief (Re-)initializes the x and y value arrays.
    void buildXAxis(unsigned int new_size);

    //!@brief the displayed data
    cedar::aux::ConstMatDataPtr mMatData;
    //!@brief a curve inside the plot
    vtkPlot* mpCurve;
    vtkWeakPointer<vtkChartXY> mpChart;
    vtkWeakPointer<vtkTable> mpVtkTable;
    //!@brief reference to the x values of the plot, currently the same for all plots
    vtkIdType mXColumn;
    //!@brief references to the y values of the plot by Name
    std::string mYColumnName;
  };

  CEDAR_GENERATE_POINTER_TYPES(PlotSeries);

  typedef std::vector<PlotSeriesPtr> PlotSeriesVector;

  struct LineColor
  {
    LineColor(unsigned char r, unsigned char g, unsigned char b)
    :
    mR(r),
    mG(g),
    mB(b)
    {
    }

    ~LineColor()
    {      
    }

    //convenience method
    unsigned char* toA()
    {
      unsigned char* colorTuple = new unsigned char[3];
      colorTuple[0] = mR;
      colorTuple[1] = mG;
      colorTuple[2] = mB;
      return colorTuple;
    }

    unsigned char mR, mG, mB;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  VtkLinePlot(QWidget* pParent = NULL);

  //!@brief Constructor expecting a DataPtr.
  VtkLinePlot(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget* pParent = NULL);

  //!@brief Destructor
  ~VtkLinePlot();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief display data
  void plot(cedar::aux::ConstDataPtr matData, const std::string& title);

  //!@brief handle timer events
  void timerEvent(QTimerEvent* pEvent);

  bool canAppend(cedar::aux::ConstDataPtr data) const;
  bool canDetach(cedar::aux::ConstDataPtr data) const;

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

  void doAppend(cedar::aux::ConstDataPtr data, const std::string& title);
  void doDetach(cedar::aux::ConstDataPtr data);

  //!@brief Applies a plot style to a given curve.
  static void applyStyle(size_t lineId, vtkPlot* pCurve);

private slots:
  void conversionDone();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief QVTKWidget nested in QWidget
  QVTKWidget* mpVtkWidget;
  //! Table holding the data of a plot
  vtkSmartPointer<vtkTable> mpVtkTable;
  vtkSmartPointer<vtkChartXY> mpChart;
  //!@brief the view for rendering the plot
  vtkSmartPointer<vtkContextView> mpView;

  PlotSeriesVector mPlotSeriesVector;

  //! For locking the plot itself.
  QReadWriteLock* mpLock;

  //! VtkPen Line Types
  static const int SOLID_LINE;
  static const int DASHED_LINE;
  static const int DOTTED_LINE;
  static const int DOT_DASHED_LINE;
  static const int DOT_DASH_DASHED_LINE;
  //! A vector containing all the colors used for plot lines.
  static std::vector<LineColor> mLineColors;

  //! A vector containing all the line stypes for the plot.
  static std::vector<int> mLineStyles;

  //! Thread in which matrix data is converted to a qwt-ready format.
  QThread* mpWorkerThread;

  //! The worker that does actual converison.
  cedar::aux::gui::detail::VtkLinePlotWorkerPtr mConversionWorker;

}; // class cedar::aux::gui::VtkLinePlot

#endif // CEDAR_USE_VTK
#endif // CEDAR_AUX_GUI_VTK_LINE_PLOT_H
