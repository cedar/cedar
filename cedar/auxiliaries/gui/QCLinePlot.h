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
#ifndef CEDAR_AUX_GUI_QC_LINE_PLOT_H
#define CEDAR_AUX_GUI_QC_LINE_PLOT_H

#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/qcustomplot.h"
#include "cedar/auxiliaries/gui/MultiPlotInterface.h"
#include "cedar/auxiliaries/math/DoubleLimitsParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/annotation/ValueRangeHint.fwd.h"
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/auxiliaries/gui/QCLinePlot.fwd.h"
#include "cedar/auxiliaries/math/Limits.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QReadWriteLock>
#include <opencv2/opencv.hpp>
#include <vector>


/*!@brief Matrix plot that can display 1D matrices (i.e. vectors).
 *
 *        This plot is capable of displaying any matrix data with a dimensionality of one. It displays the data as a
 *        line, assuming the indices of the matrix as the x axis.
 */
class cedar::aux::gui::QCLinePlot : public cedar::aux::gui::MultiPlotInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:


    //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  QCLinePlot(QWidget *pParent = nullptr);

  //!@brief Constructor expecting a DataPtr.
  QCLinePlot(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget *pParent = nullptr);

  //!@brief Destructor
  ~QCLinePlot();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief display data
  void plot(cedar::aux::ConstDataPtr matData, const std::string& title);

  //!@brief handle timer events
  void timerEvent(QTimerEvent *pEvent);


  //! Checks if data can be appended, i.e., if data is the same dimensionality and size as existing data.
  bool canAppend(cedar::aux::ConstDataPtr data) const;

  //! Checks if the given data object is in this plot.
  bool canDetach(cedar::aux::ConstDataPtr data) const;

  //! Sets whether the plot emits dataChanged for 0d data
  void setAccepts0DData(bool accept);


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
  //! Appends the data object to this plot.
  void doAppend(cedar::aux::ConstDataPtr data, const std::string& title);

  //! Detaches the given data object from this plot.
  void doDetach(cedar::aux::ConstDataPtr data);

  //! Returns a pointer to the QwtPlot object used by this plot.
  QCustomPlot* getPlot();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief initialize
  void init();

private slots:

  void showLegend(bool show = true);
  void showGrid(bool show = true);

  void contextMenuRequest(QPoint);

  void autoScalingChanged();

  void axisLimitsChanged();

  void gridVisibilityChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  bool DiscreteMetricLabels = false;
  int DiscreteMetricLabelCounter = 0;
  QSharedPointer<QCPAxisTickerText> textTicker;
private:
    QCustomPlot *mpChart;
    std::vector<cedar::aux::ConstDataPtr> PlotSeriesDataVector;
    std::vector<QColor> mLineColors;

  //! For locking the plot itself.
  QReadWriteLock *mpLock;

  //! If true, the plot will not complain about 0d data. Otherwise, 0D data will lead it to emit a dataChanged signal.
  bool mPlot0D;

  bool SettingShowLegend = false;

  bool DiscreteMetric = false;


  bool SettingShowGrid = false;
//  bool SettingFixedYAxisScaling = false;

  double YLimitMin = 0;
  double YLimitMax = 1;

  //Todo: Remove these
  double FixedYLimitMin = 0;
  double FixedYLimitMax = 1;

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

}; // class cedar::aux::gui::QCLinePlot

#endif // CEDAR_AUX_GUI_QC_LINE_PLOT_H