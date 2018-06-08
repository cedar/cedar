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

    File:        Qt5LinePlot.h

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

#ifndef CEDAR_AUX_GUI_QT5_LINE_PLOT_H
#define CEDAR_AUX_GUI_QT5_LINE_PLOT_H

#include "cedar/configuration.h"

#ifdef CEDAR_USE_QT5_PLOTS

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/MultiPlotInterface.h"
#include "cedar/auxiliaries/math/DoubleLimitsParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/annotation/ValueRangeHint.fwd.h"
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/auxiliaries/gui/Qt5LinePlot.fwd.h"
#include "cedar/auxiliaries/math/Limits.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QReadWriteLock>
#include <opencv2/opencv.hpp>
#include <vector>
#include <QtCharts>
#include <QLegendMarker>


namespace cedar
{
  namespace aux
  {
    namespace gui
    {
      namespace detail
      {
        /* This is an internal class of QwtLinePlot that cannot be nested because Qt's moc doesn't support nested classes.
         *
         * Don't use it outside of the QwtLinePlot!
         */
        class Qt5LinePlotWorker : public QObject
        {
        Q_OBJECT

        public:
          Qt5LinePlotWorker(cedar::aux::gui::Qt5LinePlot* pPlot)
                  :
                  mpPlot(pPlot)
          {

          }

        public slots:
          void convert();

        signals:
          void done(double min, double max);
          void dataChanged();

        public:
          cedar::aux::gui::Qt5LinePlot *mpPlot;
        };
        CEDAR_GENERATE_POINTER_TYPES(Qt5LinePlotWorker);
      }
    }
  }
}

/*!@brief Matrix plot that can display 1D matrices (i.e. vectors).
 *
 *        This plot is capable of displaying any matrix data with a dimensionality of one. It displays the data as a
 *        line, assuming the indices of the matrix as the x axis.
 */
class cedar::aux::gui::Qt5LinePlot : public cedar::aux::gui::MultiPlotInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::gui::detail::Qt5LinePlotWorker;
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  struct PlotSeriesData
  {
    PlotSeriesData(QtCharts::QChart *pChart,QtCharts::QLineSeries *pQLineSeries,cedar::aux::ConstMatDataPtr pMatData):
            mChart(pChart),mQLineSeries(pQLineSeries),mMatData(pMatData)
    {
    }

    ~PlotSeriesData()
    {
      mChart->removeSeries(mQLineSeries);
    }

    QtCharts::QChart *mChart;

    QtCharts::QLineSeries *mQLineSeries;
    //!@brief the displayed data
    cedar::aux::ConstMatDataPtr mMatData;




  };

  CEDAR_GENERATE_POINTER_TYPES(PlotSeriesData);

  typedef std::vector<PlotSeriesDataPtr> PlotSeriesDataVector;



  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Qt5LinePlot(QWidget *pParent = nullptr);

  //!@brief Constructor expecting a DataPtr.
  Qt5LinePlot(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget *pParent = nullptr);

  //!@brief Destructor
  ~Qt5LinePlot();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief display data
  void plot(cedar::aux::ConstDataPtr matData, const std::string& title);

  //!@brief handle timer events
  void timerEvent(QTimerEvent *pEvent);


  void attachMarker(QtCharts::QScatterSeries* markerSeries);

  //! Checks if data can be appended, i.e., if data is the same dimensionality and size as existing data.
  bool canAppend(cedar::aux::ConstDataPtr data) const;

  //! Checks if the given data object is in this plot.
  bool canDetach(cedar::aux::ConstDataPtr data) const;

  //! Sets whether the plot emits dataChanged for 0d data
  void setAccepts0DData(bool accept);

  size_t getSamplingSize();

  void getStyleFor(cedar::aux::MatDataPtr data, QPen& pen, QBrush& brush) const;

signals:
  //!@brief Signals the worker thread to convert the data to the plot's internal format.
  void convert();

public slots:


  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief create and handle the context menu
  void contextMenuEvent(QContextMenuEvent *pEvent);

  //! Appends the data object to this plot.
  void doAppend(cedar::aux::ConstDataPtr data, const std::string& title);

  //! Detaches the given data object from this plot.
  void doDetach(cedar::aux::ConstDataPtr data);

  //! Returns a pointer to the QwtPlot object used by this plot.
  QtCharts::QChartView* getPlot();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief initialize
  void init();

private slots:
  void conversionDone(double min, double max);

  void showLegend(bool show = true);
  void showGrid(bool show = true);
  void setFixedYAxisScaling();
  void setFixedSampling();

  void handleMarkerClicked();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Pointer to the Qwt plot object used by this plot.
  QtCharts::QChartView *mpChartView;

  QtCharts::QChart *mpChart;

  //bool SettingAntiAliasing = false;
  bool SettingShowLegend = true;
  bool SettingShowGrid = false;
  bool SettingFixedYAxisScaling = false;
  bool SettingFixedSampling = false;

  size_t SamplingSize = 100;

  double YLimitMin = 0;
  double YLimitMax = 1;

  double FixedYLimitMin = 0;
  double FixedYLimitMax = 1;

//  QList<QtCharts::QLineSeries *> mpSeriesList;
  //! For locking the plot itself.
  QReadWriteLock *mpLock;

  //! Thread in which matrix data is converted to a qwt-ready format.
  QThread* mpWorkerThread;

  cedar::aux::gui::detail::Qt5LinePlotWorkerPtr mConversionWorker;

  PlotSeriesDataVector mPlotSeriesDataVector;

  std::vector<QtCharts::QScatterSeries*> mMarkers;

  //! A vector containing all the colors used for plot lines.
  static std::vector<QColor> mLineColors;

  //! A vector containing all the line stypes for the plot.
  static std::vector<Qt::PenStyle> mLineStyles;

  //! If true, the plot will not complain about 0d data. Otherwise, 0D data will lead it to emit a dataChanged signal.
  bool mPlot0D;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
private:

}; // class cedar::aux::gui::Qt5LinePlot

#endif // CEDAR_USE_QT5_PLOTS
#endif // CEDAR_AUX_GUI_QT5_LINE_PLOT_H
