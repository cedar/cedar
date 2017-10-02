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

    File:        FindDialog.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2014 06 23

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/FindDialog.h"
#include "cedar/processing/gui/GraphicsBase.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/gui/Ide.h"
#include "cedar/processing/gui/View.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/Element.h"
#include "cedar/auxiliaries/gui/Parameter.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/TypeBasedFactory.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <QHeaderView>


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::FindDialog::FindDialog(QWidget* parent, cedar::proc::gui::View* pView)
:
QDialog(parent),
mpBar(new QStatusBar(this)),
mpView(pView),
mpLastHighlightedItem(nullptr)
{
  this->setupUi(this);
  this->mpElementClass->setVisible(false);
  this->mpElementClass->setEnabled(false);
  this->mpFindNext->setEnabled(false);
  this->mpStatusLayout->addWidget(mpBar);
  this->mpBar->setSizeGripEnabled(false);
  QObject::connect(mpElementName, SIGNAL(textChanged(QString)), this, SLOT(searchStringChangedChanged()));
  QObject::connect(mpElementName, SIGNAL(textChanged(QString)), this, SLOT(checkButtonAvailability()));
  QObject::connect(mpElementName, SIGNAL(textChanged(QString)), mpBar, SLOT(clearMessage()));
  QObject::connect(mpFindNext, SIGNAL(pressed()), this, SLOT(findNext()));
}

cedar::proc::gui::FindDialog::~FindDialog()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::gui::FindDialog::searchStringChangedChanged()
{
  this->mSearchStringChanged = true;
}

void cedar::proc::gui::FindDialog::closeEvent(QCloseEvent* event)
{
  this->mSearchStringChanged = true;
  mpElementName->setText(QString(""));
  if (this->mpLastHighlightedItem)
  {
    this->mpLastHighlightedItem->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_NONE);
    this->mpLastHighlightedItem = nullptr;
  }
  this->mpView->getScene()->selectNone();
  QDialog::closeEvent(event);
}

void cedar::proc::gui::FindDialog::findNext()
{
  auto group = static_cast<cedar::proc::gui::Ide*>(this->parent())->getGroup();
  if (this->mSearchStringChanged)
  {
    mFoundElements = group->getGroup()->findElementsAcrossGroupsContainsString(mpElementName->text().toStdString());
    mFindIndex = 0;
    this->mSearchStringChanged = false;
  }
  else
  {
    ++mFindIndex;
  }
  if (mFindIndex >= mFoundElements.size())
  {
    mFindIndex = 0;
  }
  if (mFoundElements.size() > 0)
  {
    if (auto item = mpView->getScene()->getGraphicsItemFor(mFoundElements.at(mFindIndex).get()))
    {
      // first, reset selection and highlight
      this->mpView->getScene()->selectNone();
      if (this->mpLastHighlightedItem)
      {
        this->mpLastHighlightedItem->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_NONE);
      }
      this->mpLastHighlightedItem = item;
      // then, select, highlight, and center new result
      item->setSelected(true);
      item->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_SEARCH_RESULT);
      this->mpView->centerOn(item);
    }
  }
  else
  {
    this->mpBar->showMessage(QString("No element found!"));
    this->mpView->getScene()->selectNone();
    if (this->mpLastHighlightedItem)
    {
      this->mpLastHighlightedItem->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_NONE);
      this->mpLastHighlightedItem = nullptr;
    }
  }
}

void cedar::proc::gui::FindDialog::checkButtonAvailability()
{
  if (!this->mpElementName->text().isEmpty())
  {
    this->mpFindNext->setEnabled(true);
  }
  else
  {
    this->mpFindNext->setEnabled(false);
  }
}
