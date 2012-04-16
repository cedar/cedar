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

#ifndef CEDAR_AUX_GUI_MATRIX_PLOT_H
#define CEDAR_AUX_GUI_MATRIX_PLOT_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/namespace.h"
#include "cedar/auxiliaries/gui/MultiPlotInterface.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QReadWriteLock>
#include <opencv2/opencv.hpp>
#include <qwtplot3d/qwt3d_types.h>

/*!@brief Base class for plots that can display matrices.
 *
 * @todo Write more detailed description of the class here.
 */
class cedar::aux::gui::MatrixPlot : public cedar::aux::gui::MultiPlotInterface
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
  MatrixPlot(QWidget *pParent = NULL);

  //!@brief Destructor
  ~MatrixPlot();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief display a MatData
  void plot(cedar::aux::DataPtr data, const std::string& title);

  bool canAppend(cedar::aux::ConstDataPtr data) const;

  //!@brief return vector of standard colors
  static const Qwt3D::ColorVector& getStandardColorVector();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void doAppend(cedar::aux::DataPtr data, const std::string& title);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief the displayed MatData
  cedar::aux::MatDataPtr mData;
  //!@brief the plot widget
  QWidget *mpCurrentPlotWidget;
  //!@brief window title
  QString mWindowTitle;
  //!@brief vector filled with standard colors
  static Qwt3D::ColorVector mStandardColorVector;
}; // class cedar::aux::gui::MatrixPlot

#endif // CEDAR_AUX_GUI_MATRIX_PLOT_H
