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

    File:        MultiPlotInterface.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 27

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_MULTI_PLOT_INTERFACE_H
#define CEDAR_AUX_GUI_MULTI_PLOT_INTERFACE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/namespace.h"
#include "cedar/auxiliaries/gui/PlotInterface.h"

// SYSTEM INCLUDES


/*!@brief An interface that indicates that a plot is capable of plotting more than one data object at a time.
 */
class cedar::aux::gui::MultiPlotInterface : public cedar::aux::gui::PlotInterface
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
  //!@brief a map that stores data and associated names/titles of their plots
  typedef std::map<cedar::aux::ConstDataPtr, std::string> DataMap;
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  MultiPlotInterface(QWidget *pParent = NULL);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Append the given data to the plot.
   *
   * @param data  The data to append.
   * @param title The tilte (legend entry) of the data object.
   */
  void append(cedar::aux::ConstDataPtr data, const std::string& title);

  //!@brief check if given data can be appended to a plot
  virtual bool canAppend(cedar::aux::ConstDataPtr data) const = 0;

  //!@brief returns the data map
  const DataMap& getDataMap() const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! Actual implementation of the appending is deferred to the sub-classes.
  virtual void doAppend(cedar::aux::ConstDataPtr data, const std::string& title) = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  DataMap mDataMap;
}; // class cedar::aux::gui::MultiPlotInterface

#endif // CEDAR_AUX_GUI_MULTI_PLOT_INTERFACE_H
