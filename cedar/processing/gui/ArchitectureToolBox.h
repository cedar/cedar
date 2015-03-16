/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ArchitectureToolBox.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 11

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_ARCHITECTURE_TOOLBOX_H
#define CEDAR_PROC_GUI_ARCHITECTURE_TOOLBOX_H

// CEDAR INCLUDES
#include "cedar/processing/gui/ToolBox.h"
#include "cedar/processing/gui/Scene.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/ArchitectureToolBox.fwd.h"

// SYSTEM INCLUDES


/*!@brief This is the toolbox that contains the architecture tools.
 *
 *        This widget can be found in cedar's GUI.
 */
class cedar::proc::gui::ArchitectureToolBox : public cedar::proc::gui::ToolBox
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ArchitectureToolBox(QWidget *pParent = NULL);

  //!@brief Destructor.
  ~ArchitectureToolBox();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Sets the view that is controlled by this toolbox.
   */
  void setView(cedar::proc::gui::View *pView);

public slots:
  /*!@brief Slot that reacts to a change in the selected tool.
   */
  void selectionChanged(QString data);

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
  //! View that is controlled by this toolbox.
  cedar::proc::gui::View *mpView;

}; // class cedar::proc::gui::ArchitectureToolBox

#endif // CEDAR_PROC_GUI_ARCHITECTURE_TOOLBOX_H
