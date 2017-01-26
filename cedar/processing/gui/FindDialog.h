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

    File:        FindDialog.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2014 06 23

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_FIND_DIALOG_H
#define CEDAR_PROC_GUI_FIND_DIALOG_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ui_FindDialog.h"

// FORWARD DECLARATIONS
#include "cedar/processing/Element.fwd.h"
#include "cedar/processing/Group.fwd.h"
#include "cedar/processing/gui/FindDialog.fwd.h"
#include "cedar/processing/gui/GraphicsBase.fwd.h"
#include "cedar/processing/gui/Scene.fwd.h"
#include "cedar/processing/gui/View.fwd.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/signals2.hpp>
#endif
#include <vector>
#include <QStatusBar>


/*!@brief A widget for conveniently controlling the boosts in a network.
 */
class cedar::proc::gui::FindDialog : public QDialog, public Ui_FindDialog
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
  FindDialog(QWidget* parent, cedar::proc::gui::View* pView);

  //!@brief The destructor.
  ~FindDialog();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@ clean up the search when closing the window
  void closeEvent(QCloseEvent* event);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:


private slots:

  //!
  void searchStringChangedChanged();
  void checkButtonAvailability();
  void findNext();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  QStatusBar* mpBar;
  cedar::proc::gui::View* mpView;
  std::vector<cedar::proc::ConstElementPtr> mFoundElements;
  bool mSearchStringChanged;
  unsigned int mFindIndex;
  cedar::proc::gui::GraphicsBase* mpLastHighlightedItem;

}; // class cedar::proc::gui::FindDialog

#endif // CEDAR_PROC_GUI_FIND_DIALOG_H

