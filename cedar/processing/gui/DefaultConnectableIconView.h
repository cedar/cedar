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

    File:        DefaultConnectableIconView.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 08 01

    Description: Header file for the class cedar::proc::gui::DefaultConnectableIconView.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_DEFAULT_CONNECTABLE_ICON_VIEW_H
#define CEDAR_PROC_GUI_DEFAULT_CONNECTABLE_ICON_VIEW_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ConnectableIconView.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/DefaultConnectableIconView.fwd.h"

// SYSTEM INCLUDES
#include <QGraphicsSvgItem>


/*!@brief Displays a static svg icon based on the icon in the connectable's declaration.
 */
class cedar::proc::gui::DefaultConnectableIconView : public cedar::proc::gui::ConnectableIconView
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  DefaultConnectableIconView();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! returns the bounding rectangle covering the icon
  QRectF boundingRect() const;

  //! sets the bounding box (position and size)
  void setBounds(const qreal& x, const qreal& y, const qreal& size);

  //! paints the icon
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr);

  //! Prepares the item for writing out to SVG.
  void prepareSvgExport();

  //! Undoes the changes needed for writing out to SVG.
  void unprepareSvgExport();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! sets the path to the icon
  void setIconPath(const QString& path);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! handles a change in connectable
  void connectableChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! SvgItem displaying the connectable's icon
  QGraphicsSvgItem* mpIconDisplay;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::DefaultConnectableIconView

#endif // CEDAR_PROC_GUI_DEFAULT_CONNECTABLE_ICON_VIEW_H

