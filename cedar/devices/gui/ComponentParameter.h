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

    File:        ComponentParameter.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 03 05

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_GUI_COMPONENT_PARAMETER_H
#define CEDAR_DEV_GUI_COMPONENT_PARAMETER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/gui/namespace.h"
#include "cedar/devices/namespace.h"
#include "cedar/auxiliaries/gui/Parameter.h"

// SYSTEM INCLUDES
#include <QLineEdit>
#include <QTreeWidgetItem>


/*!@brief GUI representation of a cedar::dev::ComponentParameter.
 */
class cedar::dev::gui::ComponentParameter : public cedar::aux::gui::Parameter
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
  ComponentParameter();

  //!@brief Destructor
  virtual ~ComponentParameter();

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
  void selectComponent(cedar::dev::ComponentSlotPtr slot);

private slots:
  void openComponentSelector();

  void updatePathText();

  void parameterChanged();

  void componentDoubleClicked();

  void reselect(const std::string &robot_name, const std::string &slot_name);

private:
  void fillRobots(QTreeWidgetItem* pItem);
  void fillComponents(QTreeWidgetItem* pItem, cedar::dev::RobotPtr robot);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  QLineEdit* mpComponentPathDisplay;

  cedar::dev::ComponentParameterPtr mParameter;

  QDialog* mpComponentDialog;

  QTreeWidget* mpComponentTree;

}; // class cedar::dev::gui::ComponentParameter

#endif // CEDAR_DEV_GUI_COMPONENT_PARAMETER_H

