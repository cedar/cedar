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

#ifndef CEDAR_AUX_GUI_QT5_MATRIX_PLOT_H
#define CEDAR_AUX_GUI_QT5_MATRIX_PLOT_H

#include "cedar/configuration.h"
#ifdef CEDAR_USE_QT5_PLOTS

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/MultiPlotInterface.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/auxiliaries/gui/ColorValueRGBA.fwd.h"
#include "cedar/auxiliaries/gui/Qt5MatrixPlot.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QReadWriteLock>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

/*!@brief A class used for plotting with the Qwt and QwtPlot3D libraries.
 *
 * @todo This code is highly redundant with MatDataPlot, MatrixPlot and VtkMatrixPlot. Unify them.
 */
class cedar::aux::gui::Qt5MatrixPlot : public cedar::aux::gui::MultiPlotInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Qt5MatrixPlot(QWidget *pParent = NULL);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief display a MatData
  void plot(cedar::aux::ConstDataPtr data, const std::string& title);
  //!@brief Check if the given data can be appended to the plot.
  bool canAppend(cedar::aux::ConstDataPtr data) const;
  //!@brief Check if the given data can be detached from the plot.
  bool canDetach(cedar::aux::ConstDataPtr data) const;

  void readConfiguration(const cedar::aux::ConfigurationNode& configuration);

  void writeConfiguration(cedar::aux::ConfigurationNode& configuration) const;

public slots:
  /*!@brief Reacts to a change in the plotted data.
   *
   * When the dimensionality of the plotted data changes, this causes a switch of the plot type.
   */
  void processChangedData();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void doAppend(cedar::aux::ConstDataPtr data, const std::string& title);
  void doDetach(cedar::aux::ConstDataPtr data);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief the displayed MatData
  cedar::aux::ConstMatDataPtr mData;

  //!@brief the plot widget
  QWidget* mpCurrentPlotWidget;

  std::string mTitle;

}; // class cedar::aux::gui::Qt5MatrixPlot

#endif // CEDAR_USE_QT5_PLOTS
#endif // CEDAR_AUX_GUI_QT5_MATRIX_PLOT_H
