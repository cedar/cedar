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

    File:        ArchitectureConsistencyCheck.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 06 20

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_ARCHITECTURE_CONSISTENCY_CHECK_H
#define CEDAR_PROC_GUI_ARCHITECTURE_CONSISTENCY_CHECK_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/namespace.h"
#include "cedar/processing/gui/ui_ArchitectureConsistencyCheck.h"
#include "cedar/processing/consistency/ConsistencyIssue.h"

// SYSTEM INCLUDES
#include <QWidget>


/*!@brief Widget that alerts users about potential flaws in their architecture
 */
class cedar::proc::gui::ArchitectureConsistencyCheck : public QWidget, public Ui_ConsistencyCheck
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
  ArchitectureConsistencyCheck(cedar::proc::gui::View* pView,cedar::proc::gui::Scene* pScene);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Sets the network whose consistency is to be checked.
  void setNetwork(cedar::proc::gui::NetworkPtr network);

  //! Clears all issues.
  void clear();

public slots:
  //! Performs all checks again.
  void recheck();

  //! Performs an action for the given item, if applicable.
  void itemAction(int row, int column);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*! Displays the given issue.
   *
   * @param issueId Index of the issue in the issue vector.
   * @param issue The issue to display.
   */
  void addIssue(size_t issueId, cedar::proc::ConsistencyIssuePtr issue);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Network being checked.
  cedar::proc::gui::NetworkPtr mNetwork;

  //! Scene in which the network is being displayed.
  cedar::proc::gui::Scene* mpScene;

  //! View displaying the scene.
  cedar::proc::gui::View* mpView;

  //! Vector of issues.
  std::vector<cedar::proc::ConsistencyIssuePtr> mIssues;

}; // class cedar::proc::gui::ArchitectureConsistencyCheck

#endif // CEDAR_PROC_GUI_ARCHITECTURE_CONSISTENCY_CHECK_H
