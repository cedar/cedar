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

    File:        MatrixVectorPlot.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2012 07 11

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_MATRIX_VECTOR_PLOT_H
#define CEDAR_AUX_GUI_MATRIX_VECTOR_PLOT_H

#include "cedar/configuration.h"

#ifdef CEDAR_USE_QWT

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/MultiPlotInterface.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/auxiliaries/gui/MatrixVectorPlot.fwd.h"
#include "cedar/auxiliaries/math/Limits.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QReadWriteLock>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <opencv2/opencv.hpp>
#include <vector>

/*!@brief Matrix plot that can display 1D matrices (i.e. vectors) as vectors.
 */
class cedar::aux::gui::MatrixVectorPlot : public cedar::aux::gui::MultiPlotInterface
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
      mpCurve->detach();
    }

    //!@brief the displayed data
    cedar::aux::ConstMatDataPtr mMatData;
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
  MatrixVectorPlot(QWidget *pParent = NULL);

  //!@brief Constructor expecting a DataPtr.
  MatrixVectorPlot(cedar::aux::DataPtr matData, const std::string& title, QWidget *pParent = NULL);

  //!@brief Destructor
  ~MatrixVectorPlot();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief display data
  void plot(cedar::aux::ConstDataPtr matData, const std::string& title);
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
  bool canDetach(cedar::aux::ConstDataPtr data) const;

  //!@brief Returns the limits of the x axis.
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

  void doAppend(cedar::aux::ConstDataPtr data, const std::string& title);
  void doDetach(cedar::aux::ConstDataPtr data);

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

}; // class cedar::aux::gui::MatrixVectorPlot

#endif // CEDAR_USE_QWT
#endif // CEDAR_AUX_GUI_MATRIX_VECTOR_PLOT_H
