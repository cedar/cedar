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

    File:        PlotInterface.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 28

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_PLOT_INTERFACE_H
#define CEDAR_AUX_GUI_PLOT_INTERFACE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/Data.fwd.h"
#include "cedar/auxiliaries/gui/PlotInterface.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <map>

/*!@brief A unified interface for widgets that plot instances of cedar::proc::Data.
 */
class cedar::aux::gui::PlotInterface : public QWidget, public cedar::aux::Configurable
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
  PlotInterface(QWidget *pParent = NULL);

  //!@brief Destructor
  virtual ~PlotInterface();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Displays the data in the plot widget.
   *
   *        This is the main method that is called whenever a data plotter is requested to plot a different data.
   *
   * @param data  The data to display.
   * @param title the title of the plot window
   *
   */
  virtual void plot(cedar::aux::ConstDataPtr data, const std::string& title) = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

signals:
  //! Signal that is emitted when the plotter detects a change in data that it cannot handle.
  void dataChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

}; // class cedar::aux::gui::PlotInterface

#endif // CEDAR_AUX_GUI_PLOT_INTERFACE_H
