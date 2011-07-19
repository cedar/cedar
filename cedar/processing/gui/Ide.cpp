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
#include "processing/gui/NetworkFile.h"
#include "processing/Manager.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::gui::Ide::Ide()
{
  this->setupUi(this);
  this->resetStepList();

  this->mpArchitectureToolBox->setView(this->mpProcessingDrawer);
  this->mpProcessingDrawer->getScene()->setMainWindow(this);

  QObject::connect(this->mpProcessingDrawer->getScene(), SIGNAL(selectionChanged()), this, SLOT(sceneItemSelected()));
  QObject::connect(this->mpProcessingDrawer->getScene(), SIGNAL(exception(const QString&)),
                   this, SLOT(exception(const QString&)));
  QObject::connect(this->mpProcessingDrawer->getScene(), SIGNAL(modeFinished()),
                   this, SLOT(architectureToolFinished()));
  QObject::connect(this->mpThreadsStartAll, SIGNAL(triggered()), this, SLOT(startThreads()));
  QObject::connect(this->mpThreadsStopAll, SIGNAL(triggered()), this, SLOT(stopThreads()));
  QObject::connect(this->mpActionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));

  mNetwork = cedar::proc::gui::NetworkFilePtr(new cedar::proc::gui::NetworkFile(this->mpProcessingDrawer->getScene()));
  this->resetTo(mNetwork);
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::Ide::resetTo(cedar::proc::gui::NetworkFilePtr network)
{
  this->mpProcessingDrawer->getScene()->setNetwork(network);
  //!@todo Implement me!
}

void cedar::proc::gui::Ide::architectureToolFinished()
{
  this->mpArchitectureToolBox->selectMode("mode.Select");
}

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
  //!@ todo Handle the cases: multiple
  if (selected_items.size() == 1)
  {
    cedar::proc::gui::StepItem *p_drawer = dynamic_cast<cedar::proc::gui::StepItem*>(selected_items[0]);
    if (p_drawer)
    {
      this->mpPropertyTable->display(p_drawer->getStep());
    }
  }
}

void cedar::proc::gui::Ide::exception(const QString& message)
{
  QMessageBox::critical(this,
                        "An exception has occurred.",
                        message);
}

void cedar::proc::gui::Ide::startThreads()
{
  cedar::proc::Manager::getInstance().startThreads();
  this->mpThreadsStartAll->setEnabled(false);
  this->mpThreadsStopAll->setEnabled(true);
}

void cedar::proc::gui::Ide::stopThreads()
{
  cedar::proc::Manager::getInstance().stopThreads();
  this->mpThreadsStartAll->setEnabled(true);
  this->mpThreadsStopAll->setEnabled(false);
}

void cedar::proc::gui::Ide::saveAs()
{
  QString file = QFileDialog::getSaveFileName(this, // parent
                                              "Select where to save", // caption
                                              "", // initial directory; //!@todo save/restore with window settings
                                              "json (*.json)" // filter(s), separated by ';;'
                                              );

  if (!file.isEmpty())
  {
    this->mNetwork->save(file.toStdString());
  }
}
