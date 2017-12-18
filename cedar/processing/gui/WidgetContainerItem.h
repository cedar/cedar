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

    File:        WidgetContainerItem.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2015 01 29

    Description: Header file for the class cedar::proc::gui::WidgetContainerItem.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_WIDGET_CONTAINER_ITEM_H
#define CEDAR_PROC_GUI_WIDGET_CONTAINER_ITEM_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/GraphicsBase.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/WidgetContainerItem.fwd.h"

// SYSTEM INCLUDES
#include <QGraphicsProxyWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QObject>

/*!@brief A base class for including window-like widgets in a graphics scene
 */
class cedar::proc::gui::WidgetContainerItem : public QObject, public cedar::proc::gui::GraphicsBase
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  WidgetContainerItem();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void setWidget(QWidget* widget);

  bool canDuplicate() const;

  bool canResize() const;

  void setTitle(const std::string& title);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void sizeChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! The graphics proxy containing the widget
  QGraphicsProxyWidget* mpContainer;

  //! The contained widget
  QWidget* mpContained;

  //! Used for displaying the title
  QLabel* mpTitleWidget;

  //! Layout of the title bar
  QHBoxLayout* mpTitleBarLayout;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::WidgetContainerItem

#endif // CEDAR_PROC_GUI_WIDGET_CONTAINER_ITEM_H

