/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ImagePlot.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 22

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_IMAGE_PLOT_H
#define CEDAR_AUX_GUI_IMAGE_PLOT_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/namespace.h"
#include "cedar/auxiliaries/gui/PlotInterface.h"
#include "cedar/auxiliaries/annotation/namespace.h"

// SYSTEM INCLUDES
#include <QLabel>
#include <QReadWriteLock>
#include <opencv2/opencv.hpp>
#include <qwtplot3d/qwt3d_types.h>


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
        class ImagePlotWorker : public QObject
        {
          Q_OBJECT

          public:
            ImagePlotWorker(cedar::aux::gui::ImagePlot* pPlot)
            :
            mpPlot(pPlot)
            {
            }

          public slots:
            void convert();

          signals:
            void done();

          public:
            cedar::aux::gui::ImagePlot *mpPlot;
        };
        CEDAR_GENERATE_POINTER_TYPES(ImagePlotWorker);
      }
    }
  }
}
//!@endcond

/*!@brief A plot for images.
 */
class cedar::aux::gui::ImagePlot : public cedar::aux::gui::PlotInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::gui::detail::ImagePlotWorker;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! Enum for quickly accessing the type of the data displayed by the viewer.
  enum DataType
  {
    DATA_TYPE_IMAGE,
    DATA_TYPE_MAT,
    DATA_TYPE_UNKNOWN
  };

  //! Widget used for displaying the image.
  class ImageDisplay : public QLabel
  {
    public:
      ImageDisplay(const QString& text);

    protected:
      void mousePressEvent(QMouseEvent * pEvent);

    public:
      cedar::aux::ConstMatDataPtr mData;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ImagePlot(QWidget *pParent = NULL);

  //!@brief Destructor.
  ~ImagePlot();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Displays the data.
   *
   * @param data A pointer to the data to display. If this isn't a pointer to a cedar::aux::ImageData, the function
   *             throws.
   * @param title title of the plot window
   */
  void plot(cedar::aux::ConstDataPtr data, const std::string& title);

  /*!@brief Updates the plot periodically.
   */
  void timerEvent(QTimerEvent *pEvent);

  /*!@brief Set the scaling mode of the plot.
   */
  void setSmoothScaling(bool smooth);

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

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Resizes the pixmap used to display the image data.
   */
  void resizePixmap();

  /*!@brief Converts a one-channel input matrix to a three-channel matrix that contains the one-channel matrix in all
   *        channels.
   */
  cv::Mat threeChannelGrayscale(const cv::Mat& in) const;

  /*!@brief Creates the image based on the matrix.
   */
  void imageFromMat(const cv::Mat& mat);

private slots:
  void conversionDone();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Label used for displaying the image.
  cedar::aux::gui::ImagePlot::ImageDisplay *mpImageDisplay;

  //! Data displayed by the plot.
  cedar::aux::ConstMatDataPtr mData;

  //! The color space annotation of the data (if present).
  cedar::aux::annotation::ConstColorSpacePtr mDataColorSpace;

  //! Converted image.
  QImage mImage;

  //! Lock for mImage.
  QReadWriteLock mImageLock;

  //! Id of the timer used for updating the plot.
  int mTimerId;

  //! Type of the data.
  DataType mDataType;

  //! Thread in which conversion of mat data to qwt triple is done.
  QThread* mpWorkerThread;

  //! Worker object.
  cedar::aux::gui::detail::ImagePlotWorkerPtr mWorker;

  //! True if the plot is currently converting the data to the internal format. Used to skip overlapping timer events.
  bool mConverting;

  //! Whether the matrix should be smoothed during scaling.
  bool mSmoothScaling;

  static std::vector<char> mLookupTableR;
  static std::vector<char> mLookupTableG;
  static std::vector<char> mLookupTableB;

}; // class cedar::aux::gui::ImagePlot
#endif // CEDAR_AUX_GUI_IMAGE_PLOT_H
