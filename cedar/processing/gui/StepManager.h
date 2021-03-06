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

    File:        UiSettings.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 01 31

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_STEP_MANAGER_H
#define CEDAR_PROC_GUI_STEP_MANAGER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ui_StepManager.h"
#include "cedar/processing/GroupDeclaration.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/GroupDeclarationManager.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/StepManager.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QSignalMapper>
#include <QCheckBox>
#include <cedar/auxiliaries/DeclarationManagerTemplate.h>


/*!@brief A widget for displaying the settings stored in cedar::proc::gui::Settings.
 */
class cedar::proc::gui::StepManager : public QWidget, public Ui_StepManager
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
  StepManager(QWidget* pParent = NULL);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! reject settings and load settings
  //void reject();
  //! accept settings and save them
  //void accept();

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
  void fillTable();
  void fillPresetLists();

signals:
  void checkBoxToggled(const QString &fullClassName);

private slots:
  void handleCheckBoxToggle(QString fullClassName);

  void handleComboBoxSelection(const QString presetName);
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  QSignalMapper* checkBoxMapper;
  std::map<std::string,std::vector<std::string>> mPreSets;
  std::map<std::string,QCheckBox*> mCheckBoxes;
  const std::string mCustomSetName;
  // none yet

}; // class cedar::proc::gui::UiSettings

class GroupOrElementDeclaration
{
public:
    bool isGroup;
    cedar::proc::ConstGroupDeclaration* groupDeclaration;
    cedar::proc::ConstElementDeclaration* elementDeclaration;
};

#endif //CEDAR_PROC_GUI_STEP_MANAGER_H

