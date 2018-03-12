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

    File:        ArchitectureConsistencyCheck.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 06 20

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ArchitectureConsistencyCheck.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/View.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/consistency/LoopedElementNotConnected.h"
#include "cedar/processing/consistency/LoopedElementInNonLoopedGroup.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::ArchitectureConsistencyCheck::ArchitectureConsistencyCheck
                                                (
                                                  cedar::proc::gui::View* pView,
                                                  cedar::proc::gui::Scene* pScene
                                                )
:
mpScene(pScene),
mpView(pView)
{
  this->setupUi(this);

  QObject::connect(this->mpRecheckBtn, SIGNAL(clicked()), this, SLOT(recheck()));
  QObject::connect(this->mpIssueTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(itemAction(int, int)));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::proc::gui::ArchitectureConsistencyCheck::setGroup(cedar::proc::gui::GroupPtr network)
{
  this->mGroup = network;
  this->clear();
}

void cedar::proc::gui::ArchitectureConsistencyCheck::recheck()
{
  this->mIssues = this->mGroup->getGroup()->checkConsistency();

  this->clear();

  if (this->mIssues.size() > 0)
  {
    for (size_t issue_id = 0; issue_id < this->mIssues.size(); ++issue_id)
    {
      auto issue = this->mIssues.at(issue_id);
      this->addIssue(issue_id, issue);
    }
  }
  else
  {
    int row = this->mpIssueTable->rowCount();
    this->mpIssueTable->setRowCount(row + 1);
    this->mpIssueTable->setItem(row, 1, new QTableWidgetItem("No issues found"));
  }
}

void cedar::proc::gui::ArchitectureConsistencyCheck::clear()
{
  this->mpIssueTable->setRowCount(0);
}

void cedar::proc::gui::ArchitectureConsistencyCheck::addIssue(size_t issueId, cedar::proc::ConsistencyIssuePtr issue)
{
  int row = this->mpIssueTable->rowCount();
  this->mpIssueTable->setRowCount(row + 1);
  this->mpIssueTable->setItem(row, 0, new QTableWidgetItem(QString("%1").arg(issueId)));
  this->mpIssueTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(issue->getDescription())));
}

void cedar::proc::gui::ArchitectureConsistencyCheck::itemAction(int row, int)
{
  if (this->mIssues.size() == 0)
  {
    // no issues found, nothing to do
    return;
  }

  size_t issue_id = static_cast<size_t>(row);
  CEDAR_ASSERT(issue_id < this->mIssues.size());
  auto issue = this->mIssues.at(issue_id);
  if (auto looped_not_connected = boost::dynamic_pointer_cast<cedar::proc::LoopedElementNotConnected>(issue))
  {
    this->mpView->centerOn(mpScene->getGraphicsItemFor(looped_not_connected->getUnconnectedElement().get()));
  }
  else if (auto looped_not_connected = boost::dynamic_pointer_cast<cedar::proc::LoopedElementInNonLoopedGroup>(issue))
  {
    this->mpView->centerOn(mpScene->getGraphicsItemFor(looped_not_connected->getUnconnectedElement().get()));
  }
}
