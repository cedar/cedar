/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

#include "cedar/configuration.h"

#ifdef CEDAR_USE_QWT

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/MultiPlotInterface.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/auxiliaries/DoubleData.fwd.h"
#include "cedar/auxiliaries/UnitData.fwd.h"
#include "cedar/auxiliaries/gui/HistoryPlot0D.fwd.h"

// SYSTEM INCLUDES
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#ifndef Q_MOC_RUN
  #include <boost/date_time.hpp>
#endif
#include <deque>
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
        /* This is an internal class of ImagePlot that cannot be nested because Qt's moc doesn't support nested classes.
         *
         * Don't use it outside of the ImagePlot!
         */
        class HistoryPlot0DWorker : public QObject
        {
          Q_OBJECT

          public:
            HistoryPlot0DWorker(cedar::aux::gui::HistoryPlot0D* pPlot)
            :
            mpPlot(pPlot)
            {
            }

          public slots:
            void convert();

          signals:
            void done();

          public:
            cedar::aux::gui::HistoryPlot0D *mpPlot;
        };
        CEDAR_GENERATE_POINTER_TYPES(HistoryPlot0DWorker);
      }
    }
  }
}
//!@endcond

/*!@brief A time-based plot for 0D values. Displays a history of this value from a certain point in the past up to now.
 *
 * More detailed description of the class.
 */
class cedar::aux::gui::HistoryPlot0D : public cedar::aux::gui::MultiPlotInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::gui::detail::HistoryPlot0DWorker;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  struct CurveInfo
  {
    CurveInfo();

    ~CurveInfo();

    void setData(cedar::aux::ConstDataPtr data);

    double getDataValue() const;

    //! The data of this curve, as generic data pointer
    cedar::aux::ConstDataPtr mData;

    //! The data of this curve, as double data
    //!@todo Can this be removed? I think that by now, double data inherits MatData...
    cedar::aux::ConstDoubleDataPtr mDoubleData;

    //! The data of this curve, as matrix data
    cedar::aux::ConstMatDataPtr mMatData;

    //! The data of this curve, as matrix data
    cedar::aux::ConstUnitDataPtr mUnitData;

    //! The qwt curve
    QwtPlotCurve* mCurve;

    //! y values of plot, stored as double-ended queue
    std::deque<double> mXValues;

    //! y values of plot, stored as vector (needed to pass this to qwt plot because deque is not stored linearly)
    std::vector<double> mXArray;

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
  HistoryPlot0D(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget *pParent = NULL);

  //!@brief Destructor
  ~HistoryPlot0D();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief display data
  void plot(cedar::aux::ConstDataPtr data, const std::string& title);

  //!@brief handle timer events
  void timerEvent(QTimerEvent *pEvent);

  //!@brief Check if the given data can be appended to the plot.
  bool canAppend(cedar::aux::ConstDataPtr data) const;
  //!@brief Check if the given data can be detached from the plot.
  bool canDetach(cedar::aux::ConstDataPtr data) const;

signals:
  //!@brief Signals the worker thread to convert the data to the plot's internal format.
  void convert();

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

  void doAppend(cedar::aux::ConstDataPtr data, const std::string& title);
  void doDetach(cedar::aux::ConstDataPtr data);

  double getDataValue(size_t index);

  void applyStyle(size_t lineId, QwtPlotCurve *pCurve);

private slots:
  void showLegend(bool show = true);
  void conversionDone();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  std::vector<CurveInfoPtr> mCurves;

  //!@brief the current widget that holds the plot
  QWidget* mpCurrentPlotWidget;

  //!@brief a plot
  QwtPlot* mpPlot;

  //!@brief number of steps in the past, which are still plotted
  size_t mMaxHistorySize;

  //! Thread in which conversion of mat data to qwt triple is done.
  QThread* mpWorkerThread;

  //! Worker object.
  cedar::aux::gui::detail::HistoryPlot0DWorkerPtr mWorker;

  //! A vector containing all the colors used for plot lines.
  static std::vector<QColor> mLineColors;

  //! A vector containing all the line stypes for the plot.
  static std::vector<Qt::PenStyle> mLineStyles;

  static boost::posix_time::ptime mPlotStartTime;

}; // class cedar::aux::gui::HistoryPlot0D

#endif // CEDAR_USE_QWT
#endif // CEDAR_AUX_GUI_HISTORY_PLOT_0D_H
