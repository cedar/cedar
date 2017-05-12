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

    File:        QImagePlot.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 01 24

    Description: Header file for the class cedar::aux::gui::QImagePlot.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_QIMAGE_PLOT_H
#define CEDAR_AUX_GUI_QIMAGE_PLOT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/PlotInterface.h"
#include "cedar/auxiliaries/gui/ThreadedPlot.h"
#include "cedar/auxiliaries/math/DoubleLimitsParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/ColorGradient.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/annotation/ValueRangeHint.fwd.h"
#include "cedar/auxiliaries/gui/QImagePlot.fwd.h"

// SYSTEM INCLUDES
#include <QMenu>
#include <QLabel>
#include <QDir>
#include <QLinearGradient>
#include <QReadWriteLock>
#include <opencv2/opencv.hpp>


namespace cedar
{
  namespace aux
  {
    namespace gui
    {
      namespace detail
      {
        //!@cond SKIPPED_DOCUMENTATION
        /* This is an internal class of QImagePlot that cannot be nested because Qt's moc doesn't support nested classes.
         *
         * Don't use it outside of the QImagePlot!
         *
         *
         * Class for displaying a legend for the image plot worker.
         */
        class QImagePlotLegend : public QWidget
        {
          Q_OBJECT

        public:
          QImagePlotLegend();

          public slots:
          // Updates the minimum and maximum value displayed by the legend.
          void updateMinMax(double min, double max);

          // Applies the colors of the graident to the legend.
          void setGradient(cedar::aux::ColorGradientPtr gradient);

        private:
          QLabel* mpMin;
          QLabel* mpMax;

          QLinearGradient mGradient;

          QFrame* mpGradientDisplay;
        };
        //!@endcond
      } // namespace detail
    }
  }
}

/*!@brief A base class for plots that use QImages as a displaying device.
 */
class cedar::aux::gui::QImagePlot : public cedar::aux::gui::ThreadedPlot
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  class ImageDisplay;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  QImagePlot(QWidget* pParent = NULL);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------

public:
  /*! Sets fixed limits for the plot values.
   */
  void setLimits(double min, double max);

  /*! Whether the scale of the plot is determined automatically.
   */
  bool isAutoScaling() const
  {
    return this->_mAutoScaling->getValue();
  }

  /*! Whether the plot is pixelated or smoothed.
   */
  bool isSmoothScaling() const
  {
    return this->_mSmoothScaling->getValue();
  }

  //! Returns whether the plot is set to keep the aspect ratio of the original matrix.
  bool keepAspectRatio() const;

  /*! Returns the currently set value limits.
   */
  const cedar::aux::math::Limits<double>& getValueLimits() const
  {
    return this->_mValueLimits->getValue();
  }

  //! Returns the currently active color jet.
  cedar::aux::ColorGradient::StandardGradients::Id getColorJet() const;

  void plot(cedar::aux::ConstDataPtr data, const std::string& title);

public slots:
  /*!@brief Set the scaling mode of the plot.
  */
  void setSmoothScaling(bool smooth);

  //! Sets whether the plot is to keep the aspect ratio of the original matrix.
  void setKeepAspectRatio(bool keepAspectRatio);

  //! Toggles the visibility of the legend.
  void showLegend(bool show);

signals:
  //! Signals a change in the minimal and maximal values in the plotted data.
  void minMaxChanged(double min, double max);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief Reacts to a resize of the plot.
  */
  void resizeEvent(QResizeEvent *event);

  //!@brief create and handle the context menu
  void contextMenuEvent(QContextMenuEvent *pEvent);

  //! Prints a message instead of plotting the image.
  void setInfo(const std::string& text);

  void updatePlot();

  /*! Set the matrix to be displayed.
   *  @remarks This only has effect after updateImage() is called from the GUI threat.
   */
  void displayMatrix(const cv::Mat& matrix);

  //! Notifies the image plot if a legend can be displayed or not.
  void setLegendAvailable(bool available)
  {
    this->mLegendAvailable = available;
  }

  //! If set to true, fixed values can be set for the minimum and maximum of the plot.
  void setValueScalingEnabled(bool enabled)
  {
    mValueScalingAvailable = enabled;
  }

  //! Colorizes the matrix.
  cv::Mat colorizeMatrix(const cv::Mat& toColorize);

  //! Colorizes the matrix with the given minimum and maximum.
  cv::Mat colorizeMatrix(const cv::Mat& toColorize, bool applyLimits, double min, double max) const;

protected slots:
  //! Updates the minimum and maximum of the plot.
  void updateMinMax(double min, double max);

  //! Enables automatic scaling.
  void setAutomaticScaling();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Resizes the pixmap used to display the image data.
  */
  void resizePixmap();

  virtual void fillContextMenu(QMenu& menu);

  virtual void plotClicked(QMouseEvent* pEvent, double relativeImageX, double relativeImageY);

  void setColorJet(cedar::aux::ColorGradientPtr gradient);

private slots:
  void queryFixedValueScale();

  //! Updates the visibility of the legend.
  void showLegendChanged();
  
  void valueLimitsChanged();

  void colorJetChanged();

  void colorJetActionTriggered();

  void saveImageActionTriggered();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! Label used for displaying the image.
  ImageDisplay* mpImageDisplay;

private:
  //! Converted image.
  QImage mImage;

  //! Lock for mImage.
  QReadWriteLock mImageLock;

  //! Whether or not a legend is available
  bool mLegendAvailable;

  //! Whether the plot can scale its values according to user-defined limits.
  bool mValueScalingAvailable;

  //! Legend (if any).
  cedar::aux::gui::detail::QImagePlotLegend* mpLegend;

  //! The color gradient to be used.
  cedar::aux::ColorGradientPtr mColorGradient;

  //! If not null, this is used to determine the range of the plot.
  cedar::aux::annotation::ConstValueRangeHintPtr mValueHint;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! Whether the matrix should be smoothed during scaling.
  cedar::aux::BoolParameterPtr _mSmoothScaling;

  //! Whether the aspect ratio of the matrix should be preserved in the plot.
  cedar::aux::BoolParameterPtr _mKeepAspectRatio;

  //! Whether scaling of plots is determined automatically or fixed.
  cedar::aux::BoolParameterPtr _mAutoScaling;

  //! Whether scaling of plots is determined automatically or fixed.
  cedar::aux::BoolParameterPtr _mShowLegend;

  //! Limits for fixed scaling.
  cedar::aux::math::DoubleLimitsParameterPtr _mValueLimits;

  //! Color scale to use.
  cedar::aux::EnumParameterPtr _mColorJet;

  //! Static member to remember the location of the last-saved image.
  static cedar::aux::LockableMember<QDir> mLastSaveLocation;

}; // class cedar::aux::gui::QImagePlot

#endif // CEDAR_AUX_GUI_QIMAGE_PLOT_H

