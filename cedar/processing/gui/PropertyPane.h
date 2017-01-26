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

    File:        PropertyPane.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 03 09

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_PROPERTY_PANE_H
#define CEDAR_PROC_PROPERTY_PANE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/PropertyPane.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/PropertyPane.fwd.h"

// SYSTEM INCLUDES
#include <string>


/*!@brief A widget for displaying the parameters of configurables.
 *
 *        This widget offers a quick way of creating a user interface for classes that implement the
 *        cedar::aux::Configurable interface. Parameters of the configurable are displayed in a tabular widget by their
 *        name and have an edit widget associated with them.
 */
class cedar::proc::gui::PropertyPane : public cedar::aux::gui::PropertyPane
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The standard constructor.
   */
  PropertyPane(QWidget *pParent = NULL);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  std::string getInstanceTypeId(cedar::aux::ConfigurablePtr pConfigurable) const;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

}; // class cedar::proc::gui::PropertyPane

#endif // CEDAR_PROC_PROPERTY_PANE_H
