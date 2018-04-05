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

    File:        ConnectorItem.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 11 12

    Description: Header file for the class cedar::proc::gui::ConnectorItem.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_CONNECTOR_ITEM_H
#define CEDAR_PROC_GUI_CONNECTOR_ITEM_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/GraphicsBase.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/ConnectorItem.fwd.h"
#include "cedar/processing/sinks/GroupSink.fwd.h"
#include "cedar/processing/sources/GroupSource.fwd.h"

// SYSTEM INCLUDES


/*!@brief A graphics item that corresponds to a connector.
 */
class cedar::proc::gui::ConnectorItem : public cedar::proc::gui::GraphicsBase
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The constructor for a GroupSource.
  ConnectorItem(cedar::proc::sources::GroupSourcePtr source);

  //!@brief The constructor for a GroupSink.
  ConnectorItem(cedar::proc::sinks::GroupSinkPtr sink);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  bool canDuplicate() const;

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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::ConnectorItem

#endif // CEDAR_PROC_GUI_CONNECTOR_ITEM_H

