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

    File:        AdvancedParameterLinker.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 03 04

    Description: Header file for the class cedar::proc::gui::AdvancedParameterLinker.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_ADVANCED_PARAMETER_LINKER_H
#define CEDAR_PROC_GUI_ADVANCED_PARAMETER_LINKER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ui_AdvancedParameterLinker.h"
#include "cedar/processing/Group.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/AdvancedParameterLinker.fwd.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>


/*!@brief A widget for defining parameter links within a group and all its subgroups.
 */
class cedar::proc::gui::AdvancedParameterLinker : public QWidget, public Ui_AdvancedParameterLinker
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
  AdvancedParameterLinker();

  ~AdvancedParameterLinker();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief sets the group which is edited by the parameter linker
  void setGroup(cedar::proc::GroupPtr group);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void addGroup(QTreeWidgetItem* item, cedar::proc::GroupPtr group);

  void makeGroupItem(QTreeWidgetItem* pItem, cedar::proc::GroupPtr group);

  void show(const cedar::proc::Group::ParameterLinkInfo& linkInfo);

  cedar::proc::Group::ParameterLinkInfo& getCurrentLinkInfo() const;

  cedar::proc::Group* getCurrentGroup() const;

  void fillLinkTypes();

  void updateLinkItem
  (
    QTreeWidgetItem* pItem,
    const cedar::proc::Group::ParameterLinkInfo& linkInfo
  );

  void parameterLinkAdded(const cedar::proc::Group::ParameterLinkInfo& linkInfo);

  void parameterLinkRemoved(cedar::aux::ParameterLinkPtr link);

  void disconnect();

  QTreeWidgetItem* getItemForGroup(cedar::proc::GroupPtr group);

  QTreeWidgetItem* getItemForLink(cedar::aux::ParameterLinkPtr link);

  void fillParameterNameCompletions(cedar::aux::ConfigurablePtr configurable, QStringList& completions);

private slots:
  void itemSelectionChanged();

  void linkInfoChanged();

  void addLinkClicked();

  void removeLinkClicked();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::proc::GroupPtr mGroup;

  std::vector<boost::signals2::connection> mSignalConnections;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::AdvancedParameterLinker

#endif // CEDAR_PROC_GUI_ADVANCED_PARAMETER_LINKER_H

