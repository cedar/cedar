/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Ide.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 05

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "Ide.h"
#include "processing/gui/Scene.h"
#include "processing/gui/StepItem.h"
#include "processing/gui/StepClassList.h"
#include "processing/Manager.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QLabel>
#include <QMessageBox>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::gui::Ide::Ide()
{
  this->setupUi(this);
  this->resetStepList();

  QObject::connect(this->mpProcessingDrawer->getScene(), SIGNAL(selectionChanged()), this, SLOT(sceneItemSelected()));
  QObject::connect(this->mpProcessingDrawer->getScene(), SIGNAL(exception(const QString&)),
                   this, SLOT(exception(const QString&)));
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::Ide::resetStepList()
{
  using cedar::proc::Manager;

  this->mpCategoryList->clear();

  for (Manager::StepRegistry::CategoryList::const_iterator iter = Manager::getInstance().steps().getCategories().begin();
       iter != Manager::getInstance().steps().getCategories().end();
       ++iter)
  {
    const std::string& category_name = *iter;
    cedar::proc::gui::StepClassList *p_tab = new cedar::proc::gui::StepClassList();
    this->mpCategoryList->addTab(p_tab, QString(category_name.c_str()));
    mStepClassListWidgets[category_name] = p_tab;
    p_tab->showList(Manager::getInstance().steps().getCategoryEntries(category_name));
  }
}

void cedar::proc::gui::Ide::sceneItemSelected()
{
  using cedar::proc::Step;
  QList<QGraphicsItem *> selected_items = this->mpProcessingDrawer->getScene()->selectedItems();

  this->mpPropertyTable->clearContents();
  this->mpPropertyTable->setRowCount(0);
  //!@ todo Handle the cases: 0 (!), multiple
  if (selected_items.size() == 1)
  {
    cedar::proc::gui::StepItem *p_drawer = dynamic_cast<cedar::proc::gui::StepItem*>(selected_items[0]);
    int row = this->mpPropertyTable->rowCount();
    cedar::proc::Step::ParameterMap& parameters = p_drawer->getStep()->getParameters();
    for (Step::ParameterMap::iterator iter = parameters.begin(); iter != parameters.end(); ++iter)
    {
      cedar::proc::ParameterBasePtr& parameter = iter->second;
      this->mpPropertyTable->insertRow(row);
      QLabel *p_label = new QLabel();
      p_label->setText(parameter->getName().c_str());
      this->mpPropertyTable->setCellWidget(row, 0, p_label);
    }
  }
}

void cedar::proc::gui::Ide::exception(const QString& message)
{
  QMessageBox::critical(this,
                        "An exception has occurred.",
                        message);
}
