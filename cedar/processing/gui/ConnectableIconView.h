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

    File:        ConnectableIconView.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 08 01

    Description: Header file for the class cedar::proc::gui::ConnectableIconView.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_CONNECTABLE_ICON_VIEW_H
#define CEDAR_PROC_GUI_CONNECTABLE_ICON_VIEW_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/processing/gui/ConnectableIconView.fwd.h"
#include "cedar/processing/Connectable.fwd.h"

// SYSTEM INCLUDES
#include <QGraphicsItem>


/*!@brief A class that can be used to specify what an icon for a step should look like.
 */
class cedar::proc::gui::ConnectableIconView : public QGraphicsItem
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ConnectableIconView();

  //!@brief Destructor
  virtual ~ConnectableIconView();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! set a connectable
  void setConnectable(cedar::proc::ConstConnectablePtr connectable);

  //! get connectable
  cedar::proc::ConstConnectablePtr getConnectable() const;

  //! set bounding box position and size
  virtual void setBounds(const qreal& x, const qreal& y, const qreal& size) = 0;

  //! Prepares the view for writing out to SVG.
  virtual void prepareSvgExport();

  //! Undoes the changes needed for writing out to SVG.
  virtual void unprepareSvgExport();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! handles a change in connectable
  virtual void connectableChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::proc::ConstConnectablePtr mConnectable;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::ConnectableIconView

#endif // CEDAR_PROC_GUI_CONNECTABLE_ICON_VIEW_H

