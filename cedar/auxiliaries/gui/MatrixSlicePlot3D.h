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

    File:        MatrixSlicePlot3D.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2012 05 29

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_MATRIX_SLICE_PLOT_3D_H
#define CEDAR_AUX_GUI_MATRIX_SLICE_PLOT_3D_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/PlotInterface.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/auxiliaries/gui/MatrixSlicePlot3D.fwd.h"

// SYSTEM INCLUDES
#include <QLabel>
#include <QReadWriteLock>
#include <opencv2/opencv.hpp>


//!@cond SKIPPED_DOCUMENTATION
namespace cedar
{
  namespace aux
  {
    namespace gui
    {
      namespace detail
      {
        /* This is an internal class of MatrixSlicePlot3D that cannot be nested because Qt's moc doesn't support nested classes.
         *
         * Don't use it outside of the MatrixSlicePlot3D!
         */
        class MatrixSlicePlot3DWorker : public QObject
        {
          Q_OBJECT

          public:
            MatrixSlicePlot3DWorker(cedar::aux::gui::MatrixSlicePlot3D* pPlot)
            :
            mpPlot(pPlot)
            {
            }

          public slots:
            void convert();

          signals:
            void done();

          public:
            cedar::aux::gui::MatrixSlicePlot3D *mpPlot;
        };
        CEDAR_GENERATE_POINTER_TYPES(MatrixSlicePlot3DWorker);
      }
    }
  }
}
//!@endcond


/*!@brief A slice-plot for 3D matrices.
 */
class cedar::aux::gui::MatrixSlicePlot3D : public cedar::aux::gui::PlotInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::gui::detail::MatrixSlicePlot3DWorker;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The constructor.
  MatrixSlicePlot3D(QWidget* pParent = NULL);

  //!@brief A constructor taking both a data pointer and a title.
  MatrixSlicePlot3D(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget* pParent = NULL);

  //!@brief Destructor
  ~MatrixSlicePlot3D();
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Displays the data.
   *
   * @param data A pointer to the data to display. If this isn't a pointer to a cedar::aux::MatData, the function
   *             throws.
   * @param title title of the plot window
   */
  void plot(cedar::aux::ConstDataPtr data, const std::string& title);

  /*!@brief Updates the plot periodically.
   */
  void timerEvent(QTimerEvent *pEvent);

signals:
  //!@brief Signals the worker thread to convert the data to the plot's internal format.
  void convert();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief Reacts to a resize of the plot.
   */
  void resizeEvent(QResizeEvent *event);

  /*!@brief Processes key events.
   *
   * This function handles ctrl+G, which saves the window settings.
   */
  virtual void keyPressEvent(QKeyEvent* pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Creates the image based on the matrix.
   */
  void slicesFromMat(const cv::Mat& mat);

  /*!@brief Resizes the pixmap used to display the image data.
   */
  void resizePixmap();

  void updateData();

  //! initialize the widget
  void init();

private slots:
  void conversionDone();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Label used for displaying the image.
  QLabel* mpImageDisplay;

  //! Data displayed by the plot.
  cedar::aux::ConstMatDataPtr mData;

  //! Converted image.
  QImage mImage;

  //! Lock for mImage.
  QReadWriteLock mImageLock;

  //! Id of the timer used for updating the plot.
  int mTimerId;

  //! Thread in which conversion of mat data to qwt triple is done.
  QThread* mpWorkerThread;

  //! Worker object.
  cedar::aux::gui::detail::MatrixSlicePlot3DWorkerPtr mWorker;

  cv::Mat mSliceMatrix;
  cv::Mat mSliceMatrixByte;
  cv::Mat mSliceMatrixByteC3;
  cv::Mat mSliceSize;
  bool mDataIsSet;

  //! desired columns of the slice plot
  unsigned int mDesiredColumns;

  //! True if the plot is currently converting the data to the internal format. Used to skip overlapping timer events.
  bool mConverting;

}; // class cedar::aux::gui::MatrixSlicePlot3D

#endif // CEDAR_AUX_GUI_MATRIX_SLICE_PLOT_3D_H
