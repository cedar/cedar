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

    File:        PlotData.h

    Maintainer:  Kai Kuchenbecker
    Email:       Kai.Kuchenbecker@ini.rub.de
    Date:        2013-08-02

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_PLOT_DATA_H
#define CEDAR_PROC_PLOT_DATA_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/DataRole.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/BoolParameter.h"

// FORWARD DECLARATIONS
#include "cedar/processing/PlotData.fwd.h"

// SYSTEM INCLUDES

/*!@brief Holds information about data in a custom step plot.
 *
 */
class cedar::proc::PlotData : public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! Constructor.
   *
   * @param id Role of the data to be plotted.
   * @param name Name of the data slot of the step whose data is to be plotted.
   * @param ignoreIfMissing If true, no warning will be generated if the slot is missing.
   */
  PlotData
  (
    cedar::proc::DataRole::Id id = cedar::proc::DataRole::OUTPUT,
    const std::string& name = "",
    bool ignoreIfMissing = false,
    const std::string& plotDeclaration = ""
  );

  /*! Copy-Constructor.
   */
  PlotData(const cedar::proc::PlotData& other);


  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! sets a plot declaration
  void setPlotDeclaration(const std::string& newDeclaration);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

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
  //! Role of the data to be plotted.
  cedar::aux::EnumParameterPtr mId;
  //! Name of the data to be plotted.
  cedar::aux::StringParameterPtr mName;
  //! If true, no exception will be thrown if the data cannot be found.
  cedar::aux::BoolParameterPtr mIgnoreIfMissing;
  //! Plot Type to be used to plot the data
  cedar::aux::StringParameterPtr mPlotDeclaration;

}; // cedar::proc::PlotData

#endif // CEDAR_PROC_PLOT_DATA_H
