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

    File:        DataPlotter.h

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

#ifndef CEDAR_PROC_GUI_DATA_PLOTTER_H
#define CEDAR_PROC_GUI_DATA_PLOTTER_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/MultiPlotInterface.h"
#include "cedar/auxiliaries/Data.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/TypeBasedFactory.fwd.h"
#include "cedar/auxiliaries/gui/DataPlotter.fwd.h"

// SYSTEM INCLUDES
#include <QDockWidget>
#include <string>

/*!@brief Base class for a dockable plot.
 *
 * This class decides, which plot fits best the given data and instantiate a plot of the right type.
 */
class cedar::aux::gui::DataPlotter : public cedar::aux::gui::MultiPlotInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

public:
  //!@brief a factory, which is currently not used
  typedef cedar::aux::TypeBasedFactory<cedar::aux::DataPtr, boost::shared_ptr<QWidget> > WidgetFactory;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  DataPlotter(QWidget *pParent = NULL);

  //!@brief Destructor
  ~DataPlotter();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief plot data
  void plot(cedar::aux::ConstDataPtr data, const std::string& title);

  //!@brief check if given data can be added to a plot
  bool canAppend(cedar::aux::ConstDataPtr data) const;

public slots:
  //!@brief slot that induces a redraw if data changes
  void dataChanged();

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

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief the displayed data
  cedar::aux::ConstDataPtr mData;

  //!@brief The title of the data being displayed.
  std::string mTitle;

  //!@brief Pointer to the current plot.
  cedar::aux::gui::PlotInterface *mpCurrentPlot;

}; // class cedar::aux::gui::DataPlotter

#endif // CEDAR_PROC_GUI_DATA_PLOTTER_H
