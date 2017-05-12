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

    File:        GroupParameterDesigner.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 02 27

    Description: Header file for the class cedar::proc::gui::GroupParameterDesigner.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_GROUP_PARAMETER_DESIGNER_H
#define CEDAR_PROC_GUI_GROUP_PARAMETER_DESIGNER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ui_GroupParameterDesigner.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/GroupParameterDesigner.fwd.h"
#include "cedar/processing/Group.fwd.h"
#include "cedar/auxiliaries/Parameter.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QComboBox>
#include <vector>
#include <string>


/*!@brief A widget for editing parameters of groups.
 */
class cedar::proc::gui::GroupParameterDesigner : public QWidget, public Ui_GroupParameterDesigner
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The constructor.
  GroupParameterDesigner(cedar::proc::GroupPtr group);

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
  void fillParameterTypeBox();

  void addParameter(const std::string& type);

  void addParameterToList(cedar::aux::ParameterPtr parameter);

  void translateCustomParameterAdded(cedar::aux::ParameterPtr);

  void translateCustomParameterRemoved(cedar::aux::ParameterPtr);

  cedar::aux::ParameterPtr parameterFromItem(QTreeWidgetItem* pItem) const;

signals:
  //! signal that a parameter was added
  void customParameterAdded(QVariant pointer);

  //! signal that a parameter was removed
  void customParameterRemoved(QVariant pointer);

private slots:
  //! react if a parameter was added
  void customParameterAddedSlot(QVariant pointer);

  //! react if a parameter was removed
  void customParameterRemovedSlot(QVariant pointer);

  void addClicked();

  void deleteClicked();

  void itemChanged(QTreeWidgetItem* item, int column);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::proc::GroupPtr mGroup;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::GroupParameterDesigner

#endif // CEDAR_PROC_GUI_GROUP_PARAMETER_DESIGNER_H

