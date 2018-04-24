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

    File:        GroupWidget.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 11 19

    Description: Header file for the class cedar::proc::gui::GroupWidget.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_GROUP_WIDGET_H
#define CEDAR_PROC_GUI_GROUP_WIDGET_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/processing/gui/GroupWidget.fwd.h"
#include "cedar/processing/gui/Group.fwd.h"
#include "cedar/processing/gui/Scene.fwd.h"
#include "cedar/processing/gui/View.fwd.h"
#include "cedar/processing/gui/RecorderWidget.fwd.h"
#include "cedar/processing/Group.fwd.h"
#include "cedar/auxiliaries/gui/Configurable.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>


/*!@brief A widget for managing and displaying a cedar::proc::gui::Group.
 *
 * @todo cedar::proc::gui::View could probably be rewritten to serve the same purpose as this class. Right now, however,
 *       it is very difficult to use; this widget is an easy-to-use alternative to it.
 */
class cedar::proc::gui::GroupWidget : public QWidget
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  GroupWidget(QWidget* pParent = nullptr);

  //!@brief The constructor for pre-existing groups.
  GroupWidget(cedar::proc::gui::Group* pGroup, QWidget* pParent = nullptr);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! get the group
  cedar::proc::gui::GroupPtr getGroup() const;

  /*!@brief Sets the widget used for displaying/editing the parameters of configurables in the group.
   */
  void setConfigurableWidget(cedar::aux::gui::Configurable* pConfigurableWidget);

  /*!@brief Sets the widget used for displaying/editing the record parameters in the group.
   */
  void setRecorderWidget(cedar::proc::gui::RecorderWidget* pRecorderWidget);

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
  cedar::proc::gui::GroupPtr mGroup;

  cedar::proc::gui::View* mpView;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::GroupWidget

#endif // CEDAR_PROC_GUI_GROUP_WIDGET_H

