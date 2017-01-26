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

    File:        MatDataPlot.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 08 28

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_MAT_DATA_PLOT_H
#define CEDAR_AUX_GUI_MAT_DATA_PLOT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/MultiPlotInterface.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/auxiliaries/gui/MatDataPlot.fwd.h"

// SYSTEM INCLUDES


/*!@brief A generic plot for cedar::aux::MatData.
 *
 *        This plot automatically selects the proper plot for matrix data based on its annotation and other factors.
 */
class cedar::aux::gui::MatDataPlot : public cedar::aux::gui::MultiPlotInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  MatDataPlot(QWidget *pParent = NULL);

  //! Constructor that immediately plots data.
  MatDataPlot(cedar::aux::ConstDataPtr data, const std::string& title, QWidget *pParent = NULL);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Display a MatData.
  void plot(cedar::aux::ConstDataPtr data, const std::string& title);

  //!@brief Check if the given data can be appended to the plot.
  bool canAppend(cedar::aux::ConstDataPtr data) const;

  //!@brief Check if the given data can be detached from the plot.
  bool canDetach(cedar::aux::ConstDataPtr data) const;

  void writeConfiguration(cedar::aux::ConfigurationNode& configuration) const;

  void readConfiguration(const cedar::aux::ConfigurationNode& configuration);

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

  void initLayout();

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

}; // class cedar::aux::gui::MatDataPlot

#endif // CEDAR_AUX_GUI_MAT_DATA_PLOT_H
