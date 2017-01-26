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

    File:        QwtLinePlot.h

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

#ifndef CEDAR_AUX_GUI_QWT_LINE_PLOT_H
#define CEDAR_AUX_GUI_QWT_LINE_PLOT_H

#include "cedar/configuration.h"

#ifdef CEDAR_USE_QWT

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/MultiPlotInterface.h"
#include "cedar/auxiliaries/math/DoubleLimitsParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/annotation/ValueRangeHint.fwd.h"
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/auxiliaries/gui/QwtLinePlot.fwd.h"
#include "cedar/auxiliaries/math/Limits.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QReadWriteLock>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_grid.h>
#include <opencv2/opencv.hpp>
#include <vector>

//!@cond SKIPPED_DOCUMENTATION
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
        class QwtLinePlotWorker : public QObject
        {
          Q_OBJECT

          public:
            QwtLinePlotWorker(cedar::aux::gui::QwtLinePlot* pPlot)
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
            cedar::aux::gui::QwtLinePlot *mpPlot;
        };
        CEDAR_GENERATE_POINTER_TYPES(QwtLinePlotWorker);
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
class cedar::aux::gui::QwtLinePlot : public cedar::aux::gui::MultiPlotInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::gui::detail::QwtLinePlotWorker;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  struct PlotSeries
  {
    PlotSeries()
    :
    mpCurve(nullptr)
    {
    }

    ~PlotSeries()
    {
      mpCurve->detach();
    }

    //!@brief (Re-)initializes the x and y value arrays.
    void buildArrays(unsigned int new_size, double& min, double& max);

    //!@brief the displayed data
    cedar::aux::ConstMatDataPtr mMatData;

    //!@brief a curve inside the plot
    QwtPlotCurve *mpCurve;

    //!@brief the x values of the plot
    std::vector<double> mXValues;

    //!@brief the y values of the plot
    std::vector<double> mYValues;

    //! The value range hint, if any.
    cedar::aux::annotation::ConstValueRangeHintPtr mValueRange;
  };

  CEDAR_GENERATE_POINTER_TYPES(PlotSeries);

  typedef std::vector<PlotSeriesPtr> PlotSeriesVector;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  QwtLinePlot(QWidget *pParent = nullptr);

  //!@brief Constructor expecting a DataPtr.
  QwtLinePlot(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget *pParent = nullptr);

  //!@brief Destructor
  ~QwtLinePlot();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief display data
  void plot(cedar::aux::ConstDataPtr matData, const std::string& title);

  //!@brief handle timer events
  void timerEvent(QTimerEvent *pEvent);

  /*! Attaches a marker to this plot.
   */
  void attachMarker(QwtPlotMarker *pMarker);

  /*! Detaches the given marker from this plot. Also deletes the marker.
   */
  void detachMarker(QwtPlotMarker *pMarker);

  /*!@brief Detaches and deletes all markers added to this plot.
   *
   * @remarks This method is a temporary way to provide annotations, a more general one will be added soon.
   */
  void clearMarkers();

  //! Checks if data can be appended, i.e., if data is the same dimensionality and size as existing data.
  bool canAppend(cedar::aux::ConstDataPtr data) const;

  //! Checks if the given data object is in this plot.
  bool canDetach(cedar::aux::ConstDataPtr data) const;

  //!@brief Returns the limits of the x axis.
  cedar::aux::math::Limits<double> getXLimits() const;

  //!@brief Returns the limits of the y axis.
  cedar::aux::math::Limits<double> getYLimits() const;

  //! Returns whether or not autoscaling is enabled for this plot.
  bool autoScalingEnabled() const;

  //! Sets whether the plot emits dataChanged for 0d data
  void setAccepts0DData(bool accept);

  //! Determines the style (pen and brush) to be used for the given data object in this plot.
  void getStyleFor(cedar::aux::ConstDataPtr data, QPen& pen, QBrush& brush) const;

signals:
  //!@brief Signals the worker thread to convert the data to the plot's internal format.
  void convert();

public slots:
  //! When called, the y axis scaling is determined automatically every time the plot updates.
  void setAutomaticYAxisScaling();

  /*! @brief Slot that is called when the menu entry in the plot is called.
   *
   *  This function opens a dialog that lets the user enter the desired interval for the Y axis.
   */
  void setFixedYAxisScaling();

  /*! @brief Sets the minimum and maximum for the y axis.
   */
  void setFixedYAxisScaling(double lower, double upper);

  /*! @brief Sets the minimum and maximum for the x axis.
   */
  void setFixedXAxisScaling(double lower, double upper);

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
  QwtPlot* getPlot();

  //! Enables or disables automatic determination of the x axis limits.
  void setAutoDetermineXLimits(bool automatic);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief initialize
  void init();

  //!@brief Applies a plot style to a given curve.
  static void applyStyle(cedar::aux::ConstDataPtr data, size_t lineId, QwtPlotCurve *pCurve);

private slots:
  void showLegend(bool show = true);

  void showMajorGrid(bool show = true);

  void showMinorGrid(bool show = true);

  //! Shows/hides the minor and major grid at the same time.
  void showGrid(bool show = true);

  void conversionDone(double min, double max);

  void autoScalingChanged();

  void axisLimitsChanged();

  void gridVisibilityChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Pointer to the Qwt plot object used by this plot.
  QwtPlot *mpPlot;

  QwtPlotGrid *mpGrid;

  PlotSeriesVector mPlotSeriesVector;

  //! For locking the plot itself.
  QReadWriteLock *mpLock;

  //! A vector containing all the colors used for plot lines.
  static std::vector<QColor> mLineColors;

  //! A vector containing all the line stypes for the plot.
  static std::vector<Qt::PenStyle> mLineStyles;

  //! Thread in which matrix data is converted to a qwt-ready format.
  QThread* mpWorkerThread;

  //! The worker that does actual converison.
  cedar::aux::gui::detail::QwtLinePlotWorkerPtr mConversionWorker;

  std::vector<QwtPlotMarker*> mMarkers;

  //! If true, the plot will not complain about 0d data. Otherwise, 0D data will lead it to emit a dataChanged signal.
  bool mPlot0D;

  //! If true, the plot will set its x limits automatically in each plot step.
  bool mAutoDetermineXLimits;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! Whether the y axis limits are applied.
  cedar::aux::BoolParameterPtr _mAutoScalingEnabled;

  //! Whether the major grid is displayed.
  cedar::aux::BoolParameterPtr _mMajorGridVisible;

  //! Whether the minor grid is displayed.
  cedar::aux::BoolParameterPtr _mMinorGridVisible;

  //! Limits for the y axis.
  cedar::aux::math::DoubleLimitsParameterPtr _mYAxisLimits;

}; // class cedar::aux::gui::QwtLinePlot

#endif // CEDAR_USE_QWT
#endif // CEDAR_AUX_GUI_QWT_LINE_PLOT_H
