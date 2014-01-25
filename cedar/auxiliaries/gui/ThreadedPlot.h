/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ThreadedPlot.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 01 25

    Description: Header file for the class cedar::aux::gui::ThreadedPlot.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_THREADED_PLOT_H
#define CEDAR_AUX_GUI_THREADED_PLOT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/PlotInterface.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/ThreadedPlot.fwd.h"

// SYSTEM INCLUDES
#include <QObject>
#include <QThread>


namespace cedar
{
  namespace aux
  {
    namespace gui
    {
      namespace detail
      {
        //!@cond SKIPPED_DOCUMENTATION
        /* This is an internal class of ThreadedPlot that cannot be nested because Qt's moc doesn't support nested classes.
        *
        * Don't use it outside of the ThreadedPlot!
        */
        class ThreadedPlotWorker : public QObject
        {
          Q_OBJECT

        public:
          ThreadedPlotWorker(cedar::aux::gui::ThreadedPlot* pPlot)
          :
          mpPlot(pPlot)
          {
          }

          public slots :
            void convert();

        signals:
          void conversionDone();

          void conversionFailed();

          void minMaxChanged(double min, double max);

        public:
          cedar::aux::gui::ThreadedPlot *mpPlot;
        };
        CEDAR_GENERATE_POINTER_TYPES(ThreadedPlotWorker);
        //!@endcond
      } // namespace detail
    }
  }
}

/*!@brief A base class for plots that convert data in a separate thread.
 *
 * @todo The inheritance order of PlotInterface/ThreadedPlot might have to be turned around, as it might otherwise be impossible to use this class with MultiPlotInterface.
 */
class cedar::aux::gui::ThreadedPlot : public cedar::aux::gui::PlotInterface
{
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::gui::detail::ThreadedPlotWorker;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ThreadedPlot(QWidget* pParent = NULL);

  //! Destructor.
  ~ThreadedPlot();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

signals :
  void convert();

  void conversionFailedSignal();

  void conversionDoneSignal();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void startConversion();

  void timerEvent(QTimerEvent* /* pEvent */);

  void start();

  void stop();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*! Method in which child classes should do their conversions. Called outside the GUI thread.
   *
   * @return True, if the conversion was successful, false otherwise.
   */
  virtual bool doConversion() = 0;

  /*! Called in the GUI thread when the conversion is done. Child classes should update their display in this function.
   *
   */
  virtual void updatePlot() = 0;

private slots:
  void conversionDone();

  void conversionFailed();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Thread in which conversion of mat data to qwt triple is done.
  QThread* mpWorkerThread;

  //! Worker object.
  cedar::aux::gui::detail::ThreadedPlotWorkerPtr mWorker;

  //! True if the plot is currently converting the data to the internal format. Used to skip overlapping timer events.
  bool mConverting;

  //! Id of the timer used for updating the plot.
  int mTimerId;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::gui::ThreadedPlot

#endif // CEDAR_AUX_GUI_THREADED_PLOT_H

