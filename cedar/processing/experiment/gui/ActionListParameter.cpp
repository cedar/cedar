/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ActionListParameter.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2014 07 02

    Description: Source file for the class cedar::proc::experiment::gui::ActionListParameter.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/gui/ActionListParameter.h"
#include "cedar/processing/experiment/gui/ExperimentItemWidget.h"
#include "cedar/processing/experiment/action/StartAllTriggers.h"
#include "cedar/auxiliaries/gui/ObjectParameter.h"

// SYSTEM INCLUDES
#include <QBoxLayout>
#include <QPushButton>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::experiment::gui::ActionListParameter::ActionListParameter(QWidget* parent)
:
cedar::aux::gui::Parameter(parent)
,
mActionListParameter
(
  new cedar::proc::experiment::action::Action::ActionListParameter
  (
    NULL,
    "",
    std::vector<cedar::proc::experiment::action::ActionPtr>()
  )
)
,
mActionListWidget(new ActionListWidget(this))
{
  mActionListWidget->setDragDropMode(QAbstractItemView::InternalMove);
  mActionListWidget->setDefaultDropAction(Qt::MoveAction);
  mActionListWidget->setAlternatingRowColors(true);
  mActionListWidget->setStyleSheet("QListWidget {"
                            "               background-color: rgb(230,230,210);"
                            "               alternate-background-color: rgb(200,200,180);"
                            "               selection-background-color: rgb(240,210,210);"
                            "            }"
                            "");
  this->setLayout(new QVBoxLayout);
  this->layout()->setMargin(0);
  this->layout()->addWidget(mActionListWidget);
  QPushButton* add = new QPushButton();
  add->setText(QString::fromStdString("Add Action"));
  this->layout()->addWidget(add);
  connect(add,SIGNAL(clicked()),this,SLOT(addAction()));
  connect(this,SIGNAL(parameterPointerChanged()),this,SLOT(parameterPointerChanged()));
}

cedar::proc::experiment::gui::ActionListParameter::~ActionListParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::experiment::gui::ActionListParameter::addAction()
{
  cedar::proc::experiment::action::ActionPtr action =
      cedar::proc::experiment::action ::StartAllTriggersPtr(new cedar::proc::experiment::action::StartAllTriggers());
  this->mActionListParameter->pushBack(action);
  updateList();
}


void cedar::proc::experiment::gui::ActionListParameter::updateList()
{
  mActionListWidget->clear();
  for(unsigned int i=0; i < mActionListParameter->size(); i++)
  {

    cedar::proc::experiment::action::ActionPtr action = mActionListParameter->at(i);
    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(QSize(0,45));
    QWidget* itemWidget = new QWidget;
    QHBoxLayout* itemWidgetLayout = new QHBoxLayout();
    itemWidgetLayout->setMargin(5);
    itemWidget->setLayout(itemWidgetLayout);


    QPushButton* remove = new QPushButton();
    remove->setText(QString::fromStdString("-"));
    remove->setFixedSize(35,25);
    remove->setObjectName(QString::number(i));
    itemWidgetLayout->addWidget(remove);
    connect(remove,SIGNAL(clicked()),this,SLOT(actionParameterRemoved()));

    cedar::aux::ObjectParameterPtr actionParameter
        (
            new cedar::proc::experiment::action::Action::ActionParameter(NULL,std::to_string(i),action)

        );
    cedar::aux::gui::Parameter* parameterWidget = new cedar::aux::gui::ObjectParameter();
    parameterWidget->setParameter(actionParameter);
    itemWidgetLayout->addWidget(parameterWidget);

    connect(actionParameter.get(),SIGNAL(valueChanged()),this,SLOT(actionParameterChanged()));
    auto itemProperties = new cedar::proc::experiment::gui::ExperimentItemWidget();
    itemProperties->display(action);
    itemWidgetLayout->addWidget(itemProperties);

    this->mActionListWidget->addItem(item);
    this->mActionListWidget->setItemWidget(item,itemWidget);
  }
  this->mActionListWidget->setMinimumHeight(mActionListParameter->size()*45+5);
}

void cedar::proc::experiment::gui::ActionListParameter::parameterPointerChanged()
{
  this->mActionListParameter =
      boost::dynamic_pointer_cast<cedar::proc::experiment::action::Action::ActionListParameter>(this->getParameter());
  updateList();
}
void cedar::proc::experiment::gui::ActionListParameter::actionParameterChanged()
{
  auto sender = dynamic_cast<cedar::aux::ObjectParameter*>(QObject::sender());
  int number;
  std::istringstream ( sender->getName() ) >> number;
  this->mActionListParameter->removeObject(number);
  auto action = boost::dynamic_pointer_cast<cedar::proc::experiment::action::Action>(sender->getConfigurable());
  this->mActionListParameter->insert(number,action);
  this->updateList();
}
void cedar::proc::experiment::gui::ActionListParameter::actionParameterRemoved()
{
  auto sender = dynamic_cast<QPushButton*>(QObject::sender());
  int number = sender->objectName().toInt();
  this->mActionListParameter->removeObject(number);
  this->updateList();
}
void cedar::proc::experiment::gui::ActionListParameter::ActionListWidget::dragEnterEvent(QDragEnterEvent* event)
{
  QListWidget::dragEnterEvent(event);
  this->mStartDragPos = this->currentRow();
}

void cedar::proc::experiment::gui::ActionListParameter::ActionListWidget::dropEvent(QDropEvent* event)
{
  QListWidget::dropEvent(event);
  cedar::proc::experiment::action::ActionPtr action = this->mpParent->mActionListParameter->at(mStartDragPos);
  this->mpParent->mActionListParameter->removeObject(mStartDragPos);
  this->mpParent->mActionListParameter->insert(this->currentRow(), action);
}

