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

    File:        QC2DMatrixPlot.h

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2023 09 05

    Description: Header file for the class cedar::aux::gui::QC2DMatrixPlot.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_QC2DMATRIX_PLOT_H
#define CEDAR_AUX_GUI_QC2DMATRIX_PLOT_H

#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/MultiPlotInterface.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/auxiliaries/gui/ColorValueRGBA.fwd.h"
#include "cedar/auxiliaries/gui/QC2DMatrixPlot.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QReadWriteLock>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

/*!@brief A class used for plotting 2D plots with the QCustomPlot library.
 *
 * @todo This code is highly redundant with QCMatrixPlot. Unify them.
 */
class cedar::aux::gui::QC2DMatrixPlot : public cedar::aux::gui::MultiPlotInterface
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
  QC2DMatrixPlot(QWidget *pParent = NULL);

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

}; // class cedar::aux::gui::QC2DMatrixPlot

#endif // CEDAR_AUX_GUI_QC2DMATRIX_PLOT_H

