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

#ifndef CEDAR_AUX_GUI_MATRIX_PLOT_1D_H
#define CEDAR_AUX_GUI_MATRIX_PLOT_1D_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/namespace.h"
#include "cedar/auxiliaries/gui/MultiPlotInterface.h"
#include "cedar/auxiliaries/math/namespace.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QReadWriteLock>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <opencv2/opencv.hpp>

/*!@brief Matrix plot that can display 1D matrices (i.e. vectors).
 *
 * @todo Write more detailed description of the class here.
 */
class cedar::aux::gui::MatrixPlot1D : public cedar::aux::gui::MultiPlotInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  struct PlotSeries
  {
    PlotSeries()
    :
    mpCurve(NULL)
    {
    }

    ~PlotSeries()
    {
    }

    //!@brief the displayed data
    cedar::aux::MatDataPtr mMatData;
    //!@brief a curve inside the plot
    QwtPlotCurve *mpCurve;
    //!@brief the x values of the plot
    std::vector<double> mXValues;
    //!@brief the y values of the plot
    std::vector<double> mYValues;
  };

  CEDAR_GENERATE_POINTER_TYPES(PlotSeries);

  typedef std::vector<PlotSeriesPtr> PlotSeriesVector;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  MatrixPlot1D(QWidget *pParent = NULL);

  //!@brief Constructor expecting a DataPtr.
  MatrixPlot1D(cedar::aux::DataPtr matData, const std::string& title, QWidget *pParent = NULL);

  //!@brief Destructor
  ~MatrixPlot1D();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief display data
  void plot(cedar::aux::DataPtr matData, const std::string& title);
  //!@brief handle timer events
  void timerEvent(QTimerEvent *pEvent);

  /*!
   * @remarks This method is a temporary way to provide annotations, a more general one will be added soon.
   */
  void attachMarker(QwtPlotMarker *pMarker);

  /*!@brief Detaches and deletes all markers added to this plot.
   *
   * @remarks This method is a temporary way to provide annotations, a more general one will be added soon.
   */
  void clearMarkers();

  bool canAppend(cedar::aux::ConstDataPtr data) const;

  //!@brief Returns the limits of the x axis.
  //!@todo Generalize this for n-dimensional matrix plots.
  cedar::aux::math::Limits<double> getXLimits() const;

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
  //!@brief initialize
  void init();

  //!@brief (Re-)initializes the x and y value arrays.
  void buildArrays(PlotSeriesPtr series, unsigned int new_size);

  void doAppend(cedar::aux::DataPtr data, const std::string& title);

  //!@brief Applies a plot style to a given curve.
  static void applyStyle(size_t lineId, QwtPlotCurve *pCurve);

private slots:
  void showLegend(bool show = true);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief a plot
  QwtPlot *mpPlot;

  PlotSeriesVector mPlotSeriesVector;

  //! For locking the plot itself.
  QReadWriteLock *mpLock;

  //! A vector containing all the colors used for plot lines.
  static std::vector<QColor> mLineColors;

  //! A vector containing all the line stypes for the plot.
  static std::vector<Qt::PenStyle> mLineStyles;

}; // class cedar::aux::gui::MatrixPlot1D

#endif // CEDAR_AUX_GUI_MATRIX_PLOT_1D_H
