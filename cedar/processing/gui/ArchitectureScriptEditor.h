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

    File:        ArchitectureScriptEditor.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 11 21

    Description: Header file for the class cedar::proc::gui::ArchitectureScriptEditor.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_ARCHITECTURE_SCRIPT_EDITOR_H
#define CEDAR_PROC_GUI_ARCHITECTURE_SCRIPT_EDITOR_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ui_ArchitectureScriptEditor.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/ArchitectureScriptEditor.fwd.h"
#include "cedar/processing/gui/Group.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <map>

namespace cedar
{
  namespace proc
  {
    namespace gui
    {
      namespace detail
      {
        /*!@brief A widget for script control.
         */
        class ScriptControlWidget : public QWidget
        {
        Q_OBJECT

        public:
          //!@brief constructor
          ScriptControlWidget(cedar::proc::CppScriptPtr script);

        private slots:
          void runScript();

          void requestScriptStop();

          void scriptStarted();

          void scriptStopped();

        private:
          void updateScriptRunningState();
          void updateScriptRunningState(bool running);

        private:
          cedar::proc::CppScriptPtr mScript;

          QPushButton* mpStartButton;
          QPushButton* mpStopButton;
        };
      }
    }
  }
}

/*!@brief A widget for editing scripts associated with an architecture.
 */
class cedar::proc::gui::ArchitectureScriptEditor : public QWidget, Ui_ArchitectureScriptEditor
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
  ArchitectureScriptEditor(cedar::proc::gui::GroupPtr group, QWidget *pParent = nullptr);

signals:
  //! signaling that a script was added to a group
  void scriptAddedInGroup(QString scriptName);

  //! signaling that a script was removed from a group
  void scriptRemovedFromGroup(QString scriptName);

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
  void fillTypeComboBox();

  void setComboBoxItemEnabled(int index, bool enabled);

  void translateScriptAddedSignal(const std::string& scriptName);

  void translateScriptRemovedSignal(const std::string& scriptName);

  void showItemProperties(QTableWidgetItem* pItem);

  cedar::proc::CppScriptPtr getScriptFromItem(QTableWidgetItem* pItem) const;

  void refreshScriptList();

  cedar::proc::gui::GroupPtr getGroup() const;

private slots:
  void addClicked();

  void removeClicked();

  void addScriptToList(const QString& scriptName);

  void removeScriptFromList(const QString& scriptName);

  void itemSelected();

  void scriptNameChanged();

  void scriptStatusChanged(QString newStatus);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::proc::gui::GroupWeakPtr mGroup;

  boost::signals2::scoped_connection mConnectionScriptAdded;

  boost::signals2::scoped_connection mConnectionScriptRemoved;

  std::map<cedar::aux::Parameter*, std::string> mUsedParameterNames;

  const static int M_NAME_COL;
  const static int M_TYPE_COL;
  const static int M_STATUS_COL;
  const static int M_CTRL_COL;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::ArchitectureScriptEditor

#endif // CEDAR_PROC_GUI_ARCHITECTURE_SCRIPT_EDITOR_H

