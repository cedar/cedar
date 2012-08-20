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

    File:        HistoryPlot0D.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 09 23

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_HISTORY_PLOT_0D_H
#define CEDAR_AUX_GUI_HISTORY_PLOT_0D_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/namespace.h"
#include "cedar/auxiliaries/gui/MultiPlotInterface.h"

// SYSTEM INCLUDES
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <deque>

/*!@brief A time-based plot for 0D values. Displays a history of this value from a certain point in the past up to now.
 *
 * More detailed description of the class.
 *
 * @todo Some of the code here is redundant with MatrixPlot1D.
 */
class cedar::aux::gui::HistoryPlot0D : public cedar::aux::gui::MultiPlotInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  struct CurveInfo
  {
    CurveInfo()
    :
    mCurve(NULL)
    {
    }

    void setData(cedar::aux::DataPtr data);

    double getDataValue() const;

    //! The data of this curve, as generic data pointer
    cedar::aux::DataPtr mData;

    //! The data of this curve, as double data
    cedar::aux::DoubleDataPtr mDoubleData;

    //! The data of this curve, as matrix data
    cedar::aux::MatDataPtr mMatData;

    //! The qwt curve
    QwtPlotCurve* mCurve;

    //! y values of plot, stored as double-ended queue
    std::deque<double> mYValues;

    //! y values of plot, stored as vector (needed to pass this to qwt plot because deque is not stored linearly)
    std::vector<double> mYArray;

  };
  CEDAR_GENERATE_POINTER_TYPES(CurveInfo);

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  HistoryPlot0D(QWidget *pParent = NULL);

  //!@brief Constructor that expects a DataPtr
  HistoryPlot0D(cedar::aux::DataPtr matData, const std::string& title, QWidget *pParent = NULL);

  //!@brief Destructor
  ~HistoryPlot0D();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief display data
  void plot(cedar::aux::DataPtr data, const std::string& title);

  //!@brief handle timer events
  void timerEvent(QTimerEvent *pEvent);

  bool canAppend(cedar::aux::ConstDataPtr data) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief create and handle the context menu
  void contextMenuEvent(QContextMenuEvent *pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief initialize the plot
  void init();

  void doAppend(cedar::aux::DataPtr data, const std::string& title);

  double getDataValue(size_t index);

  void applyStyle(size_t lineId, QwtPlotCurve *pCurve);

private slots:
  void showLegend(bool show = true);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  std::vector<CurveInfoPtr> mCurves;

  //!@brief the current widget that holds the plot
  QWidget *mpCurrentPlotWidget;

  //!@brief a plot
  QwtPlot *mpPlot;

  //!@brief x values of plot, stored as double-ended queue
  std::deque<double> mpXValues;

  //!@brief x values of plot, stored as vector (needed to pass this to qwt plot)
  std::vector<double> mXArray;

  //!@brief number of steps in the past, which are still plotted
  size_t mMaxHistorySize; //!@todo Make a parameter/configurable somehow.

  //! A vector containing all the colors used for plot lines.
  static std::vector<QColor> mLineColors;

  //! A vector containing all the line stypes for the plot.
  static std::vector<Qt::PenStyle> mLineStyles;

}; // class cedar::aux::gui::HistoryPlot0D

#endif // CEDAR_AUX_GUI_HISTORY_PLOT_0D_H
